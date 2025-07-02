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
#include "Z0ftware/bcd.hpp"

parity_bcd_t evenParity(bcd_t sixbit) {
  uint8_t result = sixbit.value();
  result ^= (result << 4);
  result ^= (result << 2);
  result ^= (result >> 1);
  return (parity_bcd_t(0x40) & result) | sixbit;
}

bool isEvenParity(parity_bcd_t sevenbit) {
  return sevenbit == evenParity(sevenbit);
}

const std::array<parity_bcd_t, 1 << 6> &getEvenParityTable() {
  static auto init = []() {
    std::array<parity_bcd_t, 1 << 6> table;
    for (int i = 0; i < 1 << 6; ++i) {
      table[i] = evenParity(bcd_t(i));
    }
    return table;
  };
  static std::array<parity_bcd_t, 1 << 6> table = init();
  return table;
}

parity_bcd_t oddParity(bcd_t sixbit) {
  sixbit &= bcd_t(0x3f);
  parity_bcd_t result = parity_bcd_t(0x40) | sixbit;
  result = result ^ (result << 4);
  result ^= result << 2;
  result ^= result >> 1;
  return sixbit | (result & parity_bcd_t(0x40));
}

const std::array<parity_bcd_t, 1 << 6> &getOddParityTable() {
  static auto init = []() {
    std::array<parity_bcd_t, 1 << 6> table;
    for (int i = 0; i < 1 << 6; ++i) {
      table[i] = oddParity(bcd_t(i));
    }
    return table;
  };
  static std::array<parity_bcd_t, 1 << 6> table = init();
  return table;
}
