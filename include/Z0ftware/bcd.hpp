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

/**
 * @file bcd.hpp
 * @brief Classes and types for working with character encodings.
 *
 * Punched cards and printers use a sparse 12-bit Hollerith character encoding
 * that can be converted to a less sparse 6-bit encoding with an even parity 7th
 * bit. The character '1' through '9' were encoded with their BCD values, i.e.
 * 0x01 through 0x09. Due to constraints imposed by the physical recording
 * format, 0x00 could not be stored on tape, so '0', which already required
 * special treatment, was relocated to '0x0A'. Likewise, blanks required their
 * own special treatment since they would have been encoded as '0' if treated
 * like other values.
 *
 * On the 704, conversion between 12-bit Hollerith and 6-bit BCDIC encodings was
 * performed in software for card readers, punches and printers connected
 * directly to the CPU. It was also possible to transfer card content to tape
 * off-line or to print from tape off-line, in which case the conversion was
 * done by hardware.
 *
 * The tape 6-bit encoding was not convenient for computation because the
 * collation order differed from the binary order and the encoding for '0' was
 * after '9', so on the scientific computing family (704, 709, 7090, 7094)
 * values were rearranged to put the characters into collation order and move
 * '0' to 0x00. The business family of CPUs (702, 705) used the tape encoding.
 *
 * Although the conversion between a particular 12-bit Hollerith value or 6-bit
 * tape or 704 value depended only on the bits, there was variation in how
 * glyphs other than digits and uppercase letters were associated with bits.
 * This varied over time and whether the site was primarily commercial or
 * scientific.
 *
 * The association of characters and one of the two BCD encodings or Hollerith
 * will be called a character set. Character sets can be defined relative to a
 * Hollerith, tape BCD or 704-style BCD encoding and can be converted to a
 * character set relative to another encoding. Some programs, such as the SAP
 * assembler, merged a few character sets so they could treat any of several
 * possible encodings for a glyph as the same character.
 *
 * Most of the glyphs survived into ASCII, but a few are only in Unicode, and a
 * few don't even have Unicode characters. C++ Unicode support is almost
 * non-existent, but operating systems now support UTF-8 if
 * std::setlocale(LC_ALL, "") is called. Character sets currently use 32-bit
 * Unicode characters which are converted to UTF-8 during I/O, but this should
 * be simplified to use UTF-8 strings. For the glyphs that are not in Unicode,
 * an alternate Unicode character is used.
 */

#ifndef Z0FTWARE_BCD_HPP
#define Z0FTWARE_BCD_HPP

#include "Z0ftware/parity.hpp"

#include <Z0ftware/unicode.hpp>

// BCD values are 6 bits, but values are transformed between tape (which also
// includes a 7th even parity bit) and scientific CPUs to make BCD order
// correspond to alphabetic order and to represent the digits with their binary
// equivalents. The BCD values on tape correspond more closely to the Hollerith
// card encoding.
//
// Tape hardware does not support all seven bits 0, so BCD 0 with even parity
// cannot be used on tape. The 0 digit is moved to the "10" position, 0x0A. As a
// result, 0x0A is not used as a character on the scientific CPUs.

class cpu704_bcd_t;
class tape_bcd_t;

constexpr unsigned numCardColumns = 80;
constexpr unsigned cardColumnFirst = 1;
constexpr unsigned cardColumnLast = 80;

constexpr unsigned numCardRows = 12;

/**
 * @brief Card column implementation as 12-bit unsigned
 *
 */
class card_column_t : public UnsignedImp<card_column_t, numCardRows> {
public:
  using UnsignedImp::UnsignedImp;

  inline constexpr operator tape_bcd_t() const;
  inline constexpr operator cpu704_bcd_t() const;

  /**
   * @brief Convert a row number to a bit position
   *
   * Row: 12 11 10|0 1 2 3 4 5 6 7 8 9
   * Bit: 11 10  9|9 8 7 6 5 4 3 2 1 0
   */
  template <typename T>
  static inline constexpr T positionFromRow(const T &row) {
    return row < 10 ? 9 - row : row - 1;
  }

  static constexpr card_column_t fromRows() { return card_column_t(); }

  template <typename Row, typename... MoreRows>
  static constexpr card_column_t fromRows(Row row, MoreRows... moreRows) {
    return card_column_t((value_t(1) << positionFromRow(row)) |
                         fromRows(moreRows...).value());
  }

  template <typename T>
  static inline constexpr card_column_t bitForRow(const T &row) {
    return card_column_t(1) << positionFromRow(row);
  }

  template <typename Row> constexpr bool isSet(const Row &row) const {
    return 0 != (value() & bitForRow(row));
  }
};

inline constexpr card_column_t hollerith() { return 0; }

