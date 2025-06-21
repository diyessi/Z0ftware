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

#include "Z0ftware/characters.hpp"

#include <gtest/gtest.h>

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8

// Also https://archive.org/details/mackenzie-coded-char-sets/page/24/mode/2up
// Page 25
struct CardTapePair {
  std::uint8_t collate;
  HollerithChar hc;
  SerialChar sc;
};

CardTapePair cardTapePairs[] = {
    //
    {0, {}, 0x10},
    //
    {1, {12, 3, 8}, 0x3B},
    {2, {12, 4, 8}, 0x3C},
    {3, {12, 5, 8}, 0x3D},
    {4, {12, 6, 8}, 0x3E},
    {5, {12, 7, 8}, 0x3F},
    {6, {12}, 0x30},
    {7, {11, 3, 8}, 0x2B},
    {8, {11, 4, 8}, 0x2C},
    {9, {11, 5, 8}, 0x2D},
    {10, {11, 6, 8}, 0x2E},
    {11, {11, 7, 8}, 0x2F},
    {12, {11}, 0x20},
    {13, {0, 1}, 0x11},
    {14, {0, 3, 8}, 0x1B},
    {15, {0, 4, 8}, 0x1C},
    {16, {0, 5, 8}, 0x1D},
    {17, {0, 6, 8}, 0x1E},
    {18, {0, 7, 8}, {0x1F}},
    // Replaced by blank on tape
    //{19, {2, 8}, 0x10},
    {20, {3, 8}, 0x0B},
    {21, {4, 8}, 0x0C},
    {22, {5, 8}, 0x0D},
    {23, {6, 8}, 0x0E},
    {24, {7, 8}, 0x0F},
    //
    {25, {12, 0}, 0x3A},
    {26, {12, 1}, 0x31},
    {27, {12, 2}, 0x32},
    {28, {12, 3}, 0x33},
    {29, {12, 4}, 0x34},
    {30, {12, 5}, 0x35},
    {31, {12, 6}, 0x36},
    {32, {12, 7}, 0x37},
    {33, {12, 8}, 0x38},
    {34, {12, 9}, 0x39},
    //
    {35, {11, 0}, 0x2A},
    {36, {11, 1}, 0x21},
    {37, {11, 2}, 0x22},
    {38, {11, 3}, 0x23},
    {39, {11, 4}, 0x24},
    {40, {11, 5}, 0x25},
    {41, {11, 6}, 0x26},
    {42, {11, 7}, 0x27},
    {43, {11, 8}, 0x28},
    {44, {11, 9}, 0x29},
    {45, {0, 2, 8}, 0x1A},
    {46, {0, 2}, 0x12},
    {47, {0, 3}, 0x13},
    {48, {0, 4}, 0x14},
    {49, {0, 5}, 0x15},
    {50, {0, 6}, 0x16},
    {51, {0, 7}, 0x17},
    {52, {0, 8}, 0x18},
    {53, {0, 9}, 0x19},
    {54, {0}, 0x0A},
    {55, {1}, 0x01},
    {56, {2}, 0x02},
    {57, {3}, 0x03},
    {58, {4}, 0x04},
    {59, {5}, 0x05},
    {60, {6}, 0x06},
    {61, {7}, 0x07},
    {62, {8}, 0x08},
    {63, {9}, 0x09}};

TEST(characters, hollerith) {
  for (auto &ctp : cardTapePairs) {
    EXPECT_EQ(SerialChar(ctp.hc), ctp.sc) << int(ctp.collate);
  }
}