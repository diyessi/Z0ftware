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

#include "Z0ftware/exprs.hpp"
#include "Z0ftware/parser.hpp"
#include "Z0ftware/word.hpp"

#include <any>
#include <map>
#include <string>

namespace {
FixPoint pegDecimal(const peg::SemanticValues &vs) {
  auto sign = std::any_cast<std::uint8_t>(vs[0]);
  auto num = vs.tokens[0];
  auto expbexp = std::any_cast<ExpBExp_t>(vs[vs.size() - 1]);
  std::ostringstream dstr;
  dstr << (sign ? "-" : "") << num;
  if (std::get<0>(expbexp)) {
    dstr << "E" << std::get<0>(expbexp).value();
  }
  auto d = std::stod(dstr.str());
  if (!std::get<1>(expbexp) &&
      (num.find('.') != std::string::npos || std::get<0>(expbexp))) {
    // Floating point
    auto exp = std::ilogb(d);
    auto mantissa = long(std::ldexp(d, 26 - exp));
    return FixPoint(sign, exp + 1, mantissa);
  }
  auto b = std::get<1>(expbexp) ? 35 - std::get<1>(expbexp).value() : 0;
  d = std::ldexp(d, b);
  // Integer
  return FixPoint(sign, int64_t(d));
}

std::uint8_t pegSignBit(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 0:
  case 2:
    return 0;
  case 1:
    return 1;
  default:
    return 0;
  }
}

Exp_t pegExp(const peg::SemanticValues &vs) {
  auto sign = std::any_cast<std::uint8_t>(vs[0]);
  auto value = vs.token_to_number<int16_t>();
  if (sign == 1) {
    value = -value;
  }
  return value;
}

BExp_t pegBExp(const peg::SemanticValues &vs) {
  auto value = vs.token_to_number<int8_t>();
  return value;
}

Expr::ptr pegInteger(const peg::SemanticValues &vs) {
  auto sign = std::any_cast<std::uint8_t>(vs[0]);
  const auto &digits = std::any_cast<std::string_view>(vs[1]);
  return std::make_shared<IntegerExpr>(FixPoint(
      sign, digits.empty() ? 0 : std::stol(std::string(digits), nullptr, 10)));
}

std::string_view pegDecimalString(const peg::SemanticValues &vs) {
  return vs.token();
}

ExpBExp_t pegExpBExp(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 0: { // Exp BExp
    return ExpBExp_t{std::any_cast<Exp_t>(vs[0]), std::any_cast<BExp_t>(vs[1])};
  }
  case 1: { // BExp Exp
    return ExpBExp_t{std::any_cast<Exp_t>(vs[1]),
                     std::any_cast<BExp_t>((vs[0]))};
  }
  case 2: { // Exp
    return ExpBExp_t{std::any_cast<Exp_t>(vs[0]), BExp_t()};
  }
  case 3: { // BExp
    return ExpBExp_t{Exp_t(), std::any_cast<BExp_t>(vs[0])};
  }
  default: {
    return ExpBExp_t{};
  }
  }
}

const std::string grammarDEC = R"(
    DEC                <- Decimal (',' Decimal)*(' ' .*)?
    Decimal            <- SignBit <  '.' DecimalString / DecimalString '.'? DecimalString? > ExpBExp
    ExpBExp            <- Exp BExp / BExp Exp / Exp / BExp / ''
    Exp                <- 'E' SignBit < [0-9]+ >
    BExp               <- 'B' < [0-9]{1,2} >
    SignBit            <- '+' / '-' / ''
    DecimalString      <- < DecimalChar+ >
    DecimalChar        <- [0-9]
)";
} // namespace

PegDECParser::PegDECParser() : peg::parser(grammarDEC) {
  (*this)["DEC"] = [](const peg::SemanticValues &vs) -> std::vector<FixPoint> {
    std::vector<FixPoint> numbers;
    for (auto v : vs) {
      numbers.push_back(std::any_cast<FixPoint>(v));
    }
    return numbers;
  };

  (*this)["Decimal"] = pegDecimal;
  (*this)["ExpBExp"] = pegExpBExp;
  (*this)["Exp"] = pegExp;
  (*this)["BExp"] = pegBExp;
  (*this)["SignBit"] = pegSignBit;
  (*this)["DecimalString"] = pegDecimalString;
  set_logger([](size_t line, size_t col, const std::string &msg,
                const std::string &rule) {
    std::cerr << line << ":" << col << ": " << msg << "\n";
  });
  enable_packrat_parsing();
};

