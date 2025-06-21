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

#include "Z0ftware/asm.hpp"
#include "Z0ftware/card.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/disasm.hpp"
#include "Z0ftware/operation.hpp"
#include "Z0ftware/parser.hpp"
#include "Z0ftware/signature.hpp"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {
llvm::cl::opt<std::string>
    outputFileName("o", llvm::cl::desc("Specify output filename"),
                   llvm::cl::value_desc("filename"));

llvm::cl::list<std::string> inputFileNames(llvm::cl::Positional,
                                           llvm::cl::desc("<Input files>"),
                                           llvm::cl::OneOrMore);
} // namespace

int main(int argc, const char **argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "Version " << Z0ftware_VERSION_MAJOR << "." << Z0ftware_VERSION_MINOR
       << "." << Z0ftware_VERSION_PATCH << "\n";
  });
  llvm::cl::ParseCommandLineOptions(
      argc, argv,
      "SAP23 assembler for IBM 704\n\n"
      "  This program assembles a subset of what UASAP can assemble, but\n"
      "  does so considerably faster.\n");

  SAPAssembler sapAssembler;
  std::ofstream os(outputFileName, std::ofstream::binary | std::ofstream::out |
                                       std::ofstream::trunc);
  if (!outputFileName.empty()) {
    section_writer_t sectionWriter = [&os](const Section &section) {
      CardImage cardImage;
      int pos = 0;
      addr_t cardBeginAddr = 0;
      addr_t cardEndAddr = cardBeginAddr;
      uint64_t checksum = 0;
      auto finishCard = [&section, &cardBeginAddr, &cardEndAddr, &cardImage,
                         &os, &pos, &checksum]() {
        switch (section.getBinaryFormat()) {
        case BinaryFormat::Absolute: {
          word_t L = 0;
          dpb<33, 3>(0, L);
          dpb<18, 15>(cardEndAddr - cardBeginAddr, L);
          dpb<15, 3>(0, L);
          dpb<0, 15>(cardBeginAddr, L);
          checksum += L;
          cardImage.setWord(0, L);
          cardImage.setWord(1, checksum % ((uint64_t(1) << 36) - 1));
          break;
        }
        case BinaryFormat::Relative: {
          std::cerr << "Relative not supported\n";
          cardImage.setWord(0, 0);
          cardImage.setWord(1, 0);
          break;
        }
        case BinaryFormat::Full: {
          break;
        }
        }
        writeCBN(os, cardImage);
        cardImage.clear();
        pos = 0;
      };
      for (auto &chunk : section.getChunks()) {
        for (auto it = chunk.begin(); it < chunk.end(); ++it) {
          if (pos == 0) {
            switch (section.getBinaryFormat()) {
            case BinaryFormat::Absolute:
            case BinaryFormat::Relative: {
              cardBeginAddr = chunk.getBaseAddr() + (it - chunk.begin());
              cardEndAddr = cardBeginAddr;
              pos = 2;
              checksum = 0;
              break;
            }
            case BinaryFormat::Full: {
              break;
            }
            }
          }
          cardImage.setWord(pos++, *it);
          checksum += *it;
          cardEndAddr++;
          if (24 == pos) {
            finishCard();
          }
        }
        if (chunk.isTransfer()) {
          if (pos > 0) {
            finishCard();
          }
          cardImage.clear();
          switch (section.getBinaryFormat()) {
          case BinaryFormat::Absolute: {
            word_t L = 0;
            dpb<0, 15>(chunk.getTransfer(), L);
            cardImage.setWord(0, L);
            cardImage.setWord(1, 0);
            break;
          }
          case BinaryFormat::Full:
            break;
          case BinaryFormat::Relative: {
            std::cerr << "Relative not supported\n";
            cardImage.setWord(0, 0);
            cardImage.setWord(1, 0);
            break;
          }
          }
          writeCBN(os, cardImage);
          cardImage.clear();
          pos = 0;
        }
      }
      if (pos > 0) {
        finishCard();
      }
    };
    sapAssembler.setSectionWriter(sectionWriter);
  } else {
    section_writer_t sectionWriter = [](const Section &section) {
      std::cout << "===================\n";
      auto address = section.getBase();
      for (auto &chunk : section.getChunks()) {
        for (auto it = chunk.begin(); it < chunk.end(); ++it) {
          writeWord(std::cout, address++, *it);
          std::cout << "\n";
        }
      }
      std::cout << "===================\n";
    };
    sapAssembler.setSectionWriter(sectionWriter);
  }
  std::vector<SAPDeck> decks;
  for (auto &inputFileName : inputFileNames) {
    std::ifstream is(inputFileName);
    auto &sapDeck = decks.emplace_back(SAPDeck(is));
    for (auto &card : sapDeck.getCards()) {
      auto operation = sapAssembler.parseLine(card);
      sapAssembler.appendOperation(std::move(operation));
    }
  }
  sapAssembler.assemble();
  os.close();

  return EXIT_SUCCESS;
}