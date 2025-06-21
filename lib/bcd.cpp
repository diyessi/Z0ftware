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
     " ", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "#", "@", c_invalid,
     c_invalid, c_invalid,
     // 1
     c_invalid, "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_invalid, ",",
     "%", c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_invalid, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_invalid, ".",
     c_lozenge, c_invalid, c_invalid, c_invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Code_page_353
const TapeBCDCharSet BCDIC_A{"BCDIC-A Code Page 353",
                             {// 0
                              " ", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                              "0", "#", "@", ":", ">", c_radical,
                              // 1
                              c_blank, "/", "S", "T", "U", "V", "W", "X", "Y",
                              "Z", c_record_mark, ",", "%", c_gamma, "\\",
                              c_triple_plus,
                              // 2
                              "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
                              "!", CharSpec("#", false), "*", "]", ";", c_delta,
                              // 3
                              "&", "A", "B", "C", "D", "E", "F", "G", "H", "I",
                              "?", ".", c_lozenge, "[", "<", c_group_mark}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Code_page_354
const TapeBCDCharSet BCDIC_B{"BCDIC-B Code Page 354",
                             {// 0
                              " ", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                              "0", c_circle_dot, "'", ":", ">", c_radical,
                              // 1
                              c_blank, "/", "S", "T", "U", "V", "W", "X", "Y",
                              "Z", c_record_mark, ",", "(", c_gamma, "\\",
                              c_triple_plus,
                              // 2
                              "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
                              "!", "#", "*", "]", ";", c_delta,
                              // 3
                              "+", "A", "B", "C", "D", "E", "F", "G", "H", "I",
                              "?", ".", ")", "[", "<", c_group_mark}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code
const IBM704BCDCharSet BCD704{
    "IBM 704 character set",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", c_invalid, "#", "@",
     c_invalid, c_invalid, c_invalid,
     // 1
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_plus_zero, ".",
     c_lozenge, c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_minus_zero, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_record_mark, ",", "%",
     c_invalid, c_invalid, c_invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_storage_style
const IBM704BCDCharSet BCD716G{
    "IBM 716 printer character set G",
    {// 0
     "*", "1", "2", "3", "4", "5", "6", "7", "8", "9", c_invalid, "+", "-",
     c_invalid, c_invalid, c_invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_invalid, ".",
     c_lozenge, c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_invalid, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     "0", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_invalid, ",", "%",
     c_invalid, c_invalid, c_invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#Fortran_character_set
const IBM704BCDCharSet BCD716Fortran{
    "IBM 716 printer Fortran character set",
    {// 0
     "*", "1", "2", "3", "4", "5", "6", "7", "8", "9", c_invalid, "=", "-",
     c_invalid, c_invalid, c_invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_invalid, ".", ")",
     c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_invalid, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     "0", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_invalid, ",", "(",
     c_invalid, c_invalid, c_invalid}};

// https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_storage_style
const IBM704BCDCharSet BCDIBM7090{
    "IBM 7090/7094 character set",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", c_invalid, "=", "\"",
     c_invalid, c_invalid, c_invalid,
     // 1
     "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_plus_zero, ".", ")",
     c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_minus_zero, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_plus_minus, ",", "(",
     c_invalid, c_invalid, c_invalid}};

// clang-format off
// From page 209, Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.
// https://bitsavers.org/pdf/ibm/7090/books/Sherman_Programming_and_Coding_the_IBM_709-7090-7094_Computers_1963.pdf
const IBM704BCDCharSet BCDSherman{
    "Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.",
    {// 0
     "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", c_invalid, "=", "\"",
     c_invalid, c_invalid, c_invalid,
     // 1
     "+", "A", "B", "C", "D", "E", "F", "G", "H", "I", c_invalid, ".", ")",
     c_invalid, c_invalid, c_invalid,
     // 2
     "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", c_invalid, "$", "*",
     c_invalid, c_invalid, c_invalid,
     // 3
     " ", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", c_plus_minus, ",", "(",
     c_invalid, c_invalid, c_invalid}};

bcd_t CPUFromTape(bcd_t tapeBCD) {
    bcd_t cpuBCD = (bcd_t(0) == (tapeBCD & bcd_t(0x10))) ? tapeBCD : tapeBCD ^ bcd_t(0x10);
    if (tapeBCD == bcd_t(0x10)) {
      // '0'
      cpuBCD = bcd_t(0x00);
    } else if (tapeBCD == bcd_t(0x10)) {
      // ' '
      cpuBCD = bcd_t(0x30);
    }
    return cpuBCD;
}

bcd_t tapeFromCPU(bcd_t cpuBCD) {
    bcd_t tapeBCD = (bcd_t(0) == (cpuBCD & bcd_t(0x10))) ? cpuBCD : (cpuBCD ^ bcd_t(0x20));
    if (cpuBCD == bcd_t(0x00)) {
      // '0'
      tapeBCD = bcd_t(0x0A);
    } else if (cpuBCD == bcd_t(0x30)) {
      // ' '
      tapeBCD = bcd_t(0x10);
    }
    return tapeBCD;
}

void TapeBCDCharSet::initCPUChars(charmap_t &cpuChars) const {
  std::fill(cpuChars.begin(), cpuChars.end(), c_invalid);
  for (int tapeBCD = 0; tapeBCD < 64; tapeBCD = tapeBCD++) {
    int cpuBCD = int(CPUFromTape(bcd_t(tapeBCD)));
    auto& c = charMap_[tapeBCD];
    if (c) {
        cpuChars[cpuBCD] = c;
    }
  }
}

void TapeBCDCharSet::initTapeChars(charmap_t &tapeChars) const {
  std::copy(charMap_.begin(), charMap_.end(), tapeChars.begin());
}

void IBM704BCDCharSet::initCPUChars(charmap_t &cpuChars) const {
  std::copy(charMap_.begin(), charMap_.end(), cpuChars.end());
}

void IBM704BCDCharSet::initTapeChars(charmap_t &tapeChars) const {
  std::fill(tapeChars.begin(), tapeChars.end(), c_invalid);
  for (int cpuBCD = 0; cpuBCD < 64; cpuBCD++) {
    if (cpuBCD == 0x0A) {
        // Remapped to space
      continue;
    }
    int tapeBCD = int(tapeFromCPU(bcd_t(cpuBCD)));
    auto &c =charMap_[cpuBCD];
    if (c) { 
        tapeChars[tapeBCD] = c;
    }
  }
}

constexpr bcd_t bcdSwapZeroBlank(bcd_t bcd) {
  return (bcd == bcd_t(0) || bcd == bcd_t(0x10)) ? bcd ^= bcd_t(0x10) : bcd;
}

constexpr bcd_t hbcdFromHollerith(hollerith_t hollerith) {
  bcd_t bcd{0};
  // digits
  for (unsigned digit = 0; digit < 10; ++digit) {
    if (0 != hbit(digit & hollerith)) {
      bcd |= bcd_t(digit);
    }
  }
  // zone
  if (0 != (hbit(12) & hollerith)) {
    bcd |= bcd_t(0x30);
  }
  if (0 != (hbit(11) & hollerith)) {
    bcd |= bcd_t(0x20);
  }
  if (0 != (hbit(0) & hollerith)) {
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
  return hbcdZoneMap[unsigned((bcdVal >> 4) & bcd_t(0x3))] |
         hbcdDigitsMap[unsigned((bcdVal & bcd_t(0xF)))];
}

HBCD::HBCD(hollerith_t hollerith) : bcd_(hbcdFromHollerith(hollerith)) {}

hollerith_t HBCD::getHollerith() const { return hollerithFromHbcd(bcd_); }

namespace {
std::unordered_map<hollerith_t, bcd_t> bcdFromHollerithMapInit() {
  std::unordered_map<hollerith_t, bcd_t> result;
  for (unsigned bcd = 0; bcd < 64; bcd++) {
    result[hollerithFromHbcd(bcd_t(bcd))] = bcd_t(bcd);
  }
  return result;
}

std::array<hollerith_t, bcdSize> hollerithFromBcdInit() {
  std::array<hollerith_t, bcdSize> result;
  for (unsigned bcd = 0; bcd < 64; bcd++) {
    result[bcd] = hollerithFromHbcd(bcd_t(bcd));
  }
  return result;
}

} // namespace

std::unordered_map<hollerith_t, bcd_t> HBCD::bcdFromHollerith_ =
    bcdFromHollerithMapInit();
std::array<hollerith_t, bcdSize> HBCD::hollerithFromBcd_ =
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

const std::array<hollerith_t, bcdSize> &HBCD::getHollerithFromBcdArray() {
  return hollerithFromBcd_;
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
    return bcd_t(0b110000);
  }
  // Rows 9 to 1
  for (int pos = 9; pos > 0; pos--) {
    if (1 == (column & 0x1)) {
      bcd |= bcd_t(pos);
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

bcd_t BCDfromPunches(const std::vector<uint8_t> &punches, bool forTape) {
  if (punches.empty()) {
    return bcd_t(forTape ? 0b010000 : 0b110000);
  }

  bcd_t bcd{0};
  for (auto punch : punches) {
    switch (punch) {
    case 12: {
      bcd |= bcd_t(forTape ? 0b110000 : 0b010000);
      break;
    }
    case 11: {
      bcd |= bcd_t(0b100000);
      break;
    }
    case 0: {
      // 0 is a zone if more than one punch
      if (punches.size() == 1) {
        bcd |= bcd_t(forTape ? 0b001010 : 0b000000);
      } else if (*punches.begin() == 0) {
        bcd |= bcd_t(forTape ? 0b010000 : 0b110000);
      } else {
        bcd |= bcd_t(0b001010);
      }
      break;
    }
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: {
      bcd |= bcd_t(punch);
      break;
    }
    }
  }
  return bcd;
}

bcd_t tapeBCDParity(const std::vector<uint8_t> &rows) {
  return evenParity(BCDfromPunches(rows, true));
}

namespace {
struct BCD_ASCII {
  bcd_t bcd;
  char32_t ascii;
};

// clang-format off
// From page 209, Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.
// https://bitsavers.org/pdf/ibm/7090/books/Sherman_Programming_and_Coding_the_IBM_709-7090-7094_Computers_1963.pdf
BCD_ASCII bcd704[] = {
    {bcd_t(000), '0'},
    {bcd_t(001), '1'},
    {bcd_t(002), '2'},
    {bcd_t(003), '3'},
    {bcd_t(004), '4'},
    {bcd_t(005), '5'},
    {bcd_t(006), '6'},
    {bcd_t(007), '7'},
    {bcd_t(010), '8'},
    {bcd_t(011), '9'},
    // '0' on tape
    //{bcd_t(012), '0'},
    {bcd_t(013), '='},
    {bcd_t(014), '"'},
    {bcd_t(020), '+'},
    {bcd_t(021), 'A'},
    {bcd_t(022), 'B'},
    {bcd_t(023), 'C'},
    {bcd_t(024), 'D'},
    {bcd_t(025), 'E'},
    {bcd_t(026), 'F'},
    {bcd_t(027), 'G'},
    {bcd_t(030), 'H'},
    {bcd_t(031), 'I'},
    {bcd_t(033), '.'},
    {bcd_t(034), ')'},
    {bcd_t(040), '-'},
    {bcd_t(041), 'J'},
    {bcd_t(042), 'K'},
    {bcd_t(043), 'L'},
    {bcd_t(044), 'M'},
    {bcd_t(045), 'N'},
    {bcd_t(046), 'O'},
    {bcd_t(047), 'P'},
    {bcd_t(050), 'Q'},
    {bcd_t(051), 'R'},
    {bcd_t(053), '$'},
    {bcd_t(054), '*'},
    {bcd_t(060), ' '},
    {bcd_t(061), '/'},
    {bcd_t(062), 'S'},
    {bcd_t(063), 'T'},
    {bcd_t(064), 'U'},
    {bcd_t(065), 'V'},
    {bcd_t(066), 'W'},
    {bcd_t(067), 'X'},
    {bcd_t(070), 'Y'},
    {bcd_t(071), 'Z'},
    {bcd_t(073), ','},
    {bcd_t(074), '('},
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

bcd_t BCDFromChar(char ascii) { return BCDSherman.getBCD(ascii); }

char charFromBCD(bcd_t bcd) {
  static auto buildTable = []() {
    std::unordered_map<bcd_t, char32_t> table;
    for (auto bcd : bcd704) {
      table[bcd.bcd] = bcd.ascii;
    }
    return table;
  };
  static auto table = buildTable();
  auto it = table.find(bcd);
  return it == table.end() ? 0x7f : it->second;
}

char ASCIIFromTapeBCD(bcd_t bcd) {
  static auto table = []() {
    std::array<char32_t, 128> table;
    for (auto cu : getFORTRANIVEncoding()) {
      auto tapeBCD = tapeBCDfromBCD(BCDFromColumn(cu.hollerith));
      table[unsigned(evenParity(tapeBCD))] = cu.unicode;
    }
    return table;
  }();
  static char fill{'a'};
  auto i = unsigned(bcd & bcd_t(0x3F));
  char c = table[i];
  if (c == '\0') {
    std::cout << "\n*** SUB " << std::hex << int(i) << " '" << fill
              << "' ***\n";
    c = fill++;
    table[i] = c;
  }
  return c;
}

/// Convert first 6 ASCII chars to big-endian bcd chars
/// If less than 6, pad right with spaces
uint64_t bcd(std::string_view chars) {
  std::uint64_t result =
      std::uint64_t(BCDFromChar(chars.empty() ? ' ' : chars[0]));
  for (int i = 1; i < 6; ++i) {
    result = (result << 6) |
             std::uint64_t(BCDFromChar(i < chars.size() ? chars[i] : ' '));
  }
  return result;
}

std::array<std::uint8_t, bcdSize> bcdEvenParity() {
  static std::array<std::uint8_t, bcdSize> table;
  for (size_t i = 0; i < bcdSize; ++i) {
    std::uint8_t val = (i ^ (i >> 4));
    val ^= (val >> 2);
    val ^= (val >> 1);
    table[i] = (val & 0x01) << 6;
  }
  return table;
}
