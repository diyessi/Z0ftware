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

#include "Z0ftware/disasm.hpp"
#include "Z0ftware/field.hpp"
#include "Z0ftware/op.hpp"

#include <iomanip>
#include <iostream>

void disassemble(std::ostream &output, addr_t location, word_t word) {
  uint16_t prefix = OpSpec::Prefix::ref(word);
  uint16_t opCode = OpSpec::OpCode::ref(word);
  uint16_t tag = OpSpec::Tag::ref(word);
  addr_t address = OpSpec::Address::ref(word);
  addr_t decrement{0};
  if ((prefix & 03) != 0) {
    // Type A
    opCode = opCode & 07000;
    decrement = OpSpec::Decrement::ref(word);
  } else {
    // Type B
  }
  word_t baseWord{0};
  OpSpec::OpCode::ref(baseWord) = opCode;
  OpSpec::Address::ref(baseWord) = address;
  const auto *opSpec = OpSpec::getOpSpec(baseWord);
  writeInstruction(output, location, word);
  output << " "
         << "      "
         << " ";
  if (opCode == opSpec->getOpCode()) {
    output << std::dec << opSpec->getOperation() << " " << (address - opSpec->getAddress());
    output << "," << tag;
    if (decrement != 0) {
      output << "," << decrement;
    }
  } else {
    output << "OCT"
           << " " << std::oct << word;
  }
}

// TODO: Define these fields somewhere
std::ostream &writeInstruction(std::ostream &os, addr_t location, word_t word) {
  os << std::setw(5) << std::setfill('0') << std::oct << location << ' ';
  os << (BitField<35, 1, uint16_t>::ref(word) == 0 ? ' ' : '-');
  os << std::setw(1) << std::setfill('0') << std::oct
     << BitField<33, 2, uint16_t>::ref(word) << ' ';
  os << std::setw(5) << std::setfill('0') << std::oct
     << BitField<35 - 17, 15>::ref(word) << ' ';
  os << std::setw(1) << std::setfill('0') << std::oct
     << BitField<35 - 20, 3, uint16_t>::ref(word) << ' ';
  os << std::setw(5) << std::setfill('0') << std::oct
     << BitField<0, 15>::ref(word);
  return os;
}

std::ostream &writeAddress(std::ostream &os, addr_t address) {
  return os << "                 " << std::setw(5) << std::setfill('0')
            << std::oct << address;
}

std::ostream &writeWord(std::ostream &os, addr_t location, word_t word) {
  os << std::setw(5) << std::setfill('0') << std::oct << location << "    ";
  os << std::setw(12) << std::setfill('0') << std::oct << word << "   ";
  return os;
}
