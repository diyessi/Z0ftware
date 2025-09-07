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

#include <Z0ftware/field.hpp>
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

/**
 * @brief A generic 6-bit unsigned value.
 */
// TODO: Make a template so UnsignedImp uses T as the op type, not bcd_t.
class bcd_t : public UnsignedImp<bcd_t, 6> {
public:
  using UnsignedImp::UnsignedImp;
};

/**
 * @brief A six bit encoding in tape format
 */
class tape_bcd_t : public bcd_t {
public:
  using bcd_t::bcd_t;

  template <typename OS>
  inline friend OS &operator<<(OS &os, tape_bcd_t &value) {
    return os << value;
  }
};

/**
 * @brief six bit encoding in 704 format
 */
class cpu704_bcd_t : public bcd_t {
public:
  using bcd_t::bcd_t;

  template <typename OS>
  inline friend OS &operator<<(OS &os, cpu704_bcd_t &value) {
    return os << value;
  }
};

#endif