template <typename Row, typename... MoreRows>
inline constexpr card_column_t hollerith(Row row, MoreRows... moreRows) {
  return card_column_t::fromRows(row) | hollerith(moreRows...);
}

class card_row_t : public UnsignedImp<card_row_t, numCardColumns> {
public:
  using UnsignedImp::UnsignedImp;
};

namespace std {
template <> struct hash<card_column_t> {
  std::size_t operator()(const card_column_t &h) const { return h.value(); }
};
} // namespace std

/**
 * @brief A generic 6-bit unsigned value.
 */
// TODO: Make a template so UnsignedImp uses T as the op type, not bcd_t.
class bcd_t : public UnsignedImp<bcd_t, 6> {
public:
  template <typename T> static inline constexpr T swapZeroBlank(const T &bcd) {
    switch (value_t(bcd)) {
    case 0:
      return 0x10;
    case 0x10:
      return 0x0A;
    default:
      return value_t(bcd);
    }
  }

  template <typename T> static inline constexpr T swapZone(const T &bcd) {
    auto zone = bcd & 0x30;
    return (0 == (bcd & 0x10)) ? T(bcd ^ 0x20) : bcd;
  }

  using UnsignedImp::UnsignedImp;
};

/**
 * @brief A six bit encoding in tape format
 */
class tape_bcd_t : public bcd_t {
public:
  using bcd_t::bcd_t;

  inline explicit constexpr tape_bcd_t(const card_column_t &);
  inline explicit constexpr tape_bcd_t(const cpu704_bcd_t &);

  inline constexpr operator card_column_t() const;
  inline constexpr operator cpu704_bcd_t() const;

  template <typename OS>
  inline friend OS &operator<<(OS &os, tape_bcd_t &value) {
    return os << value;
  }
};

/**
 * @brief six bit unsigned tape value + even parity
 */
class parity_bcd_t : public UnsignedImp<parity_bcd_t, 7> {
public:
  using UnsignedImp::UnsignedImp;
};

/**
 * @brief six bit encoding in 704 format
 */
class cpu704_bcd_t : public bcd_t {
public:
  using bcd_t::bcd_t;

  inline explicit constexpr cpu704_bcd_t(const card_column_t &);
  inline explicit constexpr cpu704_bcd_t(const tape_bcd_t &);

  inline constexpr operator card_column_t() const;
  inline constexpr operator tape_bcd_t() const;

  template <typename OS>
  inline friend OS &operator<<(OS &os, cpu704_bcd_t &value) {
    return os << value;
  }
};

constexpr card_column_t::operator tape_bcd_t() const { return *this; }
constexpr card_column_t::operator cpu704_bcd_t() const { return *this; };

constexpr tape_bcd_t::tape_bcd_t(const card_column_t &column) : bcd_t(0) {
  if (column == hollerith()) {
    // blank
    value_ = 0x10;
  } else if (column == hollerith(0)) {
    value_ = 0x0A;
  } else {
    int start_digit = 10;
    for (int zone = 12; zone >= 10; zone--) {
      if (column.isSet(zone)) {
        value_ |= (zone - 9) * 0x10;
        start_digit = zone - 1;
        break;
      }
    }
    for (int digit = start_digit; digit >= 1; digit--) {
      if (column.isSet(digit)) {
        value_ |= digit;
      }
    }
  }
}

constexpr tape_bcd_t::tape_bcd_t(const cpu704_bcd_t &cpu) {
  value_ = cpu.value();
  if ((value_ & 0x10) != 0) {
    value_ ^= 0x20;
  } else if (value_ == 0x00) {
    value_ = 0x0A;
  }
}

constexpr tape_bcd_t::operator card_column_t() const { return *this; }
constexpr tape_bcd_t::operator cpu704_bcd_t() const { return *this; }

constexpr cpu704_bcd_t::cpu704_bcd_t(const card_column_t &column) {
  if (column == hollerith()) {
    // blank
    value_ = 0x30;
  } else if (column == hollerith(0)) {
    value_ = 0x00;
  } else {
    int start_digit = 10;
    for (int zone = 12; zone >= 10; zone--) {
      if (column.isSet(zone)) {
        value_ |= (13 - zone) * 0x10;
        start_digit = zone - 1;
        break;
      }
    }
    for (int digit = start_digit; digit >= 1; digit--) {
      if (column.isSet(digit)) {
        value_ |= digit;
      }
    }
  }
}

constexpr cpu704_bcd_t::cpu704_bcd_t(const tape_bcd_t &tape)
    : bcd_t(swapZeroBlank(swapZone(tape.value()))) {}

constexpr cpu704_bcd_t::operator tape_bcd_t() const { return *this; }
#endif
