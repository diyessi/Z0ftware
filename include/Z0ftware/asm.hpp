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

#include "Z0ftware/card.hpp"
#include "Z0ftware/exprs.hpp"
#include "Z0ftware/operation.hpp"

#include <functional>
#include <map>
#include <utility>

// Core corresponding to one operation
class Chunk {
public:
  Chunk(std::unique_ptr<Operation> &&operation, addr_t base)
      : operation_(std::move(operation)), base_(base) {}
  Chunk(Chunk &&) = default;
  Chunk &operator=(Chunk &&) = default;

  const Operation *getOperation() const { return operation_.get(); }
  addr_t getBaseAddr() const { return base_; }
  addr_t getSize() const { return addr_t(words_.size()); }
  void setSize(addr_t size) { words_.resize(size); }
  addr_t getEndAddr() const { return base_ + getSize(); }

  auto begin() { return words_.begin(); }
  auto begin() const { return words_.begin(); }
  auto end() { return words_.end(); }
  auto end() const { return words_.end(); }
  void shiftBase(int shift) { base_ += shift; }

private:
  addr_t base_;
  std::unique_ptr<Operation> operation_;
  std::vector<word_t> words_;
};

// Format for binary output
enum class BinaryFormat { Absolute, Relative, Full };

// Contiguous memory covered by its Chunks
class Section {
public:
  Section(addr_t base, BinaryFormat binaryFormat)
      : base_(base), binaryFormat_(binaryFormat) {}
  Section(const Section &) = default;
  Section(Section &&) = default;
  Section &operator=(const Section &) = default;
  Section &operator=(Section &&) = default;

  BinaryFormat getBinaryFormat() const { return binaryFormat_; }
  void setBinaryFormat(BinaryFormat binaryFormat) {
    binaryFormat_ = binaryFormat;
  }
  bool isTransfer() const { return transfer_; }
  void setIsTransfer(bool value) { transfer_ = value; }

  addr_t getBase() const { return base_; }
  void setBase(word_t base) {
    int shift = base - base_;
    base_ = base;
    for (auto &chunk : getChunks()) {
      chunk.shiftBase(shift);
    }
  }
  addr_t getNextAddr() const {
    return chunks_.empty() ? base_ : chunks_.back().getEndAddr();
  }

  std::vector<Chunk> &getChunks() { return chunks_; }
  const std::vector<Chunk> &getChunks() const { return chunks_; }

  addr_t getEndAddr() const {
    return chunks_.empty() ? base_ : chunks_.back().getEndAddr();
  }

  addr_t getAddrSize() const { return getEndAddr() - base_; }

private:
  BinaryFormat binaryFormat_;
  bool transfer_{false};
  addr_t base_;
  std::vector<Chunk> chunks_;
};

class AssemblerEnvironment : public Environment {
public:
  AssemblerEnvironment(Assembler &assembler, const Chunk &chunk)
      : assembler_(assembler), chunk_(chunk) {}

  addr_t getLocation() const override;
  addr_t getSymbolValue(const std::string &string) override;

private:
  Assembler &assembler_;
  const Chunk &chunk_;
};

// A function that can write a segment of memory
using section_writer_t = std::function<void(const Section &)>;

class Assembler {
public:
  // Split variableAndComment into variable, comment on first space. If no
  // space, comment is empty.
  virtual std::pair<std::string_view, std::string_view>
  splitVariableAndComment(const std::string_view &variableandComment) const;

  auto operator[](const std::string_view &symbol) {
    return symbolValues_[{symbol.begin(), symbol.end()}];
  }
  // If symbol is not empty, associate it with value
  void defineSymbol(const std::string_view &symbol, FixPoint value) {
    if (!symbol.empty()) {
      symbolValues_.emplace(std::string{symbol.begin(), symbol.end()}, value);
    }
  }
  [[nodiscard]] addr_t getSymbolValue(const std::string &symbol);

  addr_t evaluate(const Chunk &chunk, const Expr &expr);
  addr_t evaluate(const Expr &expr);

  void appendOperation(std::unique_ptr<Operation> &&operation);
  BinaryFormat getBinaryFormat() const { return binaryFormat_; }
  void setBinaryFormat(BinaryFormat binaryFormat) {
    binaryFormat_ = binaryFormat;
    if (!sections_.empty() && sections_.back().getAddrSize() == 0) {
      sections_.back().setBinaryFormat(binaryFormat);
    }
  }
  auto &getSections() { return sections_; }
  const auto &getSections() const { return sections_; }
  Section &addSection(addr_t base) {
    return sections_.emplace_back(base, binaryFormat_);
  }

  // Start a new section if the previous section was not empty
  Section &getSection() {
    return sections_.empty() ? addSection(0) : sections_.back();
  }

  // Allocate memory for operations.
  enum class AssignType { None, Begin, End };
  void allocate(Chunk &chunk, addr_t size, AssignType assignType);

  void assemble();

  void setDefineLocation(Chunk &chunk) {
    defineLocation_ = chunk.getBaseAddr();
  }

  void setSectionWriter(section_writer_t sectionWriter) {
    sectionWriter_ = sectionWriter;
  }
  section_writer_t getSectionWriter() const { return sectionWriter_; };

  void writeBinarySection(const Section &section);

  const std::map<std::string, FixPoint> &getSymbolValues() const {
    return symbolValues_;
  }

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
  std::map<std::string, FixPoint> symbolValues_;
  std::vector<Section> sections_;
  std::optional<addr_t> defineLocation_;
  section_writer_t sectionWriter_;
  BinaryFormat binaryFormat_{BinaryFormat::Absolute};

  static OperationParsers operationParsers_;
};

class SAPAssembler : public Assembler {
public:
  static constexpr CardTextField field80{cardColumnFirst, cardColumnLast};
  static constexpr CardTextField field72{1, 72};
  static constexpr CardTextField fieldLocationSymbol{1, 6};
  static constexpr CardTextField fieldOperation{8, 3};
  static constexpr CardTextField fieldVariableAndComment{12, 60};

  virtual std::unique_ptr<Operation>
  parseLine(const std::string_view &line) override;
};

#endif
