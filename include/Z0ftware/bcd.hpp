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

// TODO:
//
// Zones 01 and 11 are switched, so check for 0x10 but ^= with 0x20.
//
// CharSet is 64-element char32_t array indexed by HBCD/CBCD and unordered map
// char32_t -> BCD. Base is digits/alphabet/blank. Can extend to a new charset
// by providing BCD->char overrides.
//
// bcd_t, hollerith_t are lower-level than *BCD and should be defined in a
// header for lower-level types. Should be lower-level conversions between these
// types, used by *BCD etc.
//
// Should not allow invalid hollerith->bcd conversions, but might need
// hollerith_t for column view of a binary card.

// There are 64 BCD values and 3 6-bit encodings for the values:
// - 704
// - Tape
// - Card
// In addition, there is a 12-bit Hollerith encoding.
// The 704 encoding will serve as the canonic representation.
//
// A character set is encoded by BCD values. All character sets use the same BCD
// values for digits, uppercase letters and blank. There is variability for
// symbols. Unicoode will serve as the canonic character encoding for those
// characters which appear in Unicode. Unicode symbols with similar appearance
// will be used for other symbols.

#ifndef Z0FTWARE_BCD_HPP
#define Z0FTWARE_BCD_HPP

#include "Z0ftware/hollerith.hpp"
#include "Z0ftware/parity.hpp"

#include <Z0ftware/unicode.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

class CharSpec {
public:
  template <typename T>
  CharSpec(T c, bool canonic = true) : char_(c), canonic_(canonic) {}

  CharSpec() : char_(invalid_), canonic_(false) {}

  bool operator==(const CharSpec &other) const {
    return char_ == other.char_ && canonic_ == other.canonic_;
  }

  bool operator!=(const CharSpec &other) const {
    return char_ != other.char_ || canonic_ != other.canonic_;
  }

  operator bool() const { return char_ != invalid_; }

  operator const Unicode &() const { return char_; }
  operator Unicode() const { return char_; }

  friend inline std::ostream &operator<<(std::ostream &s, const CharSpec &cs) {
    return s << cs.char_;
  }

  const Unicode &getChar() const { return char_; }
  bool isCanonic() const { return canonic_; }

protected:
  static constexpr std::string invalid_{"x"};

  Unicode char_;
  bool canonic_;
};

static const CharSpec c_invalid{};

// Characters not in ASCII
static const CharSpec c_blank{"␢"};
static const CharSpec c_circle_dot{"⊙"};
static const CharSpec c_delta{"Δ"};
static const CharSpec c_gamma{"γ"};
static const CharSpec c_group_mark{"⯒"};
static const CharSpec c_lozenge{"⌑"};
static const CharSpec c_minus_zero{"⦵"};
static const CharSpec c_plus_minus{"±"};
static const CharSpec c_plus_zero{"⨁"};
static const CharSpec c_radical{"√"};
static const CharSpec c_record_mark{"⧧"};
static const CharSpec c_triple_plus{"⧻"};

class TapeBCDCharSet;
class IBM704BCDCharSet;

struct BCDCharSet {
public:
  using charmap_t = std::array<CharSpec, 64>;
  using unicodeMap_t = std::unordered_map<char32_t, bcd_t>;

  BCDCharSet(std::string &&description,
             const std::initializer_list<CharSpec> &chars)
      : description_(std::move(description)) {
    std::copy(chars.begin(), chars.end(), charMap_.begin());
    for (int bcd = 0; bcd < 64; bcd++) {
      auto &c = charMap_[bcd];
      if (c && c.isCanonic()) {
        unicodeMap_[c.getChar()] = bcd_t(bcd);
      }
    }
  }

  BCDCharSet(const std::string &description) : description_() {}

  BCDCharSet() = default;

  const CharSpec &operator[](int index) const { return charMap_[index]; }

  virtual void initCPUChars(charmap_t &) const = 0;
  virtual void initTapeChars(charmap_t &) const = 0;
  virtual void initParityTapeChars(std::array<std::string, 128> &) const {};

  const std::string &getDescription() const { return description_; }

  bcd_t getBCD(char32_t c) const {
    auto it = unicodeMap_.find(c);
    return it == unicodeMap_.end() ? bcd_t(0x7f) : it->second;
  }

protected:
  std::string description_;
  charmap_t charMap_{c_invalid};
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

struct BCDPoint {
  wchar_t point;
  wchar_t symbol;
};

/*
 * The BCD character encoding is a six bit encoding for characters based on the
 * Hollerith encoding for cards. The encodings for digits and alphabetic
 * characters was fixed, but there was some variation for symbols
 *
 * There are three related six bit BCD encodings:
 * 1) Hollerith, where the high two bits specify the zone (0 digits, 1 zone 0
 * S-Z, 2 zone 11 J-R, 3 zone 12 A-I) and the low four bits the digit, ORed with
 * 0x08 if 8 is punched. Blank is mapped to 0x10.
 *
 * 2) Tape, like Hollerith, but 0x00 is moved to 0x0A since tape cannot store
 * 0x00.
 *
 * 3) CPU, like Hollerith, but 0x1* and 0x2* zone encodings are swapped so that
 * BCD ordering matches alphabetic
 *
 * We need to be able to go from any of the BCD variants to the appropriate
 * character in an encoding, and we need to be able to go from a supported
 * character to any of the BCD encodings. Some character sets are described
 * relative to a BCD encoding, some for Hollerith.
 *
 * Hollerith is a sparse 12-bit encoding.
 */
constexpr size_t bcdBits = 6;
constexpr size_t bcdSize = 1 << bcdBits;

// Tape has even parity
constexpr size_t tbcdBits = 7;
constexpr size_t tbcdSize = 1 << tbcdBits;

// Hollerith
class HBCD;
// CPU
class CBCD;
// Tape
class TBCD;

// Hollerith BCD
class HBCD {
public:
  HBCD(hollerith_t hollerith);
  inline HBCD(bcd_t bcd) : bcd_(bcd) {}
  inline HBCD(const HBCD &) = default;
  inline HBCD(const CBCD &);
  inline HBCD(const TBCD &);
  inline HBCD &operator=(const HBCD &) = default;
  inline HBCD &operator=(const CBCD &);
  inline HBCD &operator=(const TBCD &);

