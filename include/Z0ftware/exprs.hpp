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

#ifndef Z0FTWARE_EXPRS_HPP
#define Z0FTWARE_EXPRS_HPP

// Expression evaluation. Expressions, at least in UASAP, are address
// expressions, i.e. 15-bit twos-complement, so the low 15 bits of an int
// evaluation will be correct.

#include "Z0ftware/word.hpp"

#include <memory>
#include <string>

// Access to named locations
class Environment {
public:
  // The current location, corresponding to '*'
  virtual int getLocation() const = 0;
  // The value of an already defined location
  // Cannot be const since symbols can be defined on first use
  virtual int get(const std::string &string) = 0;
};

class Expr {
public:
  // Needs to be shared_ptr for parser
  using ptr = std::shared_ptr<Expr>;

  Expr() = default;
  virtual ~Expr() = default;

  // For testing.
  // If the expression's type is the same as the function's argument, run the
  // function on the expression and return true. Otherwise return false.
  template <typename Function> bool on(Function function) {
    using ExprType = decltype(&std::get<0>(parameters(function)));
    if (auto cast = dynamic_cast<ExprType>(this)) {
      function(*cast);
      return true;
    }
    return false;
  }

  int value(Environment &environment) {
    return ldb<0,15>(evaluate(environment));
  }

  // Evaluare the expression using the environment for definitions
  virtual int evaluate(Environment &environment) const = 0;
};

class AddExpr : public Expr {
public:
  AddExpr(const Expr::ptr &left, const Expr::ptr &right)
      : left_(std::move(left)), right_(std::move(right)) {}
  Expr::ptr &getLeft() { return left_; }
  Expr::ptr &getRight() { return right_; }

  int evaluate(Environment &environment) const override;

private:
  Expr::ptr left_;
  Expr::ptr right_;
};

class SubtractExpr : public Expr {
public:
  SubtractExpr(const Expr::ptr &left, const Expr::ptr &right)
      : left_(left), right_(right) {}
  Expr::ptr &getLeft() { return left_; }
  Expr::ptr &getRight() { return right_; }

  int evaluate(Environment &environment) const override;

private:
  Expr::ptr left_;
  Expr::ptr right_;
};

class MultExpr : public Expr {
public:
  MultExpr(const Expr::ptr &left, const Expr::ptr &right)
      : left_(left), right_(right) {}
  Expr::ptr &getLeft() { return left_; }
  Expr::ptr &getRight() { return right_; }

  int evaluate(Environment &environment) const override;

private:
  Expr::ptr left_;
  Expr::ptr right_;
};

class DivideExpr : public Expr {
public:
  DivideExpr(const Expr::ptr &left, const Expr::ptr &right)
      : left_(left), right_(right) {}
  Expr::ptr &getLeft() { return left_; }
  Expr::ptr &getRight() { return right_; }

  int evaluate(Environment &environment) const override;

private:
  Expr::ptr left_;
  Expr::ptr right_;
};

class NegativeExpr : public Expr {
public:
  NegativeExpr(const Expr::ptr &value) : value_(value) {}
  Expr::ptr &getValue() { return value_; }

  int evaluate(Environment &environment) const override;

private:
  Expr::ptr value_;
};

class HereExpr : public Expr {
public:
  HereExpr() = default;
  int evaluate(Environment &environment) const override;
};

class ZeroExpr : public Expr {
public:
  ZeroExpr() = default;
  int evaluate(Environment &environment) const override;
};

class IntegerExpr : public Expr {
public:
  IntegerExpr(int value) : value_(value) {}
  int &getValue() { return value_; }
  int evaluate(Environment &environment) const override;

private:
  int value_;
};

class SymbolExpr : public Expr {
public:
  SymbolExpr(std::string_view name) : name_(name) {}
  const std::string &getValue() { return name_; }
  int evaluate(Environment &environment) const override;

private:
  std::string name_;
};

#endif