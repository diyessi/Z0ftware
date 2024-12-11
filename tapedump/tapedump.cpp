// MIT License
//
// Copyright (c) 2023 Scott Cyphers
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// https://www.piercefuller.com/oldibm-shadow/709x.html
// https://www.piercefuller.com/library/magtape7.html

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/card.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/disasm.hpp"
#include "Z0ftware/parity.hpp"
#include "Z0ftware/tape.hpp"
#include "Z0ftware/word.hpp"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace {

llvm::cl::list<std::string> inputFileNames(llvm::cl::Positional,
                                           llvm::cl::desc("<Input files>"),
                                           llvm::cl::OneOrMore);

llvm::cl::opt<unsigned> width("w", llvm::cl::desc("line-width (default 40)"),
                              llvm::cl::init(40));
llvm::cl::opt<unsigned> grouping("g", llvm::cl::desc("grouping (default 5)"),
                                 llvm::cl::init(5));
llvm::cl::opt<bool> dump("d", llvm::cl::desc("binary dump"),
                         llvm::cl::init(false));
llvm::cl::opt<bool> raw("r", llvm::cl::desc("raw binary dump"),
                        llvm::cl::init(false));
llvm::cl::opt<bool> binary("b", llvm::cl::desc("binary records"),
                           llvm::cl::init(false));
llvm::cl::opt<bool> symbolic("s", llvm::cl::desc("symbolic records"),
                             llvm::cl::init(false));

enum Encoding { IBM704, IBM704_4, CP29 };

llvm::cl::opt<Encoding> encoding(
    "encoding", llvm::cl::desc("Character encoding (default 704-4)"),
    values(clEnumValN(Encoding::IBM704, "704", "FORTRAN encoding for IBM 704"),
           clEnumValN(Encoding::IBM704_4, "704-4",
                      "FORTRAN encoding for IBM 704 (4)"),
           clEnumValN(Encoding::CP29, "029", "IBM 029 Card Punch")),
    llvm::cl::init(Encoding::IBM704_4));

} // namespace

class BCDHandler {
public:
  // Use c as the character for in-memory bcd
  void setChar(bcd_t bcd, char32_t c) {
    static constexpr bcd_t relocatedZero{0b001010};
    auto tapeBCD = evenParity(tapeBCDfromBCD(bcd));
    if (relocatedZero == bcd) {
      // Relocated zero can't be used
      return;
    }
    table_[char(tapeBCD)] = c;
  }

  char decode(char tapeBCD) {
    char c;
    auto charit = table_.find(tapeBCD);
    if (charit != table_.end()) {
      c = charit->second;
    } else if (isEvenParity(bcd_t(tapeBCD))) {
      // Unmapped character
      c = 'a' + unmapped_.size();
      unmapped_[tapeBCD] = c;
      table_[tapeBCD] = c;
      std::cout << "*** Unmapped " << std::hex << std::setw(2)
                << std::setfill('0') << int(tapeBCD) << " '" << c << "'\n";
    } else {
      // Bad parity, search for neighbors
      std::vector<char> candidates;
      unsigned char bit = 1;
      do {
        char bitc = tapeBCD ^ bit;
        auto cit = table_.find(bitc);
        if (cit != table_.end()) {
          candidates.push_back(cit->second);
        }
        bit <<= 1;
      } while (bit != 1 << 7);
      std::cout << "*** Parity " << std::hex << std::setw(2)
                << std::setfill('0') << int(tapeBCD) << " {";
      for (auto nc : candidates) {
        std::cout << nc;
      }
      std::cout << "}\n";
      c = 'x';
    }
    return c;
  }

protected:
  std::unordered_map<char, char32_t> table_;
  std::unordered_map<char, char> unmapped_;
};

class DumpTapeIByteStream : public IByteStream {
public:
  DumpTapeIByteStream(std::istream &input) : input_(input) {}
  size_t read(char *buffer, size_t size) override {
    size_t pos = input_.tellg();
    input_.read(buffer, size);
    size_t numRead = input_.gcount();
    if (raw) {
      for (size_t i = 0; i < numRead; ++i) {
        if (0 == (pos + i) % width_) {
          if (pos + i > 0) {
            std::cout << '\n';
          }
          std::cout << std::setw(7) << std::setfill(' ') << std::dec
                    << (pos + i) << ":";
        }
        if (0 == (pos + i) % grouping_) {
          std::cout << ' ';
        }
        std::cout << std::uppercase << std::hex << std::setw(2)
                  << std::setfill('0') << (0xFF & unsigned(buffer[i]));
      }
      if (0 == numRead && (pos % width_) != 0) {
        std::cout << "\n";
      }
    }
    return numRead;
  }

protected:
  std::istream &input_;
  size_t width_{width};
  size_t grouping_{grouping};
};

