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

#include "Z0ftware/operation.hpp"
#include "Z0ftware/asm.hpp"
#include "Z0ftware/bcd.hpp"
#include "Z0ftware/disasm.hpp"
#include "Z0ftware/op.hpp"
#include "Z0ftware/parser.hpp"
#include "Z0ftware/utils.hpp"

std::pair<std::string_view, std::string_view>
Operation::splitVariableAndComment(Assembler &assembler,
                                   const std::string_view &variableAndComment) {
  return assembler.splitVariableAndComment(variableAndComment);
}

void Operation::parseVariable(Assembler &assembler,
                              const std::string_view &variable) {
  exprs_ = assembler.parseEXP(*this, variable);
}

void Abs::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.setBinaryFormat(BinaryFormat::Absolute);
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

std::pair<std::string_view, std::string_view>
Bcd::splitVariableAndComment(Assembler &assembler,
                             const std::string_view &variableAndComment) {
  return {variableAndComment, ""};
}

Section &Operation::getSection(Assembler &assembler) const {
  return assembler.getSection();
}

void Bcd::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  size_t count = 10;
  std::string_view::size_type pos = 0;
  std::string_view::size_type limit = variable.size();
  auto take = [this, &pos, &limit, &variable]() {
    if (pos < limit) {
      return variable[pos++];
    }
    pos++;
    return ' ';
  };

  auto countChar = take();
  if (countChar != ' ') {
    if ('0' < countChar && countChar <= '9') {
      count = countChar - '0';
    } else {
      addWarning() << "Invalid BCD count, using 10";
    }
  }
  char buffer[6];
  for (int i = 0; i < count; ++i) {
    for (int j = 0; j < 6; j++) {
      buffer[j] = take();
    }
    values_.push_back(bcd(std::string_view(&buffer[0], &buffer[6])));
  }
}

void Bcd::allocate(Assembler &assembler, Chunk &chunk) const {
  auto size = values_.size();
  assembler.allocate(chunk, size, Assembler::AssignType::Begin);
}

void Bcd::assemble(Assembler &assembler, Chunk &chunk) const {
  std::copy(values_.begin(), values_.end(), chunk.begin());
}

void Bes::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for " << operationSymbol_;
  }
  requireLocation(assembler);
}

void Bes::allocate(Assembler &assembler, Chunk &chunk) const {
  auto size = assembler.evaluate(chunk, *getExprs()[0]);
  assembler.allocate(chunk, size, Assembler::AssignType::End);
}
void Bes::assemble(Assembler &assembler, Chunk &chunk) const {
  std::fill(chunk.begin(), chunk.end(), 0);
}

void Bss::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Bss::allocate(Assembler &assembler, Chunk &chunk) const {
  auto size = assembler.evaluate(chunk, *exprs_[0]);
  assembler.allocate(chunk, size, Assembler::AssignType::Begin);
}

void Bss::assemble(Assembler &assembler, Chunk &chunk) const {
  std::fill(chunk.begin(), chunk.end(), 0);
}

void Dec::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  static PegDECParser parser;
  bool success = parser.parse(variable, values_);
  if (!success) {
    addError() << "Could not parse DEC";
  }
}

void Dec::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, values_.size(), Assembler::AssignType::Begin);
}

void Dec::assemble(Assembler &assembler, Chunk &chunk) const {
  std::copy(values_.begin(), values_.end(), chunk.begin());
}

void Def::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.setDefineLocation(chunk);
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

void End::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

Section &End::getSection(Assembler &assembler) const {
  return assembler.addSection(assembler.evaluate(*exprs_[0]), BinaryFormat::AbsoluteTransfer);
}

void End::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 1, Assembler::AssignType::None);
}

void Equ::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Equ::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
  assembler.defineSymbol(getLocationSymbol(),
                         assembler.evaluate(chunk, *exprs_[0]));
}

std::ostream &Equ::print(std::ostream &os, Assembler &assembler,
                         const Chunk &chunk) const {
  return writeAddress(os, assembler.evaluate(chunk, *exprs_[0]));
}

void Ful::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.setBinaryFormat(BinaryFormat::Full);
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

void Hed::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  hed_ = variable;
}

void Hed::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

void Lib::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  library_ = variable;
}

void Lib::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

void Oct::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  auto it = variable.begin();
  std::optional<bool> negative;
  word_t magnitude{0};
  bool partial{false};
  while (it != variable.end()) {
    auto c = *it++;
    partial = true;
    if (c == '+' || c == '-') {
      if (negative) {
        auto col = std::distance(it, variable.begin()) - 1;
        addError() << "Multiple signs";
        break;
      }
      negative = (c == '-');
    } else if ('0' <= c && c <= '9') {
      magnitude = magnitude * 8 + c - '0';
    } else {
      values_.push_back({negative ? negative.value() : false, magnitude});
      if (c == ' ') {
        break;
      } else if (c != ',') {
        addError() << "Invalid character";
        break;
      } else {
        negative = std::optional<bool>();
        magnitude = 0;
        partial = false;
      }
    }
  }
  if (partial) {
    values_.push_back({negative ? negative.value() : false, magnitude});
  }
}

void Oct::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, values_.size(), Assembler::AssignType::Begin);
}

void Oct::assemble(Assembler &assembler, Chunk &chunk) const {
  std::copy(values_.begin(), values_.end(), chunk.begin());
}

void Instruction::validate(Assembler &assembler) {
  if (exprs_.size() > 3) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  while (exprs_.size() < 3) {
    exprs_.emplace_back(std::make_shared<ZeroExpr>());
  }
  opSpec_ = OpSpec::getOpSpec(operationSymbol_);
}

void Instruction::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 1, Assembler::AssignType::Begin);
}
void Instruction::assemble(Assembler &assembler, Chunk &chunk) const {
  auto &word = *chunk.begin();
  word = opSpec_ ? opSpec_.value()->getWord() : 0;

  OpSpec::Address::ref(word) =
      OpSpec::Address::ref(word) + assembler.evaluate(chunk, *exprs_[0]);
  OpSpec::Tag::ref(word) =
      OpSpec::Tag::ref(word) + assembler.evaluate(chunk, *exprs_[1]);
  OpSpec::Decrement::ref(word) =
      OpSpec::Decrement::ref(word) + assembler.evaluate(chunk, *exprs_[2]);
}

std::ostream &Instruction::print(std::ostream &os, Assembler &assembler,
                                 const Chunk &chunk) const {
  return writeInstruction(os, chunk.getBaseAddr(), *chunk.begin());
}

void Org::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

Section &Org::getSection(Assembler &assembler) const {
  return assembler.addSection(assembler.evaluate(*exprs_[0]));
}

void Org::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

std::ostream &Org::print(std::ostream &os, Assembler &assembler,
                         const Chunk &chunk) const {
  return writeAddress(os, assembler.evaluate(chunk, *exprs_[0]));
}

void Rem::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
}

void Rep::validate(Assembler &assembler) {
  if (exprs_.size() != 2) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

void Rep::allocate(Assembler &assembler, Chunk &chunk) const {
  auto size = assembler.evaluate(chunk, *m_) * assembler.evaluate(chunk, *n_);
  assembler.allocate(chunk, size, Assembler::AssignType::None);
}

void Syn::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Syn::allocate(Assembler &assembler, Chunk &chunk) const {
  assembler.allocate(chunk, 0, Assembler::AssignType::None);
  assembler.defineSymbol(getLocationSymbol(),
                         assembler.evaluate(chunk, *exprs_[0]));
}
