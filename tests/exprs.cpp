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

#include "Z0ftware/exprs.hpp"
#include "Z0ftware/parser.hpp"

#include <gtest/gtest.h>

#include <string>
#include <unordered_map>
#include <vector>

class TestEnvironment : public Environment {
public:
  int getLocation() const override { return location_; }
  int get(const std::string &string) override { return map_.at(string); }

  void setLocation(int location) { location_ = location; }
  void set(const std::string &name, int value) { map_[name] = value; }

private:
  int location_{0};
  std::unordered_map<std::string, int> map_;
};

TEST(expr, values) {
  TestEnvironment env;
  env.setLocation(7);
  env.set("FOUR", 4);
  env.set("FIVE", 5);

  PegEXPParser parser;
  std::vector<Expr::ptr> exprs;
  std::string_view comment;

  ASSERT_TRUE(parser.parse("", exprs));
  ASSERT_EQ(exprs.size(), 1);
  auto value = exprs[0]->value(env);
  EXPECT_EQ(0, value);

  ASSERT_TRUE(parser.parse("5", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(5, value);

  ASSERT_TRUE(parser.parse("**", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(0, value);

  ASSERT_TRUE(parser.parse("*", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(7, value);

  ASSERT_TRUE(parser.parse("FOUR", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(4, value);

  ASSERT_TRUE(parser.parse("FOUR+FIVE", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(9, value);

  ASSERT_TRUE(parser.parse("-FOUR", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ((ldb<0, 15>(-4)), value);

  ASSERT_TRUE(parser.parse("5*FOUR", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(20, value);

  ASSERT_TRUE(parser.parse("1+5*FOUR-1", exprs));
  ASSERT_EQ(exprs.size(), 1);
  value = exprs[0]->value(env);
  EXPECT_EQ(20, value);

  ASSERT_TRUE(parser.parse("X,Y+1,A*A,V", exprs));
  ASSERT_EQ(exprs.size(), 4);
}

