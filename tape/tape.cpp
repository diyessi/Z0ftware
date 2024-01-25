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
#include "Z0ftware/word.hpp"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

namespace {

llvm::cl::list<std::string> inputFileNames(llvm::cl::Positional,
                                           llvm::cl::desc("<Input files>"),
                                           llvm::cl::OneOrMore);

llvm::cl::opt<unsigned> cardWidth("w",
                                  llvm::cl::desc("card-width (default 84)"),
                                  llvm::cl::init(84));

enum Encoding { IBM704, CP29 };

llvm::cl::opt<Encoding> encoding(
    "encoding", llvm::cl::desc("Character encoding (default 704)"),
    values(clEnumValN(Encoding::IBM704, "704", "FORTRAN encoding for IBM 704"),
           clEnumValN(Encoding::CP29, "029", "IBM 029 Card Punch")),
    llvm::cl::init(Encoding::IBM704));

} // namespace

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
  ReadHandler(std::istream &input, size_t cardWidth)
      : input_(input), cardWidth_(cardWidth) {}

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
      for (size_t i = 0; i < recordSize; ++i) {
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
            std::min<size_t>(cardWidth_ - lineSize_, record.end() - it);
        auto thisEnd = it + thisWhack;
        std::ostringstream buffer;
        while (it < thisEnd) {
          char c;
          auto tapeBCD = *it++;
          auto charit = table_.find(tapeBCD);
          if (charit != table_.end()) {
            c = charit->second;
          } else if (isEvenParity(bcd_t(tapeBCD))) {
            // Unmapped character
            c = 'a' + unmapped_.size();
            unmapped_[tapeBCD] = c;
            table_[tapeBCD] = c;
            std::cout << "*** Unmapped " << std::hex << std::setw(2)
                      << std::setfill('0') << int(tapeBCD) << " '" << c
                      << "'\n";
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
          buffer << c;
        }
        std::cout << buffer.str();
        lineSize_ += thisWhack;
        if (lineSize_ == cardWidth_) {
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
  size_t cardWidth_{0};
  std::unordered_map<char, char32_t> table_;
  std::unordered_map<char, char> unmapped_;
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

  for (auto &inputFileName : inputFileNames) {
    std::ifstream input(inputFileName,
                        std::ifstream::binary | std::ifstream::in);
    ReadHandler handler(input, cardWidth);
    auto charEncoding = getFORTRAN704Encoding();
    if (encoding == CP29) {
      charEncoding = get029Encoding();
    }
    for (auto colUni : charEncoding) {
      handler.setChar(BCDFromColumn(colUni.column), colUni.unicode);
    }
    // '0' overrides
    handler.setChar(BCDFromColumn(columnFromRows({0})), '0');
    readTape(input, handler);
    input.close();
  }
  return EXIT_SUCCESS;
}
