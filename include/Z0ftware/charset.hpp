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
#include "Z0ftware/hollerith.hpp"
#include "Z0ftware/parity.hpp"
#include "Z0ftware/unicode.hpp"

#include <array>

/**
 * @brief Information about the UTF8 to use for a BCD or Hollerith character. In
 * some character tables, the same encoding is used for more than one glyph. In
 * this case, canonic is false for the secondary encodings. Only canonic glyphs
 * are mapped to encodings in a character set.
 */
class Glyph {

public:
  template <typename T>
  Glyph(T c, bool canonic = true)
      : utf8_(get_utf8_char(c)), canonic_(canonic) {}

  Glyph() : utf8_(utf8_replacement), canonic_(false) {}

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

  bool isValid() const { return utf8_ != utf8_replacement; }

protected:
  utf8_t utf8_;
  bool canonic_;
};

/**
 * Most early IBM characters made their way into ASCII. Of the ones that did
 * not, some made it into Unicode and some are approximated with other Unicode
 * characters.
 */

// IBM Characters in Unicode but not in ASCII
static const utf8_t utf8_blank{"␢"};
static const utf8_t utf8_circle_dot{"⊙"};
static const utf8_t utf8_delta{"Δ"};
static const utf8_t utf8_gamma{"γ"};
static const utf8_t utf8_lozenge{"⌑"};
static const utf8_t utf8_radical{"√"};
static const utf8_t utf8_cent{"¢"};
static const utf8_t utf8_not_sign{"¬"};

// Approximations for IBM characters not in Unicode
static const utf8_t utf8_minus_zero{"⦵"};
static const utf8_t utf8_plus_minus{"±"};
static const utf8_t utf8_plus_zero{"⨁"};
static const utf8_t utf8_record_mark{"⧧"};
static const utf8_t utf8_triple_plus{"⧻"};
static const utf8_t utf8_group_mark{"⯒"};

template<typename T>
using glyphs_t = std::array<utf8_t, 1 << T::bit_size()>;

using parity_glyphs_t = glyphs_t<parity_bcd_t>;

std::unique_ptr<parity_glyphs_t> getOctalLowGlyphs();
std::unique_ptr<parity_glyphs_t> getOctalHighGlyphs();

class CharsetForTape {
public:
  virtual ~CharsetForTape() = default;
  virtual std::unique_ptr<parity_glyphs_t> getTapeCharset(bool alternate) const = 0;
};

struct CollateGlyphCardTapeItem {
  size_t collate;
  std::vector<Glyph> glyphs;
  hollerith_t hc;
  tape_bcd_t sc;
};

class CollateGlyphCardTape : public CharsetForTape {
public:
  using items_t = std::array<CollateGlyphCardTapeItem, 64>;

  CollateGlyphCardTape(items_t &&items) : items_(std::move(items)) {}
  const items_t &getItems() const { return items_; }

  std::unique_ptr<parity_glyphs_t> getTapeCharset(bool alternate) const override;

  items_t items_;
};

extern CollateGlyphCardTape collateGlyphCardTape;

struct CardGlyph {
  hollerith_t hollerith;
  Glyph glyph;
};

class TapeBCDCharSet;
class IBM704BCDCharSet;

class BCDCharSet {
public:
  using charmap_t = std::array<utf8_t, 64>;
  using utf8_map_t = std::unordered_map<utf8_t, bcd_t>;

  BCDCharSet(std::string &&description,
             const std::initializer_list<Glyph> &glyphs0,
             const std::initializer_list<Glyph> &glyphs1,
             const std::initializer_list<Glyph> &glyphs2,
             const std::initializer_list<Glyph> &glyphs3)
      : description_(std::move(description)) {
    bcd_t bcd = 0;
    auto add_glyphs = [&bcd, this](const std::initializer_list<Glyph> &glyphs) {
      for (auto &glyph : glyphs) {
        charMap_[bcd.value()] = glyph.getUtf8Char();
        if (glyph.isCanonic()) {
          utf8_map_[glyph.getUtf8Char()] = bcd_t(bcd);
        }
        bcd++;
      };
    };
    add_glyphs(glyphs0);
    add_glyphs(glyphs1);
    add_glyphs(glyphs2);
    add_glyphs(glyphs3);
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

  HollerithChar(const hollerith_t &column, char32_t unicode)
      : column(column), unicode(unicode) {}
  HollerithChar(const hollerith_t &column, utf8_t utf8)
      : column(column), utf8_(utf8) {}

  hollerith_t column;
  char32_t unicode;
  utf8_t utf8_;
};

const std::vector<HollerithChar> &get029Encoding();
const std::vector<HollerithChar> &get026CommercialEncoding();
const std::vector<HollerithChar> &getFAPEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding();
const std::vector<HollerithChar> &getFORTRANIVEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding4();
const std::vector<HollerithChar> &getBCDIC1();

#endif
