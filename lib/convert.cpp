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

#include "Z0ftware/convert.hpp"

#include <array>

namespace {
struct CardTapePair {
  size_t collate;
  hollerith_t hc;
  tape_bcd_t::value_t sc;
};

CardTapePair cardTapePairs[] = {
    //
    {0, hollerith(), 0x10},
    //
    {1, hollerith(12, 3, 8), 0x3B},
    {2, hollerith(12, 4, 8), 0x3C},
    {3, hollerith(12, 5, 8), 0x3D},
    {4, hollerith(12, 6, 8), 0x3E},
    {5, hollerith(12, 7, 8), 0x3F},
    {6, hollerith(12), 0x30},
    {7, hollerith(11, 3, 8), 0x2B},
    {8, hollerith(11, 4, 8), 0x2C},
    {9, hollerith(11, 5, 8), 0x2D},
    {10, hollerith(11, 6, 8), 0x2E},
    {11, hollerith(11, 7, 8), 0x2F},
    {12, hollerith(11), 0x20},
    {13, hollerith(0, 1), 0x11},
    {14, hollerith(0, 3, 8), 0x1B},
    {15, hollerith(0, 4, 8), 0x1C},
    {16, hollerith(0, 5, 8), 0x1D},
    {17, hollerith(0, 6, 8), 0x1E},
    {18, hollerith(0, 7, 8), 0x1F},
    // Replaced by blank on tape
    {19, hollerith(2, 8), 0x00},
    {20, hollerith(3, 8), 0x0B},
    {21, hollerith(4, 8), 0x0C},
    {22, hollerith(5, 8), 0x0D},
    {23, hollerith(6, 8), 0x0E},
    {24, hollerith(7, 8), 0x0F},
    //
    {25, hollerith(12, 0), 0x3A},
    {26, hollerith(12, 1), 0x31},
    {27, hollerith(12, 2), 0x32},
    {28, hollerith(12, 3), 0x33},
    {29, hollerith(12, 4), 0x34},
    {30, hollerith(12, 5), 0x35},
    {31, hollerith(12, 6), 0x36},
    {32, hollerith(12, 7), 0x37},
    {33, hollerith(12, 8), 0x38},
    {34, hollerith(12, 9), 0x39},
    //
    {35, hollerith(11, 0), 0x2A},
    {36, hollerith(11, 1), 0x21},
    {37, hollerith(11, 2), 0x22},
    {38, hollerith(11, 3), 0x23},
    {39, hollerith(11, 4), 0x24},
    {40, hollerith(11, 5), 0x25},
    {41, hollerith(11, 6), 0x26},
    {42, hollerith(11, 7), 0x27},
    {43, hollerith(11, 8), 0x28},
    {44, hollerith(11, 9), 0x29},
    {45, hollerith(0, 2, 8), 0x1A},
    {46, hollerith(0, 2), 0x12},
    {47, hollerith(0, 3), 0x13},
    {48, hollerith(0, 4), 0x14},
    {49, hollerith(0, 5), 0x15},
    {50, hollerith(0, 6), 0x16},
    {51, hollerith(0, 7), 0x17},
    {52, hollerith(0, 8), 0x18},
    {53, hollerith(0, 9), 0x19},
    {54, hollerith(0), 0x0A},
    {55, hollerith(1), 0x01},
    {56, hollerith(2), 0x02},
    {57, hollerith(3), 0x03},
    {58, hollerith(4), 0x04},
    {59, hollerith(5), 0x05},
    {60, hollerith(6), 0x06},
    {61, hollerith(7), 0x07},
    {62, hollerith(8), 0x08},
    {63, hollerith(9), 0x09}};

using hollerith_from_tape_bcd_t =
    std::array<hollerith_t, 1 << tape_bcd_t::bit_size()>;

static hollerith_from_tape_bcd_t create_hollerith_from_tape() {
  hollerith_from_tape_bcd_t array;
  array.fill(hollerith());
  for (auto &pair : cardTapePairs) {
    array[pair.sc] = pair.hc.value();
  }
  return array;
}

static const hollerith_from_tape_bcd_t &get_hollerith_from_tape() {
  static std::unique_ptr<hollerith_from_tape_bcd_t> array =
      std::make_unique<hollerith_from_tape_bcd_t>(create_hollerith_from_tape());
  return *array;
}

using tape_bcd_from_hollerith_t =
    std::array<tape_bcd_t, 1 << hollerith_t::bit_size()>;

static tape_bcd_t tape_bcd_from_hollerith(hollerith_t h) {
  if (h == hollerith()) {
    // blank
    return tape_bcd_t(0x10);
  } else if (h == hollerith(0)) {
    return tape_bcd_t(0x0A);
  } else {
    tape_bcd_t::value_t value = 0;
    int start_digit = 10;
    for (int zone = 12; zone >= 10; zone--) {
      if (h.isSet(zone)) {
        value |= (zone - 9) * 0x10;
        start_digit = zone - 1;
        break;
      }
    }
    for (int digit = start_digit; digit >= 1; digit--) {
      if (h.isSet(digit)) {
        value |= digit;
      }
    }
    if (0x0A == value) {
      value = 0x10;
    }
    return value;
  }
}

static tape_bcd_from_hollerith_t create_tape_from_hollerith() {
  tape_bcd_from_hollerith_t array;
  for (hollerith_t::value_t h = 0; h < 1 << hollerith_t::bit_size(); ++h) {
    array[h] = tape_bcd_from_hollerith(hollerith_t(h));
  }
  return array;
}

static const tape_bcd_from_hollerith_t &get_tape_from_hollerith() {
  static std::unique_ptr<tape_bcd_from_hollerith_t> array =
      std::make_unique<tape_bcd_from_hollerith_t>(create_tape_from_hollerith());
  return *array;
}
} // namespace
/**
 * @file convert.cpp
 * @brief Convert between encodings
 */

