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

#ifndef Z0FTWARE_CHARSET_HPP
#define Z0FTWARE_CHARSET_HPP

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/unicode.hpp"

// https://bitsavers.org/pdf/ibm/702/22-6173-1_702prelim_Feb56.pdf page 76
// 11-0 -> 0- 2A (Prints &) (pg 79)
// 12-0 -> 0+ 3A (Prints -) (pg 79)
// 0-2-8-> Record mark  (prints Z) (pg 79)
// Pg 25: 0A used in memory for '0'
//

/**
 * @brief Information about the Unicode character to use for a BCD character.
 */
class BCDCharDef {

public:
  template <typename T>
  BCDCharDef(T c, bool canonic = true)
      : char_(get_unicode_char(c)), canonic_(canonic) {}

  operator unicode_char_t() const { return char_; }

  /**
   * @brief Print using utf-8.
   */
  friend inline std::ostream &operator<<(std::ostream &s,
                                         const BCDCharDef &cs) {
    return s << Unicode(cs.char_);
  }

  const unicode_char_t getChar() const { return char_; }

  bool isCanonic() const { return canonic_; }

protected:
  unicode_char_t char_;
  bool canonic_;
};

class TapeBCDCharSet;
class IBM704BCDCharSet;

class BCDCharSet {
public:
  static constexpr unicode_char_t invalid = unicode_char_invalid;

  // TODO Switch these to utf-8 since about all they get used for is in utf-8
  // contexts
  using charmap_t = std::array<unicode_char_t, 64>;
  using unicodeMap_t = std::unordered_map<unicode_char_t, bcd_t>;

  BCDCharSet(std::string &&description,
             const std::initializer_list<BCDCharDef> &chars)
      : description_(std::move(description)) {
    int bcd = 0;
    for (auto &c : chars) {
      charMap_[bcd] = c.getChar();
      if (c.getChar() != invalid && c.isCanonic()) {
        unicodeMap_[c.getChar()] = bcd_t(bcd);
      }
      bcd++;
    }
  }

  BCDCharSet(const std::string &description) : description_() {}

  BCDCharSet() = default;

  const unicode_char_t &operator[](int index) const { return charMap_[index]; }

  virtual void initCPUChars(charmap_t &) const = 0;
  virtual void initTapeChars(charmap_t &) const = 0;
  virtual void initParityTapeChars(std::array<std::string, 128> &) const {};

  const std::string &getDescription() const { return description_; }

  bcd_t getCPUBCD(char32_t c) const {
    auto it = unicodeMap_.find(c);
    return it == unicodeMap_.end() ? bcd_t(0x7f) : it->second;
  }

protected:
  std::string description_;
  charmap_t charMap_{get_unicode_char(BCDCharSet::invalid)};
  unicodeMap_t unicodeMap_;
};

class TapeBCDCharSet : public BCDCharSet {
public:
  using BCDCharSet::BCDCharSet;

  TapeBCDCharSet(const BCDCharSet &charSet)
      : BCDCharSet(charSet.getDescription()) {
    charSet.initTapeChars(charMap_);
  }

  TapeBCDCharSet() = default;

  void initCPUChars(charmap_t &) const override;
  void initTapeChars(charmap_t &) const override;
};

class IBM704BCDCharSet : public BCDCharSet {
public:
  using BCDCharSet::BCDCharSet;

  IBM704BCDCharSet(const BCDCharSet &charSet)
      : BCDCharSet(charSet.getDescription()) {
    charSet.initTapeChars(charMap_);
  }

  IBM704BCDCharSet() = default;

  void initCPUChars(charmap_t &) const override;
  void initTapeChars(charmap_t &) const override;
};

// Tape/CPU character sets in use on 704 through 7094
extern const TapeBCDCharSet BCD1;
extern const TapeBCDCharSet BCDIC_A;
extern const TapeBCDCharSet BCDIC_B;
extern const IBM704BCDCharSet BCD704;
extern const IBM704BCDCharSet BCD716G;
extern const IBM704BCDCharSet BCD716Fortran;
extern const IBM704BCDCharSet BCDIBM7090;
extern const IBM704BCDCharSet BCDSherman;
extern const TapeBCDCharSet BCDICFinal_A;
extern const TapeBCDCharSet BCDICFinal_B;

uint64_t bcd(utf8_string_view_t chars);

// Pairs a Hollerith encoding with a unicode character
struct HollerithChar {

  HollerithChar(const card_column_t &column, char32_t unicode)
      : column(column), unicode(unicode) {}
  card_column_t column;
  char32_t unicode;
};

const std::vector<HollerithChar> &get029Encoding();
const std::vector<HollerithChar> &get026CommercialEncoding();
const std::vector<HollerithChar> &getFAPEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding();
const std::vector<HollerithChar> &getFORTRANIVEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding4();
const std::vector<HollerithChar> &getBCDIC1();

#endif