namespace {

Expr::ptr pegExpr(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 0:
    if (vs.tokens[0] == "+") {
      return std::make_shared<AddExpr>(std::any_cast<Expr::ptr>(vs[0]),
                                       std::any_cast<Expr::ptr>(vs[1]));
    } else {
      return std::make_shared<SubtractExpr>(std::any_cast<Expr::ptr>(vs[0]),
                                            std::any_cast<Expr::ptr>(vs[1]));
    }
  case 1:
    return std::any_cast<Expr::ptr>(vs[0]);
  default:
    return nullptr;
  }
}

Expr::ptr pegMult(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 0:
    if (vs.tokens[0] == "*") {
      return std::make_shared<MultExpr>(std::any_cast<Expr::ptr>(vs[0]),
                                        std::any_cast<Expr::ptr>(vs[1]));
    } else {
      return std::make_shared<DivideExpr>(std::any_cast<Expr::ptr>(vs[0]),
                                          std::any_cast<Expr::ptr>(vs[1]));
    }
  case 1:
    return std::any_cast<Expr::ptr>(vs[0]);
  default:
    return nullptr;
  }
}

Expr::ptr pegValue(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 3:
    return std::make_shared<IntegerExpr>(FixPoint(false, 0));
  default:
    return std::any_cast<Expr::ptr>(vs[0]);
  }
}

Expr::ptr pegSValue(const peg::SemanticValues &vs) {
  switch (vs.choice()) {
  case 0:
    return std::make_shared<NegativeExpr>(std::any_cast<Expr::ptr>(vs[0]));
  default:
    return std::any_cast<Expr::ptr>(vs[0]);
  }
  return std::any_cast<Expr::ptr>(vs[0]);
}

Expr::ptr pegSymbol(const peg::SemanticValues &vs) {
  uint64_t value = 0;
  for (auto c : vs.token(0)) {
    if ('0' > c || c > '9') {
      return std::make_shared<SymbolExpr>(vs.token(0));
    } else {
      value = 10 * value + c - '0';
    }
  }
  return std::make_shared<IntegerExpr>(value);
}

Expr::ptr pegZero(const peg::SemanticValues &vs) {
  return std::make_shared<ZeroExpr>();
}

Expr::ptr pegHere(const peg::SemanticValues &vs) {
  return std::make_shared<HereExpr>();
}

const std::string grammarEXP = R"(
    # Any number of Exprs for macro args
    EXP                <- Expr (',' Expr)*(' ' .*)?
    Expr               <- Mult < ('+' / '-') > Expr / Mult
    Mult               <- SValue < ('*' / '/') > Mult / SValue
    SValue             <- '-' Value / '+' Value / Value
    # Integer before Symbol because Symbol can start with digit
    # Empty (as 0) last since that is 0
    Value              <- Zero / Here / Symbol / ''
    Zero               <- < '**' >
    Here               <- < '*' >
    Symbol             <- < SymbolChar+ >
    SymbolChar         <- [0-9A-Z#@_&.%]
)";
} // namespace

PegEXPParser::PegEXPParser() : peg::parser(grammarEXP) {
  (*this)["EXP"] = [](const peg::SemanticValues &vs) -> std::vector<Expr::ptr> {
    std::vector<Expr::ptr> result;
    for (auto &v : vs) {
      result.push_back(std::any_cast<Expr::ptr>(v));
    }
    return result;
  };

  (*this)["Expr"] = pegExpr;
  (*this)["Mult"] = pegMult;
  (*this)["SValue"] = pegSValue;
  (*this)["Value"] = pegValue;
  (*this)["Zero"] = pegZero;
  (*this)["Here"] = pegHere;
  (*this)["Symbol"] = pegSymbol;
  set_logger([](size_t line, size_t col, const std::string &msg,
                const std::string &rule) {
    std::cerr << line << ":" << col << ": " << msg << "\n";
  });
  enable_packrat_parsing();
}
