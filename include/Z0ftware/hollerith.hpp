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

/**
 * @file hollerith.hpp
 * @brief Hollerith encoding
 */

#ifndef Z0FTWARE_HOLLERITH
#define Z0FTWARE_HOLLERITH

#include "Z0ftware/field.hpp"

class hollerith_t : public UnsignedImp<hollerith_t, 12> {
public:
  using UnsignedImp::UnsignedImp;
  /**
   * @brief Convert a row number to a bit position
   *
   * Row: 12 11 10|0 1 2 3 4 5 6 7 8 9
   * Bit: 11 10  9|9 8 7 6 5 4 3 2 1 0
   */
  template <typename T>
  static inline constexpr T positionFromRow(const T &row) {
    return row < 10 ? 9 - row : row - 1;
  }

  static constexpr hollerith_t fromRows() { return hollerith_t(); }

  template <typename Row, typename... MoreRows>
  static constexpr hollerith_t fromRows(Row row, MoreRows... moreRows) {
    return hollerith_t((value_t(1) << positionFromRow(row)) |
                       fromRows(moreRows...).value());
  }

  template <typename T>
  static inline constexpr hollerith_t bitForRow(const T &row) {
    return hollerith_t(1) << positionFromRow(row);
  }

  template <typename Row> constexpr bool isSet(const Row &row) const {
    return 0 != (value() & bitForRow(row));
  }

  inline constexpr hollerith_t hollerith() { return 0; }

  template <typename Row, typename... MoreRows>
  inline constexpr hollerith_t hollerith(Row row, MoreRows... moreRows) {
    return hollerith_t::fromRows(row) | hollerith(moreRows...);
  }
};

inline constexpr hollerith_t hollerith() { return 0; }

template <typename Row, typename... MoreRows>
inline constexpr hollerith_t hollerith(Row row, MoreRows... moreRows) {
  return hollerith_t::fromRows(row) | hollerith(moreRows...);
}

namespace std {
template <> struct hash<hollerith_t> {
  std::size_t operator()(const hollerith_t &h) const { return h.value(); }
};
} // namespace std

#endif
