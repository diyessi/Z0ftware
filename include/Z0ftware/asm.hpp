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

#ifndef Z0FTWARE_ASM_HPP
#define Z0FTWARE_ASM_HPP

#include "Z0ftware/exprs.hpp"
#include "Z0ftware/operation.hpp"

#include <map>

struct InstructionAssembly {
  uint16_t address;
  std::vector<uint64_t> words;
};

class Assembler : public Environment {
public:
  // Split variableAndComment into variable, comment on first space. If no
  // space, comment is empty.
  virtual std::pair<std::string_view, std::string_view>
  splitVariableAndComment(const std::string_view &variableandComment) const;

  auto operator[](const std::string_view &symbol) {
    return symbolValues_[{symbol.begin(), symbol.end()}];
  }
  void define(const std::string_view &symbol, FixPoint value) {
    symbolValues_.emplace(std::string{symbol.begin(), symbol.end()}, value);
  }
  [[nodiscard]] int get(const std::string &symbol) override;
  [[nodiscard]] int getLocation() const override { return location_; }
  void setLocation(std::uint16_t location) { location_ = location & 077777; }
  std::uint16_t addLocation(int16_t offset = 1) {
    location_ = (location_ + offset) & 077777;
    return location_;
  }

  void addInstruction(std::unique_ptr<Operation> &&operation);
  const std::vector<std::unique_ptr<Operation>> &getInstructions() const {
    return operations_;
  }

  void allocate(const Operation *operation, size_t size, bool setStartLocation,
                bool setEndLocation);
  [[nodiscard]] const InstructionAssembly &
  getAssembly(const Operation *operation) const {
    return operationAssemblies_.find(operation)->second;
  }
  [[nodiscard]] InstructionAssembly &getAssembly(const Operation *operation) {
    return operationAssemblies_.find(operation)->second;
  }

  void setDefineLocation() { defineLocation_ = location_; }
  const std::map<std::string, FixPoint> &getSymbolValues() const {
    return symbolValues_;
  }
  void write(InstructionAssembly &assembly);

  using OperationParser = std::function<std::unique_ptr<Operation>()>;
  using OperationParsers = std::map<std::string_view, OperationParser>;

  virtual OperationParser getOperationParser(const std::string_view &operation);

  virtual std::unique_ptr<Operation>
  parseLine(const std::string_view &line) = 0;

  virtual std::unique_ptr<Operation>
  parseFields(const std::string_view &line,
              const std::string_view &locationSymbol,
              const std::string_view &operation,
              const std::string_view &variableAndComment);

  virtual std::vector<Expr::ptr> parseEXP(Operation &operation,
                                          const std::string_view &variable);

private:
  std::vector<std::unique_ptr<Operation>> operations_;
  std::map<std::string, FixPoint> symbolValues_;
  std::uint16_t location_{0};
  std::unordered_map<const Operation *, InstructionAssembly>
      operationAssemblies_;
  std::optional<std::uint16_t> defineLocation_;
  std::array<uint64_t, 32768> core_;
  size_t lineNumber_{0};
  static OperationParsers operationParsers_;
};

class SAPAssembler : public Assembler {
public:
  static constexpr CardTextField field80{1, 80};
  static constexpr CardTextField field72{1, 72};
  static constexpr CardTextField fieldLocationSymbol{1, 6};
  static constexpr CardTextField fieldOperation{8, 3};
  static constexpr CardTextField fieldVariableAndComment{12, 60};

  virtual std::unique_ptr<Operation>
  parseLine(const std::string_view &line) override;
};

#endif
