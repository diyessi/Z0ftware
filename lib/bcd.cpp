// MIT License
//
// Copyright (c) 2025 Scott Cyphers
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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/card.hpp"
#include "Z0ftware/hollerith.hpp"
#include "Z0ftware/parity.hpp"

#include <iostream>
#include <unordered_map>

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#48-character_BCD_code
const TapeBCDCharSet BCD{
    "IBM 48-character BCDIC code",
    {// 0
     BCDCharSet::invalid, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "#",
     "@", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     BCDCharSet::invalid, "/", "S", "T", "U", "V", "W", "X", "Y", "Z",
     BCDCharSet::invalid, ",", "%", BCDCharSet::invalid, BCDCharSet::invalid,
     BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", BCDCharSet::invalid, "$",
     "*", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", BCDCharSet::invalid, ".",
     utf8_lozenge, BCDCharSet::invalid, BCDCharSet::invalid,
     BCDCharSet::invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Code_page_353
const TapeBCDCharSet BCDIC_A{
    "BCDIC-A Code Page 353",
    {// 0
     " ", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "#", "@", ":", ">",
     utf8_radical,
     // 1
     utf8_blank, "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_record_mark,
     ",", "%", utf8_gamma, "\\", utf8_triple_plus,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "!",
     BCDCharDef("#", false), "*", "]", ";", utf8_delta,
     // 3
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", "?", ".", utf8_lozenge,
     "[", "<", utf8_group_mark}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Code_page_354
const TapeBCDCharSet BCDIC_B{
    "BCDIC-B Code Page 354",
    {// 0
     BCDCharSet::invalid, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
     utf8_circle_dot, "'", ":", ">", utf8_radical,
     // 1
     utf8_blank, "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_record_mark,
     ",", "(", utf8_gamma, "\\", utf8_triple_plus,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "!", "#", "*", "]", ";",
     utf8_delta,
     // 3
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", "?", ".", ")", "[", "<",
     utf8_group_mark}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code
const IBM704BCDCharSet BCD704{
    "IBM 704 character set",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", BCDCharSet::invalid, "#",
     "@", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", utf8_plus_zero, ".",
     utf8_lozenge, BCDCharSet::invalid, BCDCharSet::invalid,
     BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", utf8_minus_zero, "$",
     "*", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_record_mark, ",",
     "%", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_storage_style
const IBM704BCDCharSet BCD716G{
    "IBM 716 printer character set G",
    {// 0
     "*", "1", "2", "3", "4", "5", "6", "7", "8", "9", BCDCharSet::invalid, "+",
     "-", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", BCDCharSet::invalid, ".",
     utf8_lozenge, BCDCharSet::invalid, BCDCharSet::invalid,
     BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", BCDCharSet::invalid, "$",
     "*", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     "0", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", BCDCharSet::invalid, ",",
     "%", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Fortran_character_set
const IBM704BCDCharSet BCD716Fortran{
    "IBM 716 printer Fortran character set",
    {// 0
     "*", "1", "2", "3", "4", "5", "6", "7", "8", "9", BCDCharSet::invalid, "=",
     "-", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", BCDCharSet::invalid, ".",
     ")", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", BCDCharSet::invalid, "$",
     "*", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     "0", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", BCDCharSet::invalid, ",",
     "(", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_storage_style
const IBM704BCDCharSet BCDIBM7090{
    "IBM 7090/7094 character set",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", BCDCharSet::invalid, "=",
     "\"", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", utf8_plus_zero, ".", ")",
     BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", utf8_minus_zero, "$",
     "*", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_plus_minus, ",",
     "(", BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid}};

// clang-format off
// From page 209, Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.
// https://bitsavers.org/pdf/ibm/7090/books/Sherman_Programming_and_Coding_the_IBM_709-7090-7094_Computers_1963.pdf
const IBM704BCDCharSet BCDSherman{
    "Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", BCDCharSet::invalid, "=", "\"",
     BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", BCDCharSet::invalid, ".", ")",
     BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", BCDCharSet::invalid, "$", "*",
     BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_plus_minus, ",", "(",
     BCDCharSet::invalid, BCDCharSet::invalid, BCDCharSet::invalid}};

const TapeBCDCharSet BCDICFinal_A {
    "IBM Final BCDIC version",
    {// 0 
        BCDCharSet::invalid, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "#", "@", ":", ">", utf8_radical,
        // 1
        " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_record_mark, ",", "%", utf8_gamma, "\\", utf8_triple_plus,
        // 2
        "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "!", "$", "*", "]", ";", utf8_delta, 
        // 3
        "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", "?", ".", utf8_lozenge, "[", "<", utf8_group_mark
    }};

const TapeBCDCharSet BCDICFinal_B {
    "IBM Final BCDIC version",
    {// 0 
        BCDCharSet::invalid, "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "=", "'", ":", ">", utf8_radical,
        // 1
        " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", utf8_record_mark, ",", "(", utf8_gamma, "\\", utf8_triple_plus,
        // 2
        "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "!", "$", "*", "]", ";", utf8_delta, 
        // 3
        "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", "?", ".", ")", "[", "<", utf8_group_mark
    }};

bcd_t CPUFromTape(bcd_t tapeBCD) {
    bcd_t cpuBCD = (0 == (tapeBCD & 0x10)) ? tapeBCD : tapeBCD ^ 0x10;
    if (tapeBCD == 0x10) {
      // '0'
      cpuBCD = 0x00;
    } else if (tapeBCD == 0x10) {
      // ' '
      cpuBCD = 0x30;
    }
    return cpuBCD;
}

bcd_t tapeFromCPU(bcd_t cpuBCD) {
    bcd_t tapeBCD = (bcd_t(0) == (cpuBCD & bcd_t(0x10))) ? cpuBCD : (cpuBCD ^ bcd_t(0x20));
    if (cpuBCD == 0x00) {
      // '0'
      tapeBCD = 0x0A;
    } else if (cpuBCD == 0x30) {
      // ' '
      tapeBCD = 0x10;
    }
    return tapeBCD;
}

void TapeBCDCharSet::initCPUChars(charmap_t &cpuChars) const {
  std::fill(cpuChars.begin(), cpuChars.end(), Unicode(BCDCharSet::invalid));
  for (bcd_t tapeBCD = bcd_t::begin(); tapeBCD < bcd_t::end(); tapeBCD++) {
    bcd_t cpuBCD = CPUFromTape(tapeBCD);
    auto& c = charMap_[tapeBCD.value()];
    if (c) {
        cpuChars[cpuBCD.value()] = c;
    }
  }
}

void TapeBCDCharSet::initTapeChars(charmap_t &tapeChars) const {
    std::copy(charMap_.begin(), charMap_.end(), tapeChars.begin());
}

void IBM704BCDCharSet::initCPUChars(charmap_t &cpuChars) const {
  std::copy(charMap_.begin(), charMap_.end(), cpuChars.begin());
}

void IBM704BCDCharSet::initTapeChars(charmap_t &tapeChars) const {
  std::fill(tapeChars.begin(), tapeChars.end(), get_unicode_char(BCDCharSet::invalid));
  for (bcd_t cpuBCD = bcd_t::begin(); cpuBCD < bcd_t::end(); cpuBCD++) {
    if (cpuBCD == 0x0A) {
        // Remapped to space
      continue;
    }
    bcd_t tapeBCD = tapeFromCPU(cpuBCD);
    auto &c =charMap_[cpuBCD.value()];
    if (c) { 
        tapeChars[tapeBCD.value()] = c;
    }
  }
}

constexpr bcd_t bcdSwapZeroBlank(bcd_t bcd) {
  return (bcd == 0 || bcd == 0x10) ? bcd ^ 0x10 : bcd;
}

constexpr bcd_t hbcdFromHollerith(hollerith_t hollerith) {
    auto hval = hollerith.value();
  bcd_t bcd{0};
  // digits
  for (unsigned digit = 0; digit < 10; ++digit) {
    if (0 != hbit(digit & hval)) {
      bcd |= bcd_t(digit);
    }
  }
  // zone
  if (0 != (hbit(12) & hval)) {
    bcd |= bcd_t(0x30);
  }
  if (0 != (hbit(11) & hval)) {
    bcd |= bcd_t(0x20);
  }
  if (0 != (hbit(0) & hval)) {
    bcd |= bcd_t(0x10);
  }
  return bcdSwapZeroBlank(bcd);
}

namespace {
constexpr std::array<hollerith_t, 4> hbcdZoneMap = {
    hollerithFromRows({}), hollerithFromRows({0}), hollerithFromRows({11}),
    hollerithFromRows({12})};

constexpr std::array<hollerith_t, 16> hbcdDigitsMap = {
    hollerithFromRows({0}),    hollerithFromRows({1}),
    hollerithFromRows({2}),    hollerithFromRows({3}),
    hollerithFromRows({4}),    hollerithFromRows({5}),
    hollerithFromRows({6}),    hollerithFromRows({7}),
    hollerithFromRows({8}),    hollerithFromRows({9}),
    hollerithFromRows({8, 2}), hollerithFromRows({8, 3}),
    hollerithFromRows({8, 4}), hollerithFromRows({8, 5}),
    hollerithFromRows({8, 6}), hollerithFromRows({8, 7})};

} // namespace

constexpr hollerith_t hollerithFromHbcd(bcd_t bcd) {
  bcd_t bcdVal = bcdSwapZeroBlank(bcd);
  return hbcdZoneMap[((bcdVal >> 4) & 0x3).value()] |
         hbcdDigitsMap[(bcdVal & 0xF).value()];
}

HBCD::HBCD(hollerith_t hollerith) : bcd_(hbcdFromHollerith(hollerith)) {}

hollerith_t HBCD::getHollerith() const { return hollerithFromHbcd(bcd_); }

namespace {
std::unordered_map<hollerith_t, bcd_t> bcdFromHollerithMapInit() {
  std::unordered_map<hollerith_t, bcd_t> result;
  for (bcd_t bcd = bcd_t::begin(); bcd < bcd_t::end(); bcd++) {
    result[hollerithFromHbcd(bcd_t(bcd))] = bcd_t(bcd);
  }
  return result;
}

std::array<hollerith_t, CPU704BCDValue::size> hollerithFromBcdInit() {
  std::array<hollerith_t, CPU704BCDValue::size> result;
  for (bcd_t bcd = bcd_t::begin(); bcd < bcd_t::end(); bcd++) {
    result[bcd.value()] = hollerithFromHbcd(bcd_t(bcd));
  }
  return result;
}

} // namespace

std::unordered_map<hollerith_t, bcd_t> HBCD::bcdFromHollerith_ =
    bcdFromHollerithMapInit();
std::array<hollerith_t, CPU704BCDValue::size> HBCD::hollerithFromBcd_ =
    hollerithFromBcdInit();

const std::unordered_map<hollerith_t, bcd_t> &HBCD::getBcdFromHollerithMap() {
  static bool initialized{false};
  if (!initialized) {
    std::vector<hollerith_t> hzones = {
        hollerithFromRows({}), hollerithFromRows({0}), hollerithFromRows({11}),
        hollerithFromRows({12})};
    for (unsigned bzone = 0; bzone < hzones.size(); ++bzone) {
      auto &hzone = hzones[bzone];
      for (unsigned digit = 0; digit < 16; digit++) {
      }
    }
  }
  return bcdFromHollerith_;
}

bcd_t tapeBCDfromBCD(bcd_t bcd) {
  if (bcd == bcd_t(0)) {
    return bcd_t(0b001010);
  }
  if (bcd_t(0) != (bcd & bcd_t(0b010000))) {
    return bcd ^ bcd_t(0b100000);
  }
  return bcd;
}

bcd_t BCDFromColumn(hollerith_t column) {
  bcd_t bcd{0};
  if (column == 0) {
    return 0b110000;
  }
  // Rows 9 to 1
  for (int pos = 9; pos > 0; pos--) {
    if (1 == (column & 0x1)) {
      bcd |= pos;
    }
    column >>= 1;
  }
  bool hasZero = (1 == (column & 0x1));
  column >>= 1;
  if (column > 0) {
    // Zone 11 or 12 is 1
    if (hasZero) {
      // Zero with a zone 11 or 12 is like a 10
      bcd |= bcd_t(0b1010);
    }
    if (1 == (column & 0x1)) {
      // Zone 11
      bcd |= bcd_t(0b100000);
    }
    column >>= 1;
    if (1 == (column & 0x1)) {
      // Zone 12
      bcd |= bcd_t(0b010000);
    }
  } else if (hasZero && bcd != bcd_t(0)) {
    bcd |= bcd_t(0b110000);
  }
  return bcd;
}


namespace {
struct BCD_ASCII {
  bcd_t bcd;
  char32_t ascii;
};

// MAP in is270-2 pr130-6 defines codes for "- 029" 029 key punch characters
BCD_ASCII mapTape[] = {
    // 029 codes
    {bcd_t(015), '\''},
    {bcd_t(016), '@'},
    {bcd_t(035), '('},
    {bcd_t(036), '+'},
    {bcd_t(055), ')'},

    // 029 card example
    {bcd_t(037), '|'},
    {bcd_t(056), ';'},
    {bcd_t(057), '~'},
    {bcd_t(072), 'c'},
    {bcd_t(075), '_'},
    {bcd_t(076), '>'},
    {bcd_t(077), '?'},

    // Used on tape
    {bcd_t(052), 'o'},
    // Based on .bcd files
    {bcd_t(017), '"'},
    // Based on .bcd files
    {bcd_t(032), '?'},
};

// clang-format on
} // namespace

char ASCIIFromTapeBCD(bcd_t bcd) {
  static auto table = []() {
    std::array<char32_t, 128> table;
    for (auto cu : getFORTRANIVEncoding()) {
      auto tapeBCD = tapeBCDfromBCD(BCDFromColumn(cu.hollerith));
      table[tapeBCD.value()] = cu.unicode;
    }
    return table;
  }();
  static char fill{'a'};
  auto i = bcd & bcd_t(0x3F);
  char c = table[i.value()];
  if (c == '\0') {
    std::cout << "\n*** SUB " << std::hex << i.value() << " '" << fill
              << "' ***\n";
    c = fill++;
    table[i.value()] = c;
  }
  return c;
}

/// Convert first 6 ASCII chars to big-endian bcd chars
/// If less than 6, pad right with spaces
uint64_t bcd(utf8_string_view_t chars) {
  int count = 0;
  std::uint64_t result = 0;
  while (count < 6 && !chars.empty()) {
    result = (result << 6) |
             BCDSherman.getCPUBCD(get_next_unicode_char(chars)).value();
    count++;
  }
  return result;
}
