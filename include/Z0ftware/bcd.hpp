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

// BCD characters
//
// A number of 6-bit "BCD" character encodings were used, depending on the
// application, storage medium, and CPU. Not all characters became ASCII
// characters or even Unicode.
//
// Here, 6-bit characters (7-bit with parity) are embedded in std::uint8_t
// Six 6-bit characters were big-endian packed into a 36-bit word, embedded here
// in a std::uint64_t.

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

#ifndef Z0FTWARE_BCD_HPP
#define Z0FTWARE_BCD_HPP

#include "Z0ftware/hollerith.hpp"
#include "Z0ftware/parity.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>

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
 */
constexpr size_t bcdBits = 6;
constexpr size_t bcdSize = 1 << bcdBits;

// Tape has even parity
constexpr size_t tbcdBits = 7;
constexpr size_t tbcdSize = 1 << tbcdBits;

class HBCD;
class CBCD;
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

  static const std::unordered_map<hollerith_t, bcd_t>& getBcdFromHollerithMap();
  static const std::array<hollerith_t, bcdSize>& getHollerithFromBcdArray();

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

// Decode tape bcd with even parity
char32_t char32Decoder(bcd_t tapeBCD);

char charFromBCD(bcd_t bcd);
char ASCIIFromTapeBCD(bcd_t bcd);
bcd_t BCDFromChar(char ascii);
std::array<std::uint8_t, bcdSize> bcdEvenParity();
uint64_t bcd(std::string_view chars);

// Given selected card rows, ordered top to bottom, return the BCD value for
// tape
bcd_t BCDfromPunches(const std::vector<uint8_t> &punches, bool forTape);
bcd_t tapeBCDParity(const std::vector<uint8_t> &punches);

bcd_t BCDFromColumn(hollerith_t column);
bcd_t tapeBCDfromBCD(bcd_t bcd);

void compareASCII();
#endif
