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

#include "Z0ftware/charset.hpp"

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

void TapeBCDCharSet::initCPUChars(charmap_t &cpuChars) const {
  std::fill(cpuChars.begin(), cpuChars.end(), Unicode(BCDCharSet::invalid));
  for (tape_bcd_t tapeBCD = tape_bcd_t::min(); tapeBCD <= tape_bcd_t::max(); tapeBCD++) {
    cpu704_bcd_t cpuBCD = cpu704_bcd_t(tapeBCD);
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
  for (cpu704_bcd_t cpuBCD = cpu704_bcd_t::min(); cpuBCD <= cpu704_bcd_t::max(); cpuBCD++) {
    if (cpuBCD.value() == 0x0A) {
        // Remapped to space
      continue;
    }
    tape_bcd_t tapeBCD = tape_bcd_t(cpuBCD);
    auto &c =charMap_[cpuBCD.value()];
    if (c) { 
        tapeChars[tapeBCD.value()] = c;
    }
  }
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

// Everything shares the original encoding
// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/024-026/A24-0520-3_24_26_Card_Punch_Reference_Manual_Oct1965.pdf
// Page 6
const std::vector<HollerithChar> &getBaseCardEncoding() {
  static std::vector<HollerithChar> table = {
      // Blank
      {hollerith(), ' '},
      // Digits
      {hollerith(0), '0'},
      {hollerith(1), '1'},
      {hollerith(2), '2'},
      {hollerith(3), '3'},
      {hollerith(4), '4'},
      {hollerith(5), '5'},
      {hollerith(6), '6'},
      {hollerith(7), '7'},
      {hollerith(8), '8'},
      {hollerith(9), '9'},
      // Alphabetic
      {hollerith(12, 1), 'A'},
      {hollerith(12, 2), 'B'},
      {hollerith(12, 3), 'C'},
      {hollerith(12, 4), 'D'},
      {hollerith(12, 5), 'E'},
      {hollerith(12, 6), 'F'},
      {hollerith(12, 7), 'G'},
      {hollerith(12, 8), 'H'},
      {hollerith(12, 9), 'I'},
      {hollerith(11, 1), 'J'},
      {hollerith(11, 2), 'K'},
      {hollerith(11, 3), 'L'},
      {hollerith(11, 4), 'M'},
      {hollerith(11, 5), 'N'},
      {hollerith(11, 6), 'O'},
      {hollerith(11, 7), 'P'},
      {hollerith(11, 8), 'Q'},
      {hollerith(11, 9), 'R'},
      {hollerith(0, 2), 'S'},
      {hollerith(0, 3), 'T'},
      {hollerith(0, 4), 'U'},
      {hollerith(0, 5), 'V'},
      {hollerith(0, 6), 'W'},
      {hollerith(0, 7), 'X'},
      {hollerith(0, 8), 'Y'},
      {hollerith(0, 9), 'Z'},
  };
  return table;
}

namespace {
std::vector<HollerithChar>
createBCDEncoding(std::vector<HollerithChar> &&symbols) {
  auto table = getBaseCardEncoding();
  table.reserve(table.size() + symbols.size());
  std::move(symbols.begin(), symbols.end(), std::inserter(table, table.end()));
  return table;
}
} // namespace

// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/024-026/A24-0520-3_24_26_Card_Punch_Reference_Manual_Oct1965.pdf
// Page 6
const std::vector<HollerithChar> &get026CommercialEncoding() {
  static std::vector<HollerithChar> table = createBCDEncoding({
      //
      {hollerith(12), '&'},
      {hollerith(12, 3, 8), '.'},
      {hollerith(12, 4, 8), u'¤'},
      //
      {hollerith(11), '-'},
      {hollerith(11, 3, 8), '$'},
      {hollerith(11, 4, 8), '*'},
      //
      {hollerith(0, 1), '/'},
      {hollerith(0, 3, 8), ','},
      //
      {hollerith(3, 8), '#'},
      {hollerith(4, 8), '@'} //
  });
  return table;
}

// From:
// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/029/A24-3332-3_29_Reference_Man.pdf
// page 5
const std::vector<HollerithChar> &get029Encoding() {
  static std::vector<HollerithChar> table = createBCDEncoding({
      //
      {hollerith(12), '&'},
      {hollerith(12, 2, 8), u'¢'},
      {hollerith(12, 3, 8), '.'},
      {hollerith(12, 4, 8), '<'},
      {hollerith(12, 5, 8), '('},
      {hollerith(12, 6, 8), '+'},
      {hollerith(12, 7, 8), '|'},
      //
      {hollerith(11), '-'},
      {hollerith(11, 2, 8), '!'},
      {hollerith(11, 3, 8), '$'},
      {hollerith(11, 4, 8), '*'},
      {hollerith(11, 5, 8), ')'},
      {hollerith(11, 6, 8), ';'},
      {hollerith(11, 7, 8), u'¬'},
      //
      {hollerith(0, 1), '/'},
      {hollerith(0, 2, 8), ' '},
      {hollerith(0, 3, 8), ','},
      {hollerith(0, 4, 8), '%'},
      {hollerith(0, 5, 8), '_'},
      {hollerith(0, 6, 8), '>'},
      {hollerith(0, 7, 8), '?'},
      //
      {hollerith(2, 8), ':'},
      {hollerith(3, 8), '#'},
      {hollerith(4, 8), '@'},
      {hollerith(5, 8), '\''},
      {hollerith(6, 8), '='},
      {hollerith(7, 8), '"'} //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/704/704_FortranProgRefMan_Oct56.pdf
// Page 49
const std::vector<HollerithChar> &getFORTRAN704Encoding() {
  static std::vector<HollerithChar> table = createBCDEncoding({
      {hollerith(8, 3), '='},
      // This - cannot be used as a - operation in FORTRAN
      {hollerith(8, 4), '-'},
      {hollerith(12), '+'},
      {hollerith(12, 8, 3), '.'},
      {hollerith(12, 8, 4), ')'},
      {hollerith(11), '-'},
      {hollerith(11, 8, 3), '$'},
      {hollerith(11, 8, 4), '*'},
      {hollerith(0, 1), '/'},
      {hollerith(0, 8, 3), ','},
      {hollerith(0, 8, 4), '('},
  });
  return table;
}

// Coded Character Sets History and Development, C. E. MacKenzie, 1980
// https://archive.org/details/mackenzie-coded-char-sets
// Page 101
const std::vector<HollerithChar> &getFORTRAN704Encoding4() {
  static std::vector<HollerithChar> table =
      createBCDEncoding({{hollerith(8, 3), '='},
                         // This - cannot be used as a - operation in FORTRAN
                         {hollerith(8, 4), '\''},
                         {hollerith(8, 5), ':'},
                         {hollerith(8, 6), '>'},
                         {hollerith(12), '+'},
                         {hollerith(12, 8, 2), '?'},
                         {hollerith(12, 8, 3), '.'},
                         {hollerith(12, 8, 4), ')'},
                         {hollerith(12, 8, 6), '<'},
                         {hollerith(11), '-'},
                         {hollerith(11, 8, 2), '!'},
                         {hollerith(11, 8, 3), '$'},
                         {hollerith(11, 8, 4), '*'},
                         {hollerith(11, 8, 6), ';'},
                         {hollerith(0, 1), '/'},
                         {hollerith(0, 8, 3), ','},
                         {hollerith(0, 8, 4), '('},
                         {hollerith(0, 8, 7), '"'}});
  return table;
}

// http://www.bitsavers.org/pdf/ibm/7090/C28-6235-2_7090_FAP.pdf
// Page 68
// https://bitsavers.org/pdf/ibm/7090/C28-6054-4_7090_FORTRANII.pdf
// Page 46
// https://bitsavers.org/pdf/ibm/7090/C28-6311-4_MAP_Oct64.pdf
// Page 56
// https://bitsavers.org/pdf/ibm/7090/GC28-6392-4_MAP_Dec66.pdf
// Page 58
const std::vector<HollerithChar> &getFAPEncoding() {
  static std::vector<HollerithChar> table = createBCDEncoding(
      {{hollerith(), ' '},
       {hollerith(12), '+'},
       {hollerith(11), '-'},
       {hollerith(0, 1), '/'},
       {hollerith(8, 3), '='},
       // Was `-` in older versions, note not corrected on page
       {hollerith(8, 4), '\''},
       {hollerith(12, 8, 3), '.'},
       {hollerith(12, 8, 4), ')'},
       {hollerith(11, 8, 3), '$'},
       {hollerith(11, 8, 4), '*'},
       {hollerith(0, 8, 3), ','},
       {hollerith(0, 8, 4), '('}});
  return table;
}

// https://bitsavers.org/pdf/ibm/7090/C28-6274-1_7090_FORTRANIV.pdf
// Page 33
const std::vector<HollerithChar> &getFORTRANIVEncoding() {
  static std::vector<HollerithChar> table = createBCDEncoding({
      //
      {hollerith(8, 3), '='},
      {hollerith(8, 4), '\''},
      {hollerith(12), '+'},
      {hollerith(12, 8, 3), '.'},
      {hollerith(12, 8, 4), ')'},
      {hollerith(11), '-'},
      {hollerith(11, 8, 3), '$'},
      {hollerith(11, 8, 4), '*'},
      {hollerith(0, 1), '/'},
      {hollerith(0, 8, 3), ','},
      {hollerith(0, 8, 4), '('} //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8
// Comercial collating sequence
// See https://bitsavers.org/pdf/ibm/7090/C28-6365-1_genSort_Sep64.pdf
// Page 7
const std::vector<HollerithChar> &getBCDIC1() {
  static std::vector<HollerithChar> table = createBCDEncoding({
      //
      {hollerith(12, 3, 8), '.'},
      {hollerith(12, 4, 8), u'¤'},
      {hollerith(12, 5, 8), '['},
      {hollerith(12, 6, 8), '<'},
      // Triple dagger
      {hollerith(12, 7, 8), u'\u2E4B'},
      {hollerith(12), '&'},
      {hollerith(11, 3, 8), '$'},
      {hollerith(11, 4, 8), '*'},
      {hollerith(11, 5, 8), ']'},
      {hollerith(11, 6, 8), ';'},
      // Triangle
      {hollerith(11, 7, 8), u'\u25B3'},
      {hollerith(11), '-'},
      {hollerith(0, 1), '/'},
      {hollerith(0, 3, 8), ','},
      {hollerith(0, 4, 8), '%'},
      // Gamma-like thing
      {hollerith(0, 5, 8), u'\u0194'},
      {hollerith(0, 6, 8), '\\'},
      // Triple plus
      {hollerith(0, 7, 8), u'\u29FB'},
      // Encoding used for 0 on tape
      {hollerith(2, 8), '0'},
      {hollerith(3, 8), '#'},
      {hollerith(4, 8), '@'},
      {hollerith(5, 8), ':'},
      {hollerith(6, 8), '>'},
      // square root
      {hollerith(7, 8), u'\u221A'},
      // Equivalent to 12-2-8
      {hollerith(12, 0), '?'},
      {hollerith(11, 0), '!'},
      {hollerith(0, 2, 8), u'‡'},
      //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8
// Comercial collating sequence
// See https://bitsavers.org/pdf/ibm/7090/C28-6365-1_genSort_Sep64.pdf
// Page 7
const std::vector<HollerithChar> &getBCDIC2() {
  static std::vector<HollerithChar> table = createBCDEncoding({

      //
      {hollerith(), ' '},
      {hollerith(12, 3, 8), '.'},
      {hollerith(12, 4, 8), ')'},
      {hollerith(12, 5, 8), '['},
      {hollerith(12, 6, 8), '<'},
      // Triple dagger
      {hollerith(12, 7, 8), u'\u2E4B'},
      {hollerith(12), '+'},
      {hollerith(11, 3, 8), '$'},
      {hollerith(11, 4, 8), '*'},
      {hollerith(11, 5, 8), ']'},
      {hollerith(11, 6, 8), ';'},
      // Triangle
      {hollerith(11, 7, 8), u'\u25B3'},
      {hollerith(11), '-'},
      {hollerith(0, 1), '/'},
      {hollerith(0, 3, 8), ','},
      {hollerith(0, 4, 8), '('},
      // Gamma-like thing
      {hollerith(0, 5, 8), u'\u0194'},
      {hollerith(0, 6, 8), '\\'},
      // Triple plus
      {hollerith(0, 7, 8), u'\u29FB'},
      // Used for 0 on tape. Strange character in table
      {hollerith(2, 8), '0'},
      {hollerith(3, 8), '='},
      {hollerith(4, 8), '\''},
      {hollerith(5, 8), ':'},
      {hollerith(6, 8), '>'},
      // square root
      {hollerith(7, 8), u'\u221A'},
      // Equivalent to 12-2-8
      {hollerith(12, 0), '?'},
      {hollerith(11, 0), '!'},
      {hollerith(0, 2, 8), u'‡'},
      //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/702/22-6173-1_702prelim_Feb56.pdf
// Page 32, 80
// https://bitsavers.org/pdf/ibm/705/22-6627-4_705_Oper_Jun57.pdf
// Page 10
// https://bitsavers.org/pdf/ibm/punchedCard/AccountingMachine/22-5765-7_407_Operation_1953.pdf
// Page 8, 13
// https://bitsavers.org/pdf/ibm/650/22-6060-2_650_OperMan.pdf
// Page 109
const std::vector<HollerithChar> &getBCD702() {
  static std::vector<HollerithChar> table = createBCDEncoding({

      //
      {hollerith(12), '&'},
      {hollerith(12, 3, 8), '.'},
      {hollerith(12, 4, 8), u'¤'},
      {hollerith(11), '-'},
      {hollerith(11, 3, 8), '$'},
      {hollerith(11, 4, 8), '*'},
      {hollerith(0, 1), '/'},
      {hollerith(0, 3, 8), ','},
      {hollerith(0, 4, 8), '%'},
      {hollerith(3, 8), '#'},
      {hollerith(4, 8), '@'}});
  return table;
}
