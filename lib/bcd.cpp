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

#include "Z0ftware/bcd.hpp"

#include <unordered_map>

namespace {
struct BCD_ASCII {
  std::uint8_t bcd;
  char32_t ascii;
};

// clang-format off
// From page 62, Philip M. Sherman, PROGRAMMING AND CODING THE IBM 709-7090-7094 COMPUTERS, 1963.
BCD_ASCII bcd704[] = {
    {000, '0'},
    {001, '1'},
    {002, '2'},
    {003, '3'},
    {004, '4'},
    {005, '5'},
    {006, '6'},
    {007, '7'},
    {010, '8'},
    {011, '9'},
    // 012
    {013, '='},
    {014, '"'},
    // 015
    // 016
    // 017
    {020, '+'},
    {021, 'A'},
    {022, 'B'},
    {023, 'C'},
    {024, 'D'},
    {025, 'E'},
    {026, 'F'},
    {027, 'G'},
    {030, 'H'},
    {031, 'I'},
    // 032
    {033, '.'},
    {034, ')'},
    // 035
    // 036
    // 037
    {040, '-'},
    {041, 'J'},
    {042, 'K'},
    {043, 'L'},
    {044, 'M'},
    {045, 'N'},
    {046, 'O'},
    {047, 'P'},
    {050, 'Q'},
    {051, 'R'},
    // 052
    {053, '$'},
    {054, '*'},
    // 055
    // 056
    // 057
    {060, ' '},
    {061, '/'},
    {062, 'S'},
    {063, 'T'},
    {064, 'U'},
    {065, 'V'},
    {066, 'W'},
    {067, 'X'},
    {070, 'Y'},
    {071, 'Z'},
    // 072
    {073, ','},
    {074, '('}
    // 075
    // 076
    // 077
};
// clang-format on
} // namespace

bcd_t BCDFromChar(char ascii) {
  static auto buildTable = []() {
    std::unordered_map<char32_t, std::uint8_t> table;
    for (auto bcd : bcd704) {
      table[bcd.ascii] = bcd.bcd;
    }
    return table;
  };
  static auto table = buildTable();
  auto it = table.find(ascii);
  return it == table.end() ? 0x7f : it->second;
}

char charFromBCD(bcd_t bcd) {
  static auto buildTable = []() {
    std::unordered_map<std::uint8_t, char32_t> table;
    for (auto bcd : bcd704) {
      table[bcd.bcd] = bcd.ascii;
    }
    return table;
  };
  static auto table = buildTable();
  auto it = table.find(bcd);
  return it == table.end() ? 0x7f : it->second;
}

/// Convert first 6 ASCII chars to big-endian bcd chars
/// If less than 6, pad right with spaces
uint64_t bcd(std::string_view chars) {
  std::uint64_t result = BCDFromChar(chars.empty() ? ' ' : chars[0]);
  for (int i = 1; i < 6; ++i) {
    result = (result << 6) | BCDFromChar(i < chars.size() ? chars[i] : ' ');
  }
  return result;
}

std::array<std::uint8_t, bcdSize> bcdEvenParity() {
  std::array<std::uint8_t, bcdSize> table;
  for (size_t i = 0; i < bcdSize; ++i) {
    std::uint8_t val = (i ^ (i >> 4));
    val ^= (val >> 2);
    val ^= (val >> 1);
    table[i] = (val & 0x01) << 6;
  }
  return table;
}