  inline operator CBCD() const;
  inline operator TBCD() const;

  inline bcd_t getBCD() const { return bcd_; }
  inline hollerith_t getHollerith() const;

  static const std::unordered_map<hollerith_t, bcd_t> &getBcdFromHollerithMap();

protected:
  bcd_t bcd_;

  static std::unordered_map<hollerith_t, bcd_t> bcdFromHollerith_;
  static std::array<hollerith_t, bcdSize> hollerithFromBcd_;
};

// CPU BCD
class CBCD {
public:
  CBCD(bcd_t bcd) : bcd_(bcd) {}
  inline CBCD(const CBCD &) = default;
  // Swap 0x1* and 0x2*
  inline CBCD(const HBCD &);
  // Swap 0x1* and 0x2*
  // 0A -> 00
  inline CBCD(const TBCD &);
  inline CBCD &operator=(const CBCD &) = default;
  inline CBCD &operator=(const HBCD &);
  inline CBCD &operator=(const TBCD &);
  inline operator HBCD() const;
  inline operator TBCD() const;

  inline bcd_t getBCD() const { return bcd_; }

  static inline bcd_t swapHi(bcd_t bcd) {
    bcd_t hibits = bcd & bcd_t(0x30);
    if (bcd_t(0x10) == hibits || bcd_t(0x20) == hibits) {
      bcd ^= bcd_t(0x30);
    }
    return bcd;
  }

protected:
  bcd_t bcd_;
};

// Tape BCD
class TBCD {
public:
  inline TBCD(tbcd_t tbcd) : tbcd_(tbcd) {}
  inline TBCD(const TBCD &) = default;
  inline TBCD(const CBCD &);
  inline TBCD(const HBCD &);
  inline TBCD &operator=(const TBCD &) = default;
  inline TBCD &operator=(const CBCD &);
  inline TBCD &operator=(const HBCD &);

  inline operator CBCD() const;
  inline operator HBCD() const;

  inline bcd_t getBCD() const { return tbcd_; }

  static inline tbcd_t toTape(bcd_t bcd) {
    if (bcd_t(0) == bcd) {
      bcd = bcd_t(0x0A);
    }
    return evenParity(bcd);
  }

  static inline bcd_t fromTape(tbcd_t tbcd) {
    bcd_t bcd = bcd_t((tbcd_t(0x3F) & tbcd));
    if (bcd_t(0x0A) == bcd) {
      bcd = bcd_t(0);
    }
    return bcd;
  }

protected:
  tbcd_t tbcd_;
};

/* CPU */
inline CBCD::CBCD(const HBCD &hbcd) {
  bcd_ = hbcd.getBCD();
  swapHi(bcd_);
}

inline CBCD::CBCD(const TBCD &tbcd)
    : bcd_(TBCD::fromTape(swapHi(tbcd.getBCD()))) {}

inline CBCD &CBCD::operator=(const HBCD &hbcd) {
  bcd_ = swapHi(hbcd.getBCD());
  return *this;
}

inline CBCD &CBCD::operator=(const TBCD &tbcd) {
  bcd_ = TBCD::fromTape(swapHi(tbcd.getBCD()));
  return *this;
}

inline CBCD::operator HBCD() const { return HBCD(*this); }

inline CBCD::operator TBCD() const { return TBCD(*this); }

/* Hollerith */
inline HBCD::HBCD(const CBCD &cbcd) { bcd_ = CBCD::swapHi(cbcd.getBCD()); }

inline HBCD::HBCD(const TBCD &tbcd) { bcd_ = TBCD::fromTape(tbcd.getBCD()); }

inline HBCD &HBCD::operator=(const CBCD &cbcd) {
  bcd_ = CBCD::swapHi(cbcd.getBCD());
  return *this;
}

inline HBCD &HBCD::operator=(const TBCD &tbcd) {
  bcd_ = TBCD::fromTape(tbcd.getBCD());
  return *this;
}

inline HBCD::operator CBCD() const { return CBCD(*this); }
inline HBCD::operator TBCD() const { return TBCD(*this); }

/* Tape */
inline TBCD::TBCD(const CBCD &cbcd)
    : tbcd_(toTape(CBCD::swapHi(cbcd.getBCD()))) {}

inline TBCD::TBCD(const HBCD &hbcd) : tbcd_(toTape(hbcd.getBCD())) {}

inline TBCD &TBCD::operator=(const CBCD &cbcd) {
  tbcd_ = toTape(CBCD::swapHi(cbcd.getBCD()));
  return *this;
}

inline TBCD &TBCD::operator=(const HBCD &hbcd) {
  tbcd_ = toTape(hbcd.getBCD());
  return *this;
}

inline TBCD::operator CBCD() const { return CBCD(*this); }

inline TBCD::operator HBCD() const { return HBCD(*this); }

char ASCIIFromTapeBCD(bcd_t bcd);
std::array<std::uint8_t, bcdSize> bcdEvenParity();
uint64_t bcd(std::string_view chars);

bcd_t BCDFromColumn(hollerith_t column);
bcd_t tapeBCDfromBCD(bcd_t bcd);

void compareASCII();
#endif