template <> cpu704_bcd_t convert(tape_bcd_t tape) {
  auto bcd = tape.value();
  bcd_t::value_t zone_swap = (0x10 == (bcd & 0x10)) ? (bcd ^ 0x20) : bcd;
  switch (zone_swap) {
  case 0x0A:
    return 0x00;
  default:
    return zone_swap;
  }
}

template <> hollerith_t convert(tape_bcd_t tape_bcd) {
  static auto &conversion_array = get_hollerith_from_tape();
  return conversion_array[tape_bcd.value()];
}

template <> tape_bcd_t convert(cpu704_bcd_t cpu) {
  auto bcd = cpu.value();
  bcd_t::value_t zone_swap = (0x10 == (bcd & 0x10)) ? (bcd ^ 0x20) : bcd;
  switch (zone_swap) {
  case 0x00:
    return 0x0A;
  default:
    return zone_swap;
  }
}

template <> hollerith_t convert(cpu704_bcd_t cpu) {
  return convert<hollerith_t>(convert<tape_bcd_t>(cpu));
}

template <> tape_bcd_t convert(hollerith_t hollerith) {
  static auto &array = get_tape_from_hollerith();
  return array[hollerith.value()];
#if 0
  if (column == hollerith_t::hollerith()) {
    // blank
    value_ = 0x10;
  } else if (column == hollerith_t::hollerith(0)) {
    value_ = 0x0A;
  } else {
    int start_digit = 10;
    for (int zone = 12; zone >= 10; zone--) {
      if (column.isSet(zone)) {
        value_ |= (zone - 9) * 0x10;
        start_digit = zone - 1;
        break;
      }
    }
    for (int digit = start_digit; digit >= 1; digit--) {
      if (column.isSet(digit)) {
        value_ |= digit;
      }
    }
  }
#endif
}

template <> cpu704_bcd_t convert(hollerith_t hollerith) {
  return convert<cpu704_bcd_t>(convert<tape_bcd_t>(hollerith));
#if 0
  if (column == hollerith()) {
    // blank
    value_ = 0x30;
  } else if (column == hollerith(0)) {
    value_ = 0x00;
  } else {
    int start_digit = 10;
    for (int zone = 12; zone >= 10; zone--) {
      if (column.isSet(zone)) {
        value_ |= (13 - zone) * 0x10;
        start_digit = zone - 1;
        break;
      }
    }
    for (int digit = start_digit; digit >= 1; digit--) {
      if (column.isSet(digit)) {
        value_ |= digit;
      }
    }
  }
#endif
}
