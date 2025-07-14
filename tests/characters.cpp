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

#include "Z0ftware/bcd.hpp"

#include <gtest/gtest.h>

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8

// Also https://archive.org/details/mackenzie-coded-char-sets/page/24/mode/2up
// Page 25
struct CardTapePair {
  size_t collate;
  card_column_t hc;
  tape_bcd_t sc;
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
    {18, hollerith(0, 7, 8), {0x1F}},
    // Replaced by blank on tape
    //{19, {2, 8}, 0x10},
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

TEST(characters, hollerith) {
  for (auto &ctp : cardTapePairs) {
    EXPECT_EQ(tape_bcd_t(ctp.hc), ctp.sc) << ctp.collate;
  }
}