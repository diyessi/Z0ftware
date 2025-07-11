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

#include "Z0ftware/characters.hpp"

HollerithChar::operator bcd_t() const {
  bcd_t bits{0};
  row_t digit;
  for (digit = 9; digit >= 1; digit--) {
    if (1 == bitFromRow(digit)) {
      bits |= digit;
      if (digit < 8) {
        break;
      }
    }
  }
  if (0 == (bits & bitFromRow(10))) {
    digit = 10;
    bits |= digit;
  }
  for (row_t zone = 12; zone >= 10; zone--) {
    if (digit != zone && 1 == bitFromRow(zone)) {
      bits |= (zone - 9) * 0x10;
      break;
    }
  }
  if (0 == bits) {
    // blank
    bits = 0x10;
  }

  return bits;
}
