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
#include "Z0ftware/charset.hpp"
#include "Z0ftware/convert.hpp"
#include "Z0ftware/hollerith.hpp"

#include <gtest/gtest.h>

TEST(characters, hollerith) {
  for (auto &ctp : collateGlyphCardTape.getItems()) {
    EXPECT_EQ(convert<tape_bcd_t>(ctp.hc), ctp.sc) << ctp.collate;
  }
}

TEST(characters, tape_bcd) {
    std::unique_ptr<parity_glyphs_t> glyphs(collateGlyphCardTape.getTapeCharset(false));
  for (auto &ctp : collateGlyphCardTape.getItems()) {
    EXPECT_EQ(glyphs->at(evenParity(ctp.sc).value()), ctp.glyphs[0].getUtf8Char()) << ctp.collate;
  }
}
