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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/card.hpp"
#include "Z0ftware/convert.hpp"

#include <gtest/gtest.h>

TEST(cards, bcd) {
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith()), cpu704_bcd_t(0b110000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0)), cpu704_bcd_t(0b000000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(1)), cpu704_bcd_t(0b000001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(2)), cpu704_bcd_t(0b000010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(3)), cpu704_bcd_t(0b000011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(4)), cpu704_bcd_t(0b000100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(5)), cpu704_bcd_t(0b000101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(6)), cpu704_bcd_t(0b000110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(7)), cpu704_bcd_t(0b000111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8)), cpu704_bcd_t(0b001000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(9)), cpu704_bcd_t(0b001001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 2)), cpu704_bcd_t(0b110000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 3)), cpu704_bcd_t(0b001011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 4)), cpu704_bcd_t(0b001100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 5)), cpu704_bcd_t(0b001101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 6)), cpu704_bcd_t(0b001110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(8, 7)), cpu704_bcd_t(0b001111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12)), cpu704_bcd_t(0b010000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 1)), cpu704_bcd_t(0b010001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 2)), cpu704_bcd_t(0b010010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 3)), cpu704_bcd_t(0b010011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 4)), cpu704_bcd_t(0b010100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 5)), cpu704_bcd_t(0b010101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 6)), cpu704_bcd_t(0b010110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 7)), cpu704_bcd_t(0b010111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8)), cpu704_bcd_t(0b011000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 9)), cpu704_bcd_t(0b011001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 2)), cpu704_bcd_t(0b011010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 0)), cpu704_bcd_t(0b011010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 3)), cpu704_bcd_t(0b011011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 4)), cpu704_bcd_t(0b011100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 5)), cpu704_bcd_t(0b011101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 6)), cpu704_bcd_t(0b011110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(12, 8, 7)), cpu704_bcd_t(0b011111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11)), cpu704_bcd_t(0b100000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 1)), cpu704_bcd_t(0b100001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 2)), cpu704_bcd_t(0b100010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 3)), cpu704_bcd_t(0b100011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 4)), cpu704_bcd_t(0b100100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 5)), cpu704_bcd_t(0b100101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 6)), cpu704_bcd_t(0b100110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 7)), cpu704_bcd_t(0b100111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8)), cpu704_bcd_t(0b101000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 9)), cpu704_bcd_t(0b101001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 2)), cpu704_bcd_t(0b101010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 0)), cpu704_bcd_t(0b101010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 3)), cpu704_bcd_t(0b101011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 4)), cpu704_bcd_t(0b101100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 5)), cpu704_bcd_t(0b101101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 6)), cpu704_bcd_t(0b101110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(11, 8, 7)), cpu704_bcd_t(0b101111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 1)), cpu704_bcd_t(0b110001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 2)), cpu704_bcd_t(0b110010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 3)), cpu704_bcd_t(0b110011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 4)), cpu704_bcd_t(0b110100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 5)), cpu704_bcd_t(0b110101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 6)), cpu704_bcd_t(0b110110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 7)), cpu704_bcd_t(0b110111));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8)), cpu704_bcd_t(0b111000));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 9)), cpu704_bcd_t(0b111001));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 2)), cpu704_bcd_t(0b111010));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 3)), cpu704_bcd_t(0b111011));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 4)), cpu704_bcd_t(0b111100));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 5)), cpu704_bcd_t(0b111101));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 6)), cpu704_bcd_t(0b111110));
  EXPECT_EQ(convert<cpu704_bcd_t>(hollerith(0, 8, 7)), cpu704_bcd_t(0b111111));
}

TEST(cards, readWords) {
  CardImage cardImage;
  for (int i = 0; i < 80; i++) {
    cardImage[i + 1] = i | ((i + 1) << 7);
  }
  for (int i = 0; i < 80; i++) {
    EXPECT_EQ(cardImage[i + 1], hollerith_t(i | ((i + 1) << 7)))
        << "Column " << i;
  }
  EXPECT_EQ(cardImage.getWord(0), 0x555555555);
  EXPECT_EQ(cardImage.getWord(1), 0x555555555);
  EXPECT_EQ(cardImage.getWord(2), 0x333333333);
  EXPECT_EQ(cardImage.getWord(3), 0x333333333);
  EXPECT_EQ(cardImage.getWord(4), 0x0F0F0F0F0);
  EXPECT_EQ(cardImage.getWord(5), 0xF0F0F0F0F);
  EXPECT_EQ(cardImage.getWord(6), 0x00FF00FF0);
  EXPECT_EQ(cardImage.getWord(7), 0x0FF00FF00);
  EXPECT_EQ(cardImage.getWord(8), 0x0000FFFF0);
  EXPECT_EQ(cardImage.getWord(9), 0x000FFFF00);
  EXPECT_EQ(cardImage.getWord(10), 0x00000000F);
  EXPECT_EQ(cardImage.getWord(11), 0xFFFFFFF00);
  EXPECT_EQ(cardImage.getWord(12), 0x000000000);
  EXPECT_EQ(cardImage.getWord(13), 0x0000000FF);
  EXPECT_EQ(cardImage.getWord(14), 0xAAAAAAAAA);
  EXPECT_EQ(cardImage.getWord(15), 0xAAAAAAAAA);
  EXPECT_EQ(cardImage.getWord(16), 0x666666666);
  EXPECT_EQ(cardImage.getWord(17), 0x666666666);
  EXPECT_EQ(cardImage.getWord(18), 0x1E1E1E1E1);
  EXPECT_EQ(cardImage.getWord(19), 0xE1E1E1E1E);
  EXPECT_EQ(cardImage.getWord(20), 0x01FE01FE0);
  EXPECT_EQ(cardImage.getWord(21), 0x1FE01FE01);
  EXPECT_EQ(cardImage.getWord(22), 0x0001FFFE0);
  EXPECT_EQ(cardImage.getWord(23), 0x001FFFE00);
}

TEST(cards, writeWords) {
  CardImage cardImage;
  for (int position = 0; position < 24; position++) {
    cardImage.setWord(position, position * position);
  }
  for (int position = 0; position < 24; position++) {
    EXPECT_EQ(cardImage.getWord(position), position * position);
  }
}