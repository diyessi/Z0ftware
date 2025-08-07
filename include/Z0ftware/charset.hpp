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

// Characters not in ASCII
static const utf8_t utf8_blank{"␢"};
static const utf8_t utf8_circle_dot{"⊙"};
static const utf8_t utf8_delta{"Δ"};
static const utf8_t utf8_gamma{"γ"};
static const utf8_t utf8_group_mark{"⯒"};
static const utf8_t utf8_lozenge{"⌑"};
static const utf8_t utf8_minus_zero{"⦵"};
static const utf8_t utf8_plus_minus{"±"};
static const utf8_t utf8_plus_zero{"⨁"};
static const utf8_t utf8_radical{"√"};
static const utf8_t utf8_record_mark{"⧧"};
static const utf8_t utf8_triple_plus{"⧻"};

// https://bitsavers.org/pdf/ibm/702/22-6173-1_702prelim_Feb56.pdf page 76
// 11-0 -> 0- 2A (Prints &) (pg 79)
// 12-0 -> 0+ 3A (Prints -) (pg 79)
// 0-2-8-> Record mark  (prints Z) (pg 79)
// Pg 25: 0A used in memory for '0'
//

/**
 * @brief Information about the Unicode character to use for a BCD character.
 */
class Glyph {

public:
  template <typename T>
  Glyph(T c, bool canonic = true)
      : utf8_(get_utf8_char(c)), canonic_(canonic) {}

  operator unicode_char_t() const { return get_unicode_char(utf8_); }

  /**
   * @brief Print using utf-8.
   */
  friend inline std::ostream &operator<<(std::ostream &s, const Glyph &cs) {
    return s << cs.utf8_;
  }

  unicode_char_t getUnicodeChar() const { return get_unicode_char(utf8_); }

  utf8_t getUtf8Char() const { return utf8_; }

  bool isCanonic() const { return canonic_; }

protected:
  utf8_t utf8_;
  bool canonic_;
};

class TapeBCDCharSet;
class IBM704BCDCharSet;

class BCDCharSet {
public:

  using charmap_t = std::array<utf8_t, 64>;
  using utf8_map_t = std::unordered_map<utf8_t, bcd_t>;

  static Glyph invalid;

  BCDCharSet(std::string &&description,
             const std::initializer_list<Glyph> &glyphs)
      : description_(std::move(description)) {
    int bcd = 0;
    for (auto &glyph : glyphs) {
      charMap_[bcd] = glyph.getUtf8Char();
      if (glyph.getUtf8Char() != utf8_replacement && glyph.isCanonic()) {
        utf8_map_[glyph.getUtf8Char()] = bcd_t(bcd);
      }
      bcd++;
    }
  }

  BCDCharSet(const std::string &description) : description_() {}

  BCDCharSet() = default;

  const utf8_t &operator[](int index) const { return charMap_[index]; }

  virtual void initCPUChars(charmap_t &) const = 0;
  virtual void initTapeChars(charmap_t &) const = 0;
  virtual void initParityTapeChars(std::array<std::string, 128> &) const {};

  const std::string &getDescription() const { return description_; }

  bcd_t getCPUBCD(const utf8_string_view_t &sv) const {
    return getCPUBCD(utf8_t(sv));
  }

  bcd_t getCPUBCD(const utf8_t &c) const {
    auto it = utf8_map_.find(c);
    return it == utf8_map_.end() ? bcd_t(0x7f) : it->second;
  }

protected:
  std::string description_;
  charmap_t charMap_{};
  utf8_map_t utf8_map_;
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
