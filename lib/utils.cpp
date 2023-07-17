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

#include "Z0ftware/utils.hpp"

std::string_view trim(std::string_view text) {
  auto pos = text.find_first_not_of(" ");
  if (pos == std::string_view::npos) {
    text.remove_suffix(text.size());
  } else {
    text.remove_prefix(pos);
    text.remove_suffix(text.size() - text.find_last_not_of(" ") - 1);
  }
  return text;
}

std::string_view rightTrim(std::string_view text) {
  auto pos = text.find_last_not_of(" ");
  if (pos == std::string_view::npos) {
    text.remove_suffix(text.size());
  } else {
    text.remove_suffix(text.size() - pos - 1);
  }
  return text;
}
