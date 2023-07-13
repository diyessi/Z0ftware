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

#include "Z0ftware/parity.hpp"

sevenbit_t evenParity(sixbit_t sixbit) {
  sixbit &= sixbit_t(0x3f);
  sevenbit_t result = sixbit ^ (sixbit << 4);
  result ^= result << 2;
  result ^= result >> 1;
  return sixbit | (result & sevenbit_t(0x40));
}

const std::array<sevenbit_t, 1 << 6> &getEvenParityTable() {
  static auto init = []() {
    std::array<sevenbit_t, 1 << 6> table;
    for (int i = 0; i < 1 << 6; ++i) {
      table[i] = evenParity(sixbit_t(i));
    }
    return table;
  };
  static std::array<sevenbit_t, 1 << 6> table = init();
  return table;
}

sevenbit_t oddParity(sixbit_t sixbit) {
  sixbit &= sixbit_t(0x3f);
  sevenbit_t result = sevenbit_t(0x40) | sixbit;
  result = result ^ (result << 4);
  result ^= result << 2;
  result ^= result >> 1;
  return sixbit | (result & sevenbit_t(0x40));
}

const std::array<sevenbit_t, 1 << 6> &getOddParityTable() {
  static auto init = []() {
    std::array<sevenbit_t, 1 << 6> table;
    for (int i = 0; i < 1 << 6; ++i) {
      table[i] = oddParity(sixbit_t(i));
    }
    return table;
  };
  static std::array<sevenbit_t, 1 << 6> table = init();
  return table;
}
