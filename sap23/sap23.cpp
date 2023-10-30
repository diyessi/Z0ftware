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
  std::vector<SAPDeck> decks;
  for (auto &inputFileName : inputFileNames) {
    std::ifstream is(inputFileName);
    auto &sapDeck = decks.emplace_back(SAPDeck(is));
    for (auto &card : sapDeck.getCards()) {
      auto operation = sapAssembler.parseLine(card);
      sapAssembler.addInstruction(std::move(operation));
    }
  }
  for (auto &instruction : sapAssembler.getInstructions()) {
    instruction->assemble(sapAssembler);
    instruction->print(std::cout, sapAssembler);
    std::cout << instruction->getLine() << "\n";
  }

  return EXIT_SUCCESS;
}