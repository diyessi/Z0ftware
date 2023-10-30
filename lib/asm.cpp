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
#include "Z0ftware/parser.hpp"

#include <iostream>

std::pair<std::string_view, std::string_view>
Assembler::splitVariableAndComment(
    const std::string_view &variableAndComment) const {
  auto space = variableAndComment.find(' ');
  if (space == std::string_view::npos) {
    return {variableAndComment,
            variableAndComment.substr(variableAndComment.size())};
  } else {
    return {variableAndComment.substr(0, space),
            variableAndComment.substr(space + 1)};
  }
}

std::vector<Expr::ptr> Assembler::parseEXP(Operation &operation,
                                           const std::string_view &variable) {
  static PegEXPParser parser;
  std::vector<Expr::ptr> values;
  if (!parser.parse(variable, values)) {
    operation.addError() << "Could not parse expressions";
  };
  return values;
}

void Assembler::addInstruction(std::unique_ptr<Operation> &&operation) {
  operations_.emplace_back(std::move(operation));
  operations_.back()->validate(*this);
  if (!operations_.back()->hasErrors()) {
    operations_.back()->allocate(*this);
  } else {
    std::cout << operations_.back()->getLine() << "\n";
    for (auto error : operations_.back()->getErrors()) {
      std::cout << error.getMessage() << "\n";
    }
  }
}

void Assembler::allocate(const Operation *operation, size_t size,
                         bool setStartLocation, bool setEndLocation) {
  operationAssemblies_[operation] = {location_, std::vector<uint64_t>(size)};
  auto symbol = operation->getLocationSymbol();
  if (setStartLocation & !symbol.empty()) {
    define(symbol, location_);
  }
  addLocation(size);
  if (setEndLocation & !symbol.empty()) {
    define(symbol, location_ - 1);
  }
}

void Assembler::write(InstructionAssembly &assembly) {
  auto start = assembly.address;
  std::copy(assembly.words.begin(), assembly.words.end(),
            core_.begin() + start);
}

[[nodiscard]] int Assembler::get(const std::string &symbol) {
  auto it = symbolValues_.find(symbol);
  if (it != symbolValues_.end()) {
    return it->second;
  }
  if (defineLocation_) {
    auto address = defineLocation_.value();
    defineLocation_ = (address + 1) & 077777;
  }
  throw std::invalid_argument(symbol);
}

std::unique_ptr<Operation>
Assembler::parseFields(const std::string_view &line,
                       const std::string_view &locationSymbol,
                       const std::string_view &operationSymbol,
                       const std::string_view &variableAndComment) {

  auto operation = getOperationParser(operationSymbol)();
  operation->setLine(line);
  operation->setLocationSymbol(locationSymbol);
  operation->setOperationSymbol(operationSymbol);
  auto [variable, comment] =
      operation->splitVariableAndComment(*this, variableAndComment);
  operation->setComment(comment);
  operation->parseVariable(*this, variable);
  return operation;
}

std::map<std::string_view, Assembler::OperationParser>
    Assembler::operationParsers_{
        {"BCD", &Bcd::unique}, {"BES", &Bes::unique}, {"BSS", &Bss::unique},
        {"DEC", &Dec::unique}, {"DEF", &Def::unique}, {"END", &End::unique},
        {"EQU", &Equ::unique}, {"FUL", &Ful::unique}, {"HED", &Hed::unique},
        {"LIB", &Lib::unique}, {"OCT", &Oct::unique}, {"ORG", &Org::unique},
        {"REM", &Rem::unique}, {"REP", &Rep::unique}, {"SYN", &Syn::unique}};

Assembler::OperationParser
Assembler::getOperationParser(const std::string_view &operation) {
  auto it = operationParsers_.find(operation);
  if (it != operationParsers_.end()) {
    return it->second;
  }
  return &Instruction::unique;
}

std::unique_ptr<Operation>
SAPAssembler::parseLine(const std::string_view &line) {
  return parseFields(line, trim(fieldLocationSymbol(line)),
                     fieldOperation(line), fieldVariableAndComment(line));
}