class DumpTapeAdapter : public TapeReadAdapter {
public:
  DumpTapeAdapter(TapeIRecordStream &tapeIStream, BCDHandler &bcdHandler)
      : TapeReadAdapter(tapeIStream), bcdHandler_(bcdHandler) {}

  void onRecordData(char *buffer, size_t size) override {
    evenParityCount_ += std::count_if(buffer, buffer + size, [](char c) {
      return isEvenParity(sevenbit_t(c));
    });
    if (dump) {

      for (size_t i = 0; i < size; ++i) {
        if (0 == i % width_) {
          if (i > 0) {
            std::cout << '\n';
          }
          std::cout << std::dec << std::setw(7) << std::setfill(' ')
                    << tapePos_ + i << ": ";
        } else if (i > 0 && grouping_ > 0 && (0 == (i % grouping_))) {
          std::cout << ' ';
        }

        std::cout << std::uppercase << std::hex << std::setw(2)
                  << std::setfill('0') << (0xFF & unsigned(buffer[i]));
      }
      std::cout << '\n';
    }
    std::copy(&buffer[0], &buffer[size], std::back_inserter(record_));
  }

  void onBinaryRecordData() override {
    if (binary) {
      while ((record_.size() % 6) != 0) {
        record_.push_back(0);
      }
      for (size_t i = 0; i < record_.size();) {
        word_t word{0};
        dpb<30, 6>(record_[i++], word);
        dpb<24, 6>(record_[i++], word);
        dpb<18, 6>(record_[i++], word);
        dpb<12, 6>(record_[i++], word);
        dpb<6, 6>(record_[i++], word);
        dpb<0, 6>(record_[i++], word);
        std::cout << std::oct << std::setw(12) << std::setfill('0') << word
                  << '\n';
      }
    }
  }

  void onBCDRecordData() override {
    if (symbolic) {
      // BCD data
      auto it = record_.begin();
      while (it < record_.end()) {
        auto thisWhack =
            std::min<size_t>(width_ - lineSize_, record_.end() - it);
        auto thisEnd = it + thisWhack;
        std::ostringstream buffer;
        while (it < thisEnd) {
          char c = bcdHandler_.decode(*it++);
          buffer << c;
        }
        std::cout << buffer.str();
        lineSize_ += thisWhack;
        if (lineSize_ == width_) {
          std::cout << '\n';
          lineSize_ = 0;
        }
      }
    }
  }

  void onBeginOfRecord() override {
    evenParityCount_ = 0;
    lineSize_ = 0;
    if (dump) {
      std::cout << "BOR\n";
    }
  }

  void onEndOfRecord() override {
    if (2 * evenParityCount_ < record_.size()) {
      onBinaryRecordData();
    } else {
      onBCDRecordData();
    }
    record_.clear();
    if (dump) {
      size_t recordLength = tapePos_ - recordStartPos_;
      std::cout << "EOR " << std::dec << "even/odd parity: " << evenParityCount_
                << '/' << recordLength - evenParityCount_
                << " record length: " << recordLength << "\n";
    }
  }

  void onEndOfFile() override {
    if (dump) {
      std::cout << "EOF " << std::dec << tapePos_ << "\n";
      stopReading();
    }
  }

  void onEndOfTape() override {
    if (dump) {
      std::cout << "EOT " << std::dec << tapePos_ << "\n";
    }
  }

protected:
  size_t evenParityCount_{0};
  std::vector<char> record_;
  BCDHandler bcdHandler_;
  size_t width_{width};
  size_t grouping_{grouping};
  size_t lineSize_{0};
};

void dumpTape(BCDHandler &bcdHandler, std::istream &input) {
  DumpTapeIByteStream rawStream(input);
  P7BIStream reader(rawStream);
  DumpTapeAdapter dumper(reader, bcdHandler);
  dumper.read();
}

// tapePos is byte position of record start on tape
// record.empty() => EOF
using tape_event_handler_t = std::function<void(size_t tapePos, bool isBinary,
                                                std::vector<char> &record)>;

