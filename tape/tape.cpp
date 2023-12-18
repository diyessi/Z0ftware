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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/parity.hpp"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

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
      "BCD tape dumper for IBM 704\n\n"
      "  This program dumps a BCD tape as ASCII\n");

  for (auto &inputFileName : inputFileNames) {
    std::ifstream input(inputFileName,
                        std::ifstream::binary | std::ifstream::in);
    while (true) {
      std::array<char, 84> buffer;
      input.read(buffer.data(), buffer.size());
      auto count = input.gcount();
      if (count == 0) {
        break;
      }
      for (size_t i = 0; i < count; ++i) {
        std::byte c = std::byte(buffer[i]);
        if ((c & std::byte{0x80}) == std::byte{0x80}) {
          // Not sure what this means. Can occur mid-card
          // std::cout << "\n*** BO ***\n";
        }
        c = c & std::byte{0x7F};
        auto c6 = evenParity(sixbit_t(c));
        if (sevenbit_t(c) != c6) {
          std::cout << "Parity error\n" << std::endl;
        }
        c = c & std::byte(0x3F);
        if (c == std::byte{0x0A}) {
          c = std::byte{0x00};
        } else if (c == std::byte(0x0F)){
          // Seems to be an early end card
          std::cout << "\n";
          continue;
        } else if (c == std::byte(0x3A)) {
          // Record mark
          std::cout << "\n*** EOR ***\n";
          continue;
        } else if ((c & std::byte{0x10}) == std::byte{0x10}) {
          //
          c ^= std::byte{0x20};
        }
        auto d = charFromBCD(bcd_t(c));
        if (d == '\0') {
          std::ostringstream temp;
          temp << '[' << std::setw(2) << std::setfill('0') << std::hex << int(c)
               << ']';
          std::cout << temp.str() << std::endl;
        } else {
          std::cout << d;
        }
      }
      std::cout << "\n";
    }
  }
  return EXIT_SUCCESS;
}
