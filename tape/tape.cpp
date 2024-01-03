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

llvm::cl::opt<unsigned> cardWidth("w", llvm::cl::desc("card-width"),
                                  llvm::cl::init(84));
} // namespace

enum class ReadKind { Data, EndOfRecord, EndOfFile, EndOfMedia, EndOfInput };

using tape_event_handler_t =
    std::function<void(ReadKind, char *begin, char *end)>;

void readTape(std::istream &input, tape_event_handler_t handler) {
  using buffer_t = std::array<char, 1024>;
  buffer_t buffer;
  auto bufferPos = buffer.begin();
  auto bufferEnd = buffer.begin();
  bool hasRecord{false};
  while (true) {
    if (bufferPos == bufferEnd) {
      input.read(buffer.data(), buffer.size());
      bufferPos = buffer.begin();
      bufferEnd = bufferPos + input.gcount();
      if (bufferPos == bufferEnd) {
        handler(ReadKind::EndOfInput, nullptr, nullptr);
        return;
      }
    }
    auto recordChunkBegin = bufferPos;
    while (bufferPos < bufferEnd) {
      if ((std::byte(*bufferPos) & std::byte(0x80)) == std::byte(0x80)) {
        // Beginning of next record
        break;
      }
      bufferPos++;
    }
    if (recordChunkBegin == bufferPos) {
      // No more data in this record chunk
      auto c = std::byte(*bufferPos);
      if (hasRecord) {
        handler(ReadKind::EndOfRecord, nullptr, nullptr);
      }
      if (c == std::byte(0x8F)) {
        // Take byte
        bufferPos++;
        if (hasRecord) {
          handler(ReadKind::EndOfFile, nullptr, nullptr);
        } else {
          handler(ReadKind::EndOfMedia, nullptr, nullptr);
        }
        hasRecord = false;
        continue;
      }
      hasRecord = true;
      *bufferPos = char(std::byte(*bufferPos) & std::byte(0x7F));
      continue;
    }
    handler(ReadKind::Data, recordChunkBegin, bufferPos);
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

  void operator()(ReadKind readKind, char *begin, char *end) {
    switch (readKind) {
    case ReadKind::Data: {
      // Capture the entire record before processing so we can get parity counts
      std::copy(begin, end, std::back_inserter(record_));
      break;
    }
    case ReadKind::EndOfRecord: {
      auto recordSize = record_.size();
      size_t evenCount = 0;
      for (auto it = record_.begin(); it != record_.end(); ++it) {
        if (isEvenParity(sixbit_t(*it))) {
          evenCount++;
        }
      }
      if (evenCount * 2 < recordSize) {
        // Binary data
        while ((record_.size() % 6) != 0) {
          record_.push_back(0);
        }
        for (size_t i = 0; i < recordSize;) {
          word_t word{0};
          dpb<30, 6>(record_[i++], word);
          dpb<24, 6>(record_[i++], word);
          dpb<18, 6>(record_[i++], word);
          dpb<12, 6>(record_[i++], word);
          dpb<6, 6>(record_[i++], word);
          dpb<0, 6>(record_[i++], word);
          disassemble(std::cout, addr_t(i / 6 - 1), word);
          std::cout << "\n";
        }
      } else {
        // BCD data
        auto it = record_.begin();
        while (it < record_.end()) {
          auto thisWhack =
              std::min<size_t>(cardWidth_ - lineSize_, record_.end() - it);
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
      record_.clear();
      if (lineSize_ > 0) {
        std::cout << '\n';
        lineSize_ = 0;
      }
      std::cout << "--- EOR ---\n";
      break;
    }
    case ReadKind::EndOfFile: {
      std::cout << "--- EOF ---\n";
      break;
    }
    case ReadKind::EndOfMedia: {
      std::cout << "--- EOT ---\n";
      break;
    }
    case ReadKind::EndOfInput: {
      std::cout << "--- EndOfInput ---\n";
      break;
    }
    }
  }

private:
  std::istream &input_;
  std::vector<char> record_;
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
    for (auto colUni : get029Encoding()) {
      handler.setChar(BCDFromColumn(colUni.column), colUni.unicode);
    }
    // '0' overrides
    handler.setChar(BCDFromColumn(columnFromRows({0})), '0');
    readTape(input, handler);
    input.close();
  }
  return EXIT_SUCCESS;
}
