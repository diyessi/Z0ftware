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

#include "Z0ftware/card.hpp"

#include <gtest/gtest.h>

TEST(cards, readWords) {
  CardImage cardImage;
  for (int i = 0; i < 80; i++) {
    cardImage[i + 1] = i | ((i + 1) << 7);
  }
  for (int i = 0; i < 80; i++) {
    EXPECT_EQ(cardImage[i + 1], i | ((i + 1) << 7));
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
