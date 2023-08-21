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

#include "Z0ftware/asm.hpp"
#include "Z0ftware/card.hpp"
#include "Z0ftware/operation.hpp"
#include "Z0ftware/parser.hpp"
#include "Z0ftware/signature.hpp"

#include <gtest/gtest.h>

TEST(sap, REM) {
  SAPAssembler parser;

  InputTextCard card(R"(       REM  This is a comment)");
  auto operation = parser.parseCard(card);
  ASSERT_TRUE(operation->on([](Rem &rem) {}));
  EXPECT_EQ(operation->getComment(), "This is a comment");
}

TEST(sap, BCD) {
  SAPAssembler sap;
  {
    InputTextCard card("       BCD 1ABCDEF");
    auto operation = sap.parseCard(card);
    Bcd *bcd{nullptr};
    ASSERT_TRUE(operation->on([&bcd](Bcd &ref) { bcd = &ref; }));
    auto &values = bcd->getValues();
    ASSERT_EQ(values.size(), 1);
    EXPECT_EQ(values.at(0), FixPoint(false, 0212223242526));
  }
  {
    InputTextCard card("       BCD  0123456789=\"");
    auto operation = sap.parseCard(card);
    Bcd *bcd{nullptr};
    ASSERT_TRUE(operation->on([&bcd](Bcd &ref) { bcd = &ref; }));
    auto &values = bcd->getValues();
    ASSERT_EQ(values.size(), 10);
    EXPECT_EQ(values.at(0), FixPoint(false, 0000102030405));
    EXPECT_EQ(values.at(1), FixPoint(false, 0060710111314));
  }
}

TEST(sap, parseOCT) {
  SAPAssembler sap;
  {
    InputTextCard card("       OCT 1,23,-456,+700");
    auto operation = sap.parseCard(card);
    Oct *oct{nullptr};
    ASSERT_TRUE(operation->on([&oct](Oct &ref) { oct = &ref; }));
    const auto &values = oct->getValues();
    ASSERT_EQ(values.size(), 4);
    EXPECT_EQ(values.at(0), FixPoint(false, 01));
    EXPECT_EQ(values.at(1), FixPoint(false, 023));
    EXPECT_EQ(values.at(2), FixPoint(true, 0456));
    EXPECT_EQ(values.at(3), FixPoint(false, 0700));
  }
  {
    InputTextCard card("       OCT 1,23 VALUES");
    auto operation = sap.parseCard(card);
    Oct *oct{nullptr};
    ASSERT_TRUE(operation->on([&oct](Oct &ref) { oct = &ref; }));
    const auto &values = oct->getValues();
    ASSERT_EQ(values.size(), 2);
    EXPECT_EQ(values.at(0), FixPoint(false, 01));
    EXPECT_EQ(values.at(1), FixPoint(false, 023));
    EXPECT_EQ(operation->getComment(), "VALUES");
  }
  {
    InputTextCard card("       OCT --1,23 VALUES");
    auto operation = sap.parseCard(card);
    Oct *oct{nullptr};
    ASSERT_TRUE(operation->on([&oct](Oct &ref) { oct = &ref; }));
    ASSERT_TRUE(operation->hasErrors());
  }
  {
    InputTextCard card("       OCT +-1,23 VALUES");
    auto operation = sap.parseCard(card);
    Oct *oct{nullptr};
    ASSERT_TRUE(operation->on([&oct](Oct &ref) { oct = &ref; }));
    ASSERT_TRUE(operation->hasErrors());
  }
}

TEST(sap, DEC) {
  SAPAssembler sap;
  InputTextCard card("       DEC 210,2B2,1B2,146B8,.5B34,0.75E0,-0,-0.0 COMMENT");
  auto operation = sap.parseCard(card);
  Dec *dec{nullptr};
  ASSERT_TRUE(operation->on([&dec](Dec &ref) { dec = &ref; }));
  const auto &values = dec->getValues();
  std::string_view variable = "210,2B2,1B2,146B8,.5B34,0.75E0,-0,-0.0 COMMENT";
  ASSERT_EQ(values.size(), 8);
  EXPECT_EQ(values.at(0), FixPoint(false, 210));
  EXPECT_EQ(values.at(1), FixPoint(false, 0200000000000));
  EXPECT_EQ(values.at(2), FixPoint(false, 0100000000000));
  EXPECT_EQ(values.at(3), FixPoint(false, 0222000000000));
  EXPECT_EQ(values.at(4), FixPoint(false, 01));
  EXPECT_EQ(values.at(5), FixPoint(false, 0200600000000));
  EXPECT_EQ(values.at(6), FixPoint(true, 0));
  EXPECT_EQ(values.at(7), FixPoint(true, 0));
  EXPECT_EQ(operation->getComment(), "COMMENT");
}

TEST(sap, equ) {
  SAPAssembler sap;
  {
    auto operation = sap.parseCard(InputTextCard("A      EQU 1"));
    ASSERT_TRUE(operation->on([](Equ &ref) {}));
  }
  {
    auto operation = sap.parseCard(InputTextCard("       EQU 1"));
    operation->validate(sap);
    ASSERT_TRUE(operation->on([](Equ &ref) {}));
    ASSERT_TRUE(operation->hasErrors());
  }
}
