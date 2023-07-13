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

// Helpers for working with bit fields

#ifndef Z0FTWARE_HOLLERITH_HPP
#define Z0FTWARE_HOLLERITH_HPP

#include <cstdint>

// Translate Hollerith row to bit position
template <typename T> constexpr std::uint64_t hbit(T pos) {
  //   12  11
  //   11  10
  // [1]0   9
  //    1   8
  //    2   7
  //    3   6
  //    4   5
  //    5   4
  //    6   3
  //    7   2
  //    8   1
  //    9   0
  //
  //                      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12
  constexpr T translate[]{9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 9, 10, 11};
  return translate[pos];
}

constexpr std::uint64_t hbits() { return 0; }

template <typename Row, typename... MoreRows>
constexpr std::uint64_t hbits(Row row, MoreRows... moreRows) {
  return (std::uint64_t(1) << hbit(row)) | hbits(moreRows...);
}

#endif
