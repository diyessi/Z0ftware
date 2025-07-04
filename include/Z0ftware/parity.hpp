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

#ifndef Z0FTWARE_PARITY_HPP
#define Z0FTWARE_PARITY_HPP

#include "Z0ftware/word.hpp"

#include <array>
#include <cstdint>

class bcd_t;
class parity_bcd_t;

// Return with bit 6 set to give bits 0-6 even parity
parity_bcd_t evenParity(bcd_t bcd_t);
bool isEvenParity(parity_bcd_t parity_bcd_t);
const std::array<parity_bcd_t, 1 << 6> &getEvenParityTable();
// Return with bit 6 set to give bits 0-6 odd parity
parity_bcd_t oddParity(bcd_t bcd_t);
const std::array<parity_bcd_t, 1 << 6> &getOddParityTable();

#endif
