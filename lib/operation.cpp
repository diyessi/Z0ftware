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

std::pair<std::string_view, std::string_view>
Bcd::splitVariableAndComment(Assembler &assembler,
                             const std::string_view &variableAndComment) {
  return {variableAndComment, ""};
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

void Bcd::allocate(Assembler &assembler) const {
  auto size = values_.size();
  assembler.allocate(this, size, true, false);
}

void Bcd::assemble(Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  std::copy(values_.begin(), values_.end(), assembly.words.begin());
}

void Bes::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for " << operationSymbol_;
  }
  requireLocation(assembler);
}

void Bes::allocate(Assembler &assembler) const {
  auto size = getExprs()[0]->value(assembler);
  assembler.allocate(this, size, false, true);
}
void Bes::assemble(Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  std::fill(assembly.words.begin(), assembly.words.end(), 0);
}

void Bss::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Bss::allocate(Assembler &assembler) const {
  auto size = exprs_[0]->value(assembler);
  assembler.allocate(this, size, true, false);
}

void Bss::assemble(Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  std::fill(assembly.words.begin(), assembly.words.end(), 0);
}

void Dec::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  static PegDECParser parser;
  bool success = parser.parse(variable, values_);
  if (!success) {
    addError() << "Could not parse DEC";
  }
}

void Dec::allocate(Assembler &assembler) const {
  assembler.allocate(this, values_.size(), true, false);
}

void Dec::assemble(Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  std::copy(values_.begin(), values_.end(), assembly.words.begin());
}

void Def::allocate(Assembler &assembler) const {
  assembler.setDefineLocation();
  assembler.allocate(this, 0, false, false);
}

void End::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

void End::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
}

void Equ::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Equ::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
  assembler.define(getLocationSymbol(), exprs_[0]->evaluate(assembler));
}

std::ostream &Equ::print(std::ostream &os, Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  return writeAddress(os, exprs_[0]->evaluate(assembler));
}

void Ful::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
}

void Hed::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  hed_ = variable;
}

void Hed::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
}

void Lib::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  library_ = variable;
}

void Lib::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
}

void Oct::parseVariable(Assembler &assembler,
                        const std::string_view &variable) {
  auto it = variable.begin();
  std::optional<bool> negative;
  std::uint64_t magnitude{0};
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

void Oct::allocate(Assembler &assembler) const {
  assembler.allocate(this, values_.size(), true, false);
}

void Oct::assemble(Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  std::copy(values_.begin(), values_.end(), assembly.words.begin());
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

void Instruction::allocate(Assembler &assembler) const {
  assembler.allocate(this, 1, true, false);
}
void Instruction::assemble(Assembler &assembler) const {
  auto &word = assembler.getAssembly(this).words[0];
  word = opSpec_ ? opSpec_.value()->getWord() : 0;

  OpSpec::Address::ref(word) =
      OpSpec::Address::ref(word) + exprs_[0]->evaluate(assembler);
  OpSpec::Tag::ref(word) =
      OpSpec::Tag::ref(word) + exprs_[1]->evaluate(assembler);
  OpSpec::Decrement::ref(word) =
      OpSpec::Decrement::ref(word) + exprs_[2]->evaluate(assembler);
}

std::ostream &Instruction::print(std::ostream &os, Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  return writeInstruction(os, assembly.address, assembly.words[0]);
}

void Org::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

void Org::allocate(Assembler &assembler) const {
  assembler.setLocation(exprs_[0]->evaluate(assembler));
  assembler.allocate(this, 0, false, false);
}

std::ostream &Org::print(std::ostream &os, Assembler &assembler) const {
  auto &assembly = assembler.getAssembly(this);
  return writeAddress(os, exprs_[0]->evaluate(assembler));
}

void Rem::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
}

void Rep::validate(Assembler &assembler) {
  if (exprs_.size() != 2) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
}

void Rep::allocate(Assembler &assembler) const {
  auto size = m_->evaluate(assembler) * n_->evaluate(assembler);
  assembler.allocate(this, size, false, false);
}

void Syn::validate(Assembler &assembler) {
  if (exprs_.size() != 1) {
    addWarning() << "Incorrect number of expressions for "
                 << getOperationSymbol();
  }
  requireLocation(assembler);
}

void Syn::allocate(Assembler &assembler) const {
  assembler.allocate(this, 0, false, false);
  assembler.define(getLocationSymbol(), exprs_[0]->evaluate(assembler));
}
