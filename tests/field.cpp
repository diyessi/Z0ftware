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

#include "Z0ftware/field.hpp"

#include <gtest/gtest.h>

TEST(BitField, ldb) {
  std::uint64_t value = 0xfedcba9876543210;
  EXPECT_EQ((ldb<4, 4>(value)), 1);
  EXPECT_EQ((ldb<3, 4>(value)), 2);
  EXPECT_EQ((ldb<4, 8>(value)), 0x21);
  EXPECT_EQ((ldb<63, 1>(value)), 1);
  EXPECT_EQ((ldb<62, 2>(value)), 3);
  EXPECT_EQ((ldb<61, 3>(value)), 7);
  EXPECT_EQ((ldb<60, 4>(value)), 0xF);
  EXPECT_EQ((ldb<59, 5>(value)), 0x1F);
}

TEST(BitField, dpb) {
  std::uint64_t value = 0xFFFFFFFFFFFFFFFF;
  dpb<4, 4>(3, value);
  EXPECT_EQ(value, 0xFFFFFFFFFFFFFF3F);
  dpb<8, 4>(0, value);
  EXPECT_EQ(value, 0xFFFFFFFFFFFFF03F);
  dpb<8, 4>(-1, value);
  EXPECT_EQ(value, 0xFFFFFFFFFFFFFF3F);
}

TEST(BitFieldRef, ldb) {
  std::uint64_t value = 0xfedcba9876543210;
  std::uint64_t value1 = value;
  using s31 = BitField<28, 4>;
  EXPECT_EQ(s31::ref(value), 0x7);
  s31::ref(value) = 0xC;
  // Ref<s31>::ref(value) = 0xC;
  EXPECT_EQ((BitField<32, 32>::ref(value)), (BitField<32, 32>::ref(value1)));
  EXPECT_EQ((BitField<0, 28>::ref(value)), (BitField<0, 28>::ref(value1)));
  EXPECT_EQ(s31::ref(value), 0xC);
}
