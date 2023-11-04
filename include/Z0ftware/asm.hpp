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

#include <functional>
#include <map>
#include <utility>

using core_t = std::array<uint64_t, 32768>;

struct Segment {
  uint16_t address;
  core_t::iterator first;
  core_t::iterator last;
};

// A function that can write a segment of memory
using segment_writer_t = std::function<void(Segment)>;

class Assembler : public Environment {
public:
  // Split variableAndComment into variable, comment on first space. If no
  // space, comment is empty.
  virtual std::pair<std::string_view, std::string_view>
  splitVariableAndComment(const std::string_view &variableandComment) const;

  auto operator[](const std::string_view &symbol) {
    return symbolValues_[{symbol.begin(), symbol.end()}];
  }
  // If symbol is not empty, associate it with value
  void define(const std::string_view &symbol, FixPoint value) {
    if (!symbol.empty()) {
      symbolValues_.emplace(std::string{symbol.begin(), symbol.end()}, value);
    }
  }
  [[nodiscard]] int get(const std::string &symbol) override;
  [[nodiscard]] int getLocation() const override { return location_; }
  void setLocation(std::uint16_t location) { location_ = location & 077777; }
  std::uint16_t addLocation(int16_t offset = 1) {
    location_ = (location_ + offset) & 077777;
    return location_;
  }

  // Format for binary output
  enum class BinaryFormat { Absolute, Relative, Full };
  BinaryFormat getBinaryFormat() const { return binaryFormat_; }
  void setBinaryFormat(BinaryFormat value) { binaryFormat_ = value; }

  void addInstruction(std::unique_ptr<Operation> &&operation);
  const std::vector<std::unique_ptr<Operation>> &getInstructions() const {
    return operations_;
  }

  // Allocate memory for operations.
  enum class AssignType { None, Begin, End };
  void allocate(const Operation *operation, uint16_t size,
                AssignType assignType);
  [[nodiscard]] const Segment &
  getOperationSegment(const Operation *operation) const {
    return operationSegments_.find(operation)->second;
  }
  [[nodiscard]] Segment &getOperationSegment(const Operation *operation) {
    return operationSegments_.find(operation)->second;
  }

  void setDefineLocation() { defineLocation_ = location_; }

  // Current binary segment for writing
  const Segment &getBinarySegment() const { return binarySegment_; }
  void startBinarySegment(const Operation *operation);

  void setSegmentWriter(segment_writer_t segmentWriter) {
    segmentWriter_ = segmentWriter;
  }
  segment_writer_t getSegmentWriter() const { return segmentWriter_; };

  // If non-empty, write the current segment (from base_location_ to location_)
  // and set base_location_ to location_+1;
  void writeBinarySegment(const Operation *operation);

  const std::map<std::string, FixPoint> &getSymbolValues() const {
    return symbolValues_;
  }
  auto &getCore() { return core_; }

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
  std::unordered_map<const Operation *, Segment> operationSegments_;
  std::optional<std::uint16_t> defineLocation_;
  core_t core_;
  size_t lineNumber_{0};
  BinaryFormat binaryFormat_{BinaryFormat::Absolute};
  Segment binarySegment_{0, core_.begin(), core_.begin()};
  segment_writer_t segmentWriter_;

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
