// MIT License
//
// Copyright (c) 2023-2025 Scott Cyphers
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

// https://bitsavers.org/pdf/ibm/702/22-6173-1_702prelim_Feb56.pdf page 76
// 11-0 -> 0- 2A (Prints &) (pg 79)
// 12-0 -> 0+ 3A (Prints -) (pg 79)
// 0-2-8-> Record mark  (prints Z) (pg 79)
// Pg 25: 0A used in memory for '0'
//

// HollerithChar, SerialChar, BCDChar are values that can represent
// a character. There is a mapping between most values of different kinds of
// Char.
//
// An XCharSet is a mapping betwern XChars and characters.
// A Collation is an ordering of Chars.
//

#ifndef Z0FTWARE_CHARACTERS_HPP
#define Z0FTWARE_CHARACTERS_HPP

#include "Z0ftware/bcd.hpp"

// 12-bit
class HollerithChar;

// 702/705
class SerialChar;

// 701/704/709
class BCDChar;

// 12-bit
// Row: 12 11 10/0 1 2 3 4 5 6 7 8 9
// Bit: 11 10    9 8 7 6 5 4 3 2 1 0
class HollerithChar {
public:
  using row_t = unsigned;

  HollerithChar() = default;
  HollerithChar(const HollerithChar &) = default;
  ~HollerithChar() = default;

  bool operator==(const HollerithChar &c) { return getBits() == c.getBits(); }

  card_column_t getBits() const { return bits_; }
  operator bcd_t() const;
  // operator BCDChar() const;

  // Row: 12 11 10/0 1 2 3 4 5 6 7 8 9
  // Bit: 11 10    9 8 7 6 5 4 3 2 1 0
  static inline constexpr unsigned positionFromRow(row_t row) {
    return row < 10 ? 9 - row : row - 1;
  }

  inline constexpr card_column_t bitFromRow(row_t row) const {
    return (bits_ >> (positionFromRow(row))) & 1;
  }

protected:
  card_column_t bits_{hollerith()};
};

// Used on tape and character at a time computers such as 702, 705
class SerialChar {
public:
  SerialChar() = default;
  SerialChar(const SerialChar &) = default;
  template <typename T> SerialChar(const T &bits) : bits_(bits) {}
  inline SerialChar(BCDChar c);
  inline bool operator==(const SerialChar &) const = default;

  bcd_t getBits() const { return bits_; }

  static inline bcd_t serialFromBCD(BCDChar c);

protected:
  bcd_t bits_{0};
};

// Used on scientific computers such as 701(?), 704, 709
class BCDChar {
public:
  BCDChar() = default;
  BCDChar(const BCDChar &) = default;
  BCDChar(bcd_t bits) : bits_(bits) {}
  BCDChar(SerialChar c) : bits_(bcdFromSerial(c)) {}

  bcd_t getBits() const { return bits_; }

  static bcd_t bcdFromSerial(SerialChar c) {
    bcd_t bits = c.getBits();
    // Swap zones 12 and 0
    if (0 != (bits & 0x10)) {
      bits ^= 0x20;
    }
    return bits;
  }

protected:
  bcd_t bits_{0};
};

bcd_t SerialChar::serialFromBCD(BCDChar c) {
  bcd_t bits = c.getBits();
  // Swap 12 and 0
  if (0 != (bits & 0x10)) {
    bits ^= 0x20;
  }
  return bits;
}

SerialChar::SerialChar(BCDChar c) : bits_(serialFromBCD(c)) {}

#endif
