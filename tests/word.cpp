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

#include "Z0ftware/word.hpp"

#include <gtest/gtest.h>

TEST(Word, ac_add) {
  AC ac(false, 4);
  Word val(false, 3);
  auto sum = ac + val;
  EXPECT_FALSE(sum.isNegative());
  EXPECT_EQ(sum.getMagnitude(), 7);
  EXPECT_EQ(std::uint64_t(sum), 7);
}

TEST(Word, ac_sub) {
  AC ac(false, 4);
  Word val(false, 3);
  auto sum = ac - val;
  EXPECT_FALSE(sum.isNegative());
  EXPECT_EQ(sum.getMagnitude(), 1);
  EXPECT_EQ(word_t(sum), 1);
}

TEST(Word, acmq_mul) {
  ACMQ acmq(false, 4);
  Word val(true, 3);
  auto prod = acmq * val;
  EXPECT_TRUE(prod.isNegative());
  EXPECT_EQ(prod.getMagnitude(), 12);
  EXPECT_EQ(__uint128_t(prod), -12);
}