void readTape(std::istream &input, tape_event_handler_t handler) {
  // Every byte corresponds to one character on tape
  // Bit 7 indicates the start of a new record.  One character 0x8F record
  // indicates EOF. Bit 6 is parity. Binary records have odd parity, BCD records
  // have even parity.
  // EOF is a 1 byte record containing 0xF (plus parity).

  // Start byte of current chunk on tape
  size_t tapeChunkBegin{0};
  // Start byte of current record on tape
  size_t tapeRecordBegin{0};

  std::vector<char> record;
  std::array<char, 1024> buffer;
  auto chunkBufferBegin = buffer.begin();
  auto bufferEnd = buffer.begin();
  char BORByte{'\0'};
  while (true) {
    if (chunkBufferBegin == bufferEnd) {
      // Refill
      chunkBufferBegin = buffer.begin();
      input.read(buffer.data(), buffer.size());
      bufferEnd = chunkBufferBegin + input.gcount();
      if (chunkBufferBegin == bufferEnd) {
        return;
      }
    }
    // Scan for the next begin of record mark
    auto BORBufferPos = std::find_if(chunkBufferBegin, bufferEnd, [](char c) {
      return std::byte(0x80) == (std::byte(c) & std::byte(0x80));
    });
    auto chunkSize = (BORBufferPos - chunkBufferBegin);
    if (chunkSize > 0) {
      auto recordSize = tapeChunkBegin + chunkSize - tapeRecordBegin;
      // Add data to the current record
      std::copy(chunkBufferBegin, BORBufferPos, std::back_inserter(record));
      tapeChunkBegin += chunkSize;
      chunkBufferBegin += chunkSize;
    }
    if (chunkBufferBegin == buffer.end()) {
      // Didn't find BOR in buffer, get more bytes
      continue;
    }
    if (tapeRecordBegin < tapeChunkBegin) {
      if (record.size() == 1 && ((record[0] & 0x0F) == 0x0F)) {
        record.clear();
      }
      auto evenCount = std::count_if(record.begin(), record.end(), [](char c) {
        return isEvenParity(sixbit_t(c));
      });
      handler(tapeRecordBegin, evenCount * 2 < record.size(), record);
      record.clear();
    }
    *chunkBufferBegin &= 0x7F;
    tapeRecordBegin = tapeChunkBegin;
  }
}

class ReadHandler {
public:
  ReadHandler(std::istream &input, BCDHandler &bcdHandler, size_t width)
      : input_(input), bcdHandler_(bcdHandler), width_(width) {}

  void operator()(size_t tapePos, bool isBinary, std::vector<char> &record) {
    if (record.empty()) {
      std::cout << "--- EOF ---\n";
      return;
    }
    auto recordSize = record.size();
    if (isBinary) {

      // Binary data
      while ((record.size() % 6) != 0) {
        record.push_back(0);
      }
      for (size_t i = 0; i < recordSize;) {
        word_t word{0};
        dpb<30, 6>(record[i++], word);
        dpb<24, 6>(record[i++], word);
        dpb<18, 6>(record[i++], word);
        dpb<12, 6>(record[i++], word);
        dpb<6, 6>(record[i++], word);
        dpb<0, 6>(record[i++], word);
        disassemble(std::cout, addr_t(i / 6 - 1), word);
        std::cout << "\n";
      }
    } else {
      // BCD data
      auto it = record.begin();
      while (it < record.end()) {
        auto thisWhack =
            std::min<size_t>(width_ - lineSize_, record.end() - it);
        auto thisEnd = it + thisWhack;
        std::ostringstream buffer;
        while (it < thisEnd) {
          char c = bcdHandler_.decode(*it++);
          buffer << c;
        }
        std::cout << buffer.str();
        lineSize_ += thisWhack;
        if (lineSize_ == width_) {
          std::cout << '\n';
          lineSize_ = 0;
        }
      }
    }
    if (lineSize_ > 0) {
      std::cout << '\n';
      lineSize_ = 0;
    }
    std::cout << "--- EOR " << std::dec << recordSize << " ---\n";
  }

private:
  std::istream &input_;
  size_t lineSize_{0};
  size_t width_{0};
  BCDHandler bcdHandler_;
};

int main(int argc, const char **argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "Version " << Z0ftware_VERSION_MAJOR << "." << Z0ftware_VERSION_MINOR
       << "." << Z0ftware_VERSION_PATCH << "\n";
  });
  llvm::cl::ParseCommandLineOptions(
      argc, argv,
      "BCD tape dumper for IBM 704\n\n"
      "  This program dumps a BCD tape as ASCII\n");

  BCDHandler bcdHandler;
  auto charEncoding = getBCDIC1();
  if (encoding == CP29) {
    charEncoding = get029Encoding();
  } else if (encoding == IBM704) {
    charEncoding = getFORTRAN704Encoding();
  }
  for (auto colUni : charEncoding) {
    bcdHandler.setChar(BCDFromColumn(colUni.hollerith), colUni.unicode);
  }
  // '+'
  bcdHandler.setChar(BCDFromColumn(hollerithFromRows({12})), '+');

  bcdHandler.setChar(BCDFromColumn(hollerithFromRows({3, 8})), '=');
  bcdHandler.setChar(BCDFromColumn(hollerithFromRows({0, 4, 8})), '(');
  bcdHandler.setChar(BCDFromColumn(hollerithFromRows({12, 4, 8})), ')');

  // '0' overrides
  bcdHandler.setChar(BCDFromColumn(hollerithFromRows({0})), '0');
  for (auto &inputFileName : inputFileNames) {
    std::ifstream input(inputFileName,
                        std::ifstream::binary | std::ifstream::in);

    if (dump || raw || binary || symbolic) {
      dumpTape(bcdHandler, input);
      break;
    }
    ReadHandler handler(input, bcdHandler, width);
    readTape(input, handler);
    input.close();
  }
  return EXIT_SUCCESS;
}