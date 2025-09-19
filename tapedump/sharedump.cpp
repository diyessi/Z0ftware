// MIT License
//
// Copyright (c) 202-2025 Scott Cyphers
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
#include "Z0ftware/charset.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/tape.hpp"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {
llvm::cl::list<std::string> inputFileNames(llvm::cl::Positional,
                                           llvm::cl::desc("<Input files>"),
                                           llvm::cl::OneOrMore);

llvm::cl::opt<bool> listFiles("list", llvm::cl::desc("list files on tape"),
                              llvm::cl::init(false));

llvm::cl::opt<bool> showHeaders("show-headers",
                                llvm::cl::desc("Show record headers"),
                                llvm::cl::init(false));

} // namespace

class ShareExtractor : public TapeReadAdapter {
public:
  ShareExtractor(TapeIRecordStream &tapeIStream, const CharsetForTape &charSet)
      : TapeReadAdapter(tapeIStream), tapeChars_(charSet.getTapeCharset(true)) {
    for (bcd_t i = bcd_t::min(); i <= bcd_t::max(); ++i) {
      std::cout << tapeChars_->at(evenParity(i.value()).value());
    }
    std::cout << std::endl;
  }

  size_t getCurrentDeck() { return nextDeck_ - 1; }

  void onRead(size_t pos, char *buffer, size_t size) override {}
  void onRecordData(char *buffer, size_t size) override {}
  void onBinaryRecordData() override { std::cout << "Binary\n"; }
  void onBCDRecordData() override {
    if (showHeaders_) {
      std::cout << "Record: " << getRecordNum() << " BCD: " << record_.size()
                << "\n";
    }
    size_t line_size;
    if (0 == (record_.size() % 80)) {
      line_size = 80;
    } else if (0 == (record_.size() % 84)) {
      line_size = 84;
    } else if (0 == (record_.size() % 72)) {
      line_size = 72;
    } else {
      std::cout << "Record size: " << record_.size()
                << " using line size of 80\n";
      line_size = 80;
    }

    size_t pos = 0;
    std::ostringstream ostream;
    for (auto it : record_) {
      ostream << tapeChars_->at(it);
      if (++pos == line_size) {
        pos = 0;
        auto view = ostream.view();
        if (view.end() != std::find_if(view.begin(), view.end(),
                                       [](char c) { return c != ' '; })) {
          if (record_.size() <= 84) {
            // Identification for next library file
            nextDeck_++;
            auto classification = view.substr(0, view.find(' ', 0));
            auto installation = view.substr(3, view.find(' ', 3) - 3);
            auto name = view.substr(6, view.find(' ', 6) - 6);
            auto id = view.substr(20, view.find(' ', 20) - 20);
            auto format = view.substr(33, 2);
            std::cout << "===========\n";
            std::cout << view << "\n";
            std::ostringstream deckName;
            deckName << std::setw(4) << std::setfill('0') << getCurrentDeck();
            if (!classification.empty()) {
              deckName << "-" << classification;
            }
            if (!installation.empty()) {
              deckName << "-" << installation;
            }
            deckName << "-" << name;
            if (!id.empty()) {
              deckName << "-" << id;
            }
            deckName << "." << format;

            std::cout << "Current deck: " << getCurrentDeck() << " '"
                      << deckName.str() << "'\n";
            std::cout << "Classification: '" << classification << "' Company: '"
                      << installation << "' Name: '" << name << "' Id: '" << id
                      << "' Format: '" << format << "'"
                      << "\n";
            std::cout << "===========\n";
          } else {
            std::cout << view << "\n";
          }
        }
        ostream.str("");
      }
    }
    if (showHeaders_) {
      std::cout << std::endl;
    }
  }
  void onBeginOfRecord() override {}
  void onEndOfRecord() override {}
  void onEndOfFile() override {}
  void onEndOfTape() override {}

protected:
  std::unique_ptr<even_glyphs_t> tapeChars_;
  bool showHeaders_{showHeaders};
  size_t nextDeck_{0};
};

int main(int argc, const char **argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "Version " << Z0ftware_VERSION_MAJOR << "." << Z0ftware_VERSION_MINOR
       << "." << Z0ftware_VERSION_PATCH << "\n";
  });

  llvm::cl::ParseCommandLineOptions(
      argc, argv,
      "SHARE tape extractor for IBM 704\n\n"
      "  This program extracts information from SHARE tapes.\n");

  std::setlocale(LC_ALL, "");

  for (auto &inputFileName : inputFileNames) {
    std::ifstream input(inputFileName,
                        std::ifstream::binary | std::ifstream::in);
    if (!input.is_open()) {
      std::cerr << "Count not open " << inputFileName << "\n";
      continue;
    }
    P7BIStream reader(input);
    ShareExtractor extractor(reader, collateGlyphCardTape);
    extractor.read();
    input.close();
  }

  return EXIT_SUCCESS;
}