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
 * @file convert.hpp
 * @brief Convert between encodings
 */

#ifndef Z0FTWARE_CONVERT
#define Z0FTWARE_CONVERT

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/hollerith.hpp"

// From tape
template <typename OUT> OUT convert(tape_bcd_t tape_bcd);

template <> inline constexpr tape_bcd_t convert(tape_bcd_t tape_bcd) {
  return tape_bcd;
}

template <> cpu704_bcd_t convert(tape_bcd_t tape_bcd);

template <> hollerith_t convert(tape_bcd_t tape_bcd);

// From CPU
template <typename OUT> OUT convert(cpu704_bcd_t cpu704_bcd);

template <> tape_bcd_t convert(cpu704_bcd_t cpu704_bcd);

template <> inline cpu704_bcd_t convert(cpu704_bcd_t cpu704_bcd) {
  return cpu704_bcd;
}

template <> hollerith_t convert(cpu704_bcd_t cpu704_bcd);

// From hollerith
template <typename OUT> OUT convert(hollerith_t hollerith);

template <> tape_bcd_t convert(hollerith_t hollerith);

template <> cpu704_bcd_t convert(hollerith_t hollerith);

template <> inline hollerith_t convert(hollerith_t hollerith) {
  return hollerith;
}

#endif
