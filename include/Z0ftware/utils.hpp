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

#ifndef Z0FTWARE_UTILS_HPP
#define Z0FTWARE_UTILS_HPP

#include <functional>
#include <sstream>
#include <string>

std::string_view trim(std::string_view text);
std::string_view rightTrim(std::string_view text);

// Formats a string and calls a handler with the string upon destruction.
class MessageGenerator {
public:
  using handler_type = std::function<void(std::string &&)>;
  MessageGenerator(handler_type handler) : handler_(handler) {}
  ~MessageGenerator() { handler_(os_.str()); }

  template <typename Value> MessageGenerator &operator<<(Value value) {
    os_ << value;
    return *this;
  }

private:
  std::ostringstream os_;
  handler_type handler_;
};

#endif
