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

#ifndef Z0FTWARE_OPERATION_HPP
#define Z0FTWARE_OPERATION_HPP

#include "Z0ftware/exprs.hpp"
#include "Z0ftware/utils.hpp"

#include <memory>
#include <string>

class Assembler;
class InputColumnCard;
class OpSpec;

class Operation {
public:
  using ptr = std::shared_ptr<Operation>;
  using const_ptr = std::shared_ptr<const Operation>;

  virtual ~Operation() = default;

  template <typename Function> bool on(Function function) {
    using OperationType = decltype(&std::get<0>(parameters(function)));
    if (auto cast = dynamic_cast<OperationType>(this)) {
      function(*cast);
      return true;
    }
    return false;
  }

  // Split variable/comment according to assembler, set the comment and return
  // the variable
  virtual std::pair<std::string_view, std::string_view>
  splitVariableAndComment(Assembler &assembler,
                          const std::string_view &variableAndComment);
  virtual void parseVariable(Assembler &assembler,
                             const std::string_view &variable);

  virtual void validate(Assembler &assembler) {}
  // Allocate location in assembler
  virtual void allocate(Assembler &assembler) const = 0;
  virtual void assemble(Assembler &assembler) const = 0;
  virtual std::ostream &print(std::ostream &os, Assembler &assembler) const {
    return os << "                      ";
  };
  virtual bool isError() const { return false; }

  void setLocationSymbol(const std::string_view &locationSymbol) {
    locationSymbol_ = trim(locationSymbol);
  }
  [[nodiscard]] const std::string &getLocationSymbol() const {
    return locationSymbol_;
  }
  void setOperationSymbol(const std::string_view &operation) {
    operationSymbol_ = rightTrim(operation);
  }
  [[nodiscard]] const std::string &getOperationSymbol() const { return operationSymbol_; }

  void setCard(const InputColumnCard &card) { card_ = &card; }
  const InputColumnCard &getCard() const { return *card_; }

  void setComment(const std::string_view &comment) {
    comment_ = rightTrim(comment);
  }
  [[nodiscard]] const std::string &getComment() const { return comment_; }

  void setExprs(std::vector<std::shared_ptr<Expr>> &&exprs) {
    exprs_ = std::move(exprs);
  }
  [[nodiscard]] const std::vector<std::shared_ptr<Expr>> &getExprs() const {
    return exprs_;
  }

  class Report {
  public:
    Report(std::string &&message) : message_(std::move(message)) {}
    const std::string &getMessage() { return message_; }

  private:
    std::string message_;
  };

  MessageGenerator addWarning() {
    return MessageGenerator([this](std::string &&message) {
      warnings_.emplace_back(Report(std::move(message)));
    });
  }

  MessageGenerator addError() {
    return MessageGenerator([this](std::string &&message) {
      errors_.emplace_back(std::move(Report(std::move(message))));
    });
  }

  void requireLocation(Assembler &assembler) {
    if (locationSymbol_.empty()) {
      addError() << "Location must be set for " << operationSymbol_;
    }
  }
  const std::vector<Report> &getWarnings() const { return warnings_; }
  const std::vector<Report> &getErrors() const { return errors_; }
  bool hasErrors() const { return !getErrors().empty(); }

protected:
  const InputColumnCard *card_{nullptr};
  std::string locationSymbol_;
  std::string operationSymbol_;
  std::string comment_;
  std::vector<std::shared_ptr<Expr>> exprs_;
  std::vector<Report> warnings_;
  std::vector<Report> errors_;
};

template <class T> class OperationImpl : public Operation {
public:
  static std::unique_ptr<T> unique() { return std::make_unique<T>(); }
};

// Packed characters, big-endian. If variable starts with ' ' there are 10
// packed character groups (six each). Otherwise, variable has a digit which
// is how many groups of packed characters there are.
//
// locationSymbol = location
// location += number of character groups
class Bcd : public OperationImpl<Bcd> {
public:
  const auto &getValues() const { return values_; }

  // No comment
  std::pair<std::string_view, std::string_view>
  splitVariableAndComment(Assembler &assembler,
                          const std::string_view &variableAndComment) override;
  void parseVariable(Assembler &assembler,
                     const std::string_view &variable) override;
  void validate(Assembler &assembler) override {}
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;

private:
  std::vector<FixPoint> values_;
};

// Blocked Ended by Symbol
//
// location += Exprs[0]
// locationSymbol = location - 1
class Bes : public OperationImpl<Bes> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;
};

// Block started by symbol:
//
// locationSymbol = location
// location += Exprs[0]
class Bss : public OperationImpl<Bss> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;
};

// Comma-separated decimal data (integer/float)
//
// locationSymbol = location
// location += number of values
class Dec : public OperationImpl<Dec> {
public:
  [[nodiscard]] const auto &getValues() const { return values_; }

  void parseVariable(Assembler &assembler,
                     const std::string_view &variable) override;
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;

private:
  std::vector<FixPoint> values_;
};

// Undefined symbols after DEF card are allocated addresses started at
// Exprs[0]. DEF cards after the first DEF card are ignored.
class Def : public OperationImpl<Def> {
public:
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}
};

// Ends the source.
//
// A binary correction/transfer card is punched with Exprs[0] as the transfer
// address
class End : public OperationImpl<End> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}
};

// locationSymbol = Exprs[0] for non-addresses
class Equ : public OperationImpl<Equ> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

  std::ostream &print(std::ostream &os, Assembler &assembler) const override;
};

class Ful : public OperationImpl<Ful> {
public:
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}
};

// Prefix every symbol following (until another HED) with character 1 (H)
// A symbol of the form "H$K" is treated as K rather than being prefixed
class Hed : public OperationImpl<Hed> {
public:
  void parseVariable(Assembler &assembler,
                     const std::string_view &variable) override;
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

private:
  std::string_view hed_;
};

// Insert library routine named by locationSymbol
// location incremented by routine size
class Lib : public OperationImpl<Lib> {
public:
  void parseVariable(Assembler &assembler,
                     const std::string_view &variable) override;
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

private:
  std::string_view library_;
};

// Comma-separated octal data
//
// locationSymbol = location
// location += number of values
class Oct : public OperationImpl<Oct> {
public:
  [[nodiscard]] const auto &getValues() const { return values_; }

  void parseVariable(Assembler &assembler,
                     const std::string_view &variable) override;
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;

private:
  std::vector<FixPoint> values_;
};

class Instruction : public OperationImpl<Instruction> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override;

  std::ostream &print(std::ostream &os, Assembler &assembler) const override;

private:
  std::string_view op_;
  std::optional<const OpSpec*> opSpec_;
};

// Location = Exprs[0]
class Org : public OperationImpl<Org> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

  std::ostream &print(std::ostream &os, Assembler &assembler) const override;
};

// Comment is entire variable/comment field
class Rem : public OperationImpl<Rem> {
public:
  void validate(Assembler &assembler) override{};
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

private:
};

// Repeat
//
// Repeat previous Exprs[0] words Exprs[1] times
// locationSymbol = location
// location += Exprs[0] * Exprs[1]
class Rep : public OperationImpl<Rep> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}

private:
  Expr::ptr m_;
  Expr::ptr n_;
};

// LocationSymbol = Exprs[0] for addresses
class Syn : public OperationImpl<Syn> {
public:
  void validate(Assembler &assembler) override;
  void allocate(Assembler &assembler) const override;
  void assemble(Assembler &assembler) const override {}
};

#endif
