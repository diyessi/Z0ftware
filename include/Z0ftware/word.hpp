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

// 70x Number definitions (needs reorg)

#ifndef Z0FTWARE_WORD_HPP
#define Z0FTWARE_WORD_HPP

#include "Z0ftware/field.hpp"

using addr_t = unsigned_t<15>;
using word_t = unsigned_t<36>;

constexpr addr_t addr_size = 32768;

template <typename int_t, typename word_t> class Int {
public:
  using F_EXP = BitField<27, 8>;
  using F_MANTISSA = BitField<0, 27>;
  using F_ADDRESS = BitField<0, 15>;
  using base_type = Int<int_t, word_t>;
  Int() = default;
  Int(bool negative, int64_t magnitude) {
    int_t::F_SIGN::ref(word_) = negative;
    int_t::F_MAGNITUDE::ref(word_) = magnitude;
  };
  Int(word_t value) : word_(value) {}

  bool operator==(const Int<int_t, word_t> &other) const {
    return word_ == other.word_;
  }

  operator word_t() const { return word_; }

  int64_t asInt() const {
    return isNegative() ? -getMagnitude() : getMagnitude();
  }

  bool isNegative() const { return int_t::F_SIGN::ref(word_); }
  word_t getMagnitude() const { return int_t::F_MAGNITUDE::ref(word_); }

  template <typename U> int_t operator+(const U &other) const {
    auto otherMagnitude = other.getMagnitude();
    if (isNegative() == other.isNegative()) {
      // Same signs
      return int_t(isNegative(), getMagnitude() + otherMagnitude);
    }
    // Opposite signs, left is AC
    auto difference = getMagnitude() - otherMagnitude;
    if (difference >= 0) {
      return int_t(isNegative(), difference);
    }
    return int_t(!isNegative(), -difference);
  }

  int_t operator-() const { return int_t(!isNegative(), getMagnitude()); }

  template <typename U> Int operator-(const U &other) const {
    return operator+(-other);
  }

  template <typename U> Int operator*(const U &other) const {
    return int_t(isNegative() != other.isNegative(),
                 getMagnitude() * other.getMagnitude());
  }

protected:
  word_t word_{0};
};

class AC : public Int<AC, std::uint64_t> {
public:
  using F_SIGN = BitField<37, 1>;
  using F_MAGNITUDE = BitField<0, 37>;

  using base_type::base_type;
};

class ACMQ : public Int<ACMQ, __uint128_t> {
public:
  using F_SIGN = BitField<73, 1, bool>;
  using F_MAGNITUDE = BitField<0, 73>;

  using base_type::base_type;
};

class Word : public Int<Word, uint64_t> {
public:
  using F_SIGN = BitField<35, 1, bool>;
  using F_MAGNITUDE = BitField<0, 35>;

  Word() = default;
  Word(const Word &) = default;
  Word(bool negative, uint64_t magnitude) : base_type(negative, magnitude) {}
  Word(word_t raw) : base_type(raw) {}
};

class FixPoint : public Word {
public:
  using Word::Word;

  FixPoint() : FixPoint(false, 0) {}
  FixPoint(std::uint64_t raw) : Word(raw) {}
  FixPoint(const FixPoint &fixPoint) = default;
  FixPoint(FixPoint &&fixPoint) = default;
  addr_t asAddress() {
    int64_t value = isNegative() ? -getMagnitude() : getMagnitude();
    return F_ADDRESS::ref(value);
  }
  FixPoint &operator=(const FixPoint &) = default;

  FixPoint operator+(FixPoint &value) const { return *this + value; }

  FixPoint operator-(FixPoint &value) const { return *this - value; }

  FixPoint operator-() const { return FixPoint(!isNegative(), getMagnitude()); }

  FixPoint operator*(FixPoint &value) const { return *this * value; }

  FixPoint operator/(FixPoint &value) const { return *this / value; }
};

class Float : public Word {
public:
  using SIGN = BitField<35, 1>;
  using EXP = BitField<27, 8>;
  using MANTISSA = BitField<0, 27>;

  static constexpr unsigned_t<EXP::bit_size> exp_bias = 128;

  using Word::Word;

  Float(bool sign, signed_t<EXP::bit_size> exponent,
        unsigned_t<MANTISSA::bit_size> mantissa) {
    SIGN::ref(word_) = sign;
    EXP::ref(word_) = (mantissa == 0 ? 0 : exponent + exp_bias);
    MANTISSA::ref(word_) = mantissa;
  }

  Float() = default;
  Float(const Float &) = default;

  Float(double d) {
    SIGN::ref(word_) = std::signbit(d);
    auto exp = std::ilogb(d);
    switch (exp) {
    case FP_ILOGB0:
      break;
    default: {
      // For 70x, 1/2 <= mantiss < 1, so shift exp by 1
      EXP::ref(word_) = exp + 1 + exp_bias;
      MANTISSA::ref(word_) = std::ldexp(d, 26 - exp);
    }
    }
  }

  auto getSign() { return SIGN::ref(word_); }
  auto getExp() { return EXP::ref(word_) - exp_bias; }
  auto getBiasedExp() { return EXP::ref(word_); }
  auto getMantissa() { return MANTISSA::ref(word_); }
};

#endif
