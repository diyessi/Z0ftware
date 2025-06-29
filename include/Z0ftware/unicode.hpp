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

/**
 * @file unicode.hpp
 * @brief Helpers for conversion between char32_t and utf-8 strings.
 */

#ifndef Z0FTWARE_UNICODE
#define Z0FTWARE_UNICODE

#include <sstream>
#include <string>
#include <string_view>

using utf8_t = std::string; //!< Single utf8 character or string
using utf8_string_view_t = std::string_view;

using unicode_char_t = char32_t;
using unicode_string_t = std::u32string;
using unicode_string_view_t = std::u32string_view;

// Characters not in ASCII
static const utf8_t utf8_blank{"␢"};
static const utf8_t utf8_circle_dot{"⊙"};
static const utf8_t utf8_delta{"Δ"};
static const utf8_t utf8_gamma{"γ"};
static const utf8_t utf8_group_mark{"⯒"};
static const utf8_t utf8_lozenge{"⌑"};
static const utf8_t utf8_minus_zero{"⦵"};
static const utf8_t utf8_plus_minus{"±"};
static const utf8_t utf8_plus_zero{"⨁"};
static const utf8_t utf8_radical{"√"};
static const utf8_t utf8_record_mark{"⧧"};
static const utf8_t utf8_triple_plus{"⧻"};

/**
 * @brief Value indicating invalid utf-8
 */
static constexpr unicode_char_t unicode_char_invalid{0xFFFD};

/**
 * @brief Removes the next unicode char from the prefix of the utf-8 string
 * view.
 * @arg sv A utf-8 string view.
 * @returns The unicode char32_t. Unicode::invalid is returned if utf-8 is
 * invalid.
 */
static unicode_char_t get_next_unicode_char(utf8_string_view_t &sv) {
  if (sv.empty()) {
    return unicode_char_invalid;
  }
  unicode_char_t c0 = sv.at(0);
  sv.remove_prefix(1);

  if (0 == (c0 & 0x80)) {
    return c0;
  } else if (0xC0 != (c0 & 0xC0)) {
    // High two bits must be 1
    return unicode_char_invalid;
  }
  if (sv.empty()) {
    return unicode_char_invalid;
  }
  unicode_char_t c1 = sv.at(0);
  sv.remove_prefix(1);
  if (0x80 != (c1 & 0xC0)) {
    // Missing 10
    return unicode_char_invalid;
  }
  if (0xC0 == (c0 & 0xE0)) {
    return ((c0 & 0x1F) << 6) | (c1 & 0x3F);
  }
  if (sv.empty()) {
    return unicode_char_invalid;
  }
  unicode_char_t c2 = sv.at(0);
  sv.remove_prefix(1);
  if (0x80 != (c2 & 0xC0)) {
    // Missing prefix
    return unicode_char_invalid;
  }
  if (0xE0 == (c0 & 0xF0)) {
    return ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
  }
  unicode_char_t c3 = sv.at(0);
  sv.remove_prefix(1);
  if (0x80 != (c2 & 0xC0)) {
    // Missing prefix
    return unicode_char_invalid;
  }
  if (0xF0 == (c0 & 0xF8)) {
    return ((c0 & 0x7) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) |
           (c3 & 0x3F);
  }
  return unicode_char_invalid;
}

static unicode_char_t get_unicode_char(unicode_char_t c) { return c; }

/**
 * @brief Unicode char32_t of the first utf-8 char prefix.
 * @arg s A utf-8 string.
 * @returns The Unicode character. Unicode::invalid if the utf-8 char is
 * invalid.
 */
template <typename T> static unicode_char_t get_unicode_char(T s) {
  utf8_string_view_t sv(s);
  if (sv.empty()) {
    return unicode_char_invalid;
  }
  unicode_char_t c = get_next_unicode_char(sv);
  return sv.empty() ? c : unicode_char_invalid;
}

/**
 * @brief Provides stream wrapper for unicode output as utf8
 */
class Unicode {

public:
  explicit Unicode(unicode_char_t unicode) : unicode_(unicode) {}

  template <typename T> Unicode(T utf8) : unicode_(get_unicode_char(utf8)) {}

  /**
   * @returns The untagged char32_t value.
   */
  operator unicode_char_t() const { return unicode_; }

public:
  /**
   * @brief Output utf-8 for a tagged Unicode char.
   */
  template <typename S>
  friend inline auto &operator<<(S &os, const Unicode &unicode) {
    static constexpr char32_t uni_1{0x80};
    static constexpr char32_t uni_2{0x800};
    static constexpr char32_t uni_3{0x10000};
    static constexpr char32_t uni_4{0x110000};

    if (unicode_char_invalid == unicode.unicode_) {
      return os << 'x';
    } else if (unicode.unicode_ < uni_1) {
      return os << char(unicode.unicode_);
    } else if (unicode.unicode_ < uni_2) {
      return os << char((unicode.unicode_ >> 6) | 0xC0)
                << char((unicode.unicode_ & 0x3F) | 0x80);
    } else if (unicode.unicode_ < uni_3) {
      return os << char((unicode.unicode_ >> 12) | 0xE0)
                << char(((unicode.unicode_ >> 6) & 0x3F) | 0x80)
                << char((unicode.unicode_ & 0x3F) | 0x80);
    } else if (unicode.unicode_ < uni_4) {
      return os << char(((unicode.unicode_ >> 18) & 0x7) | 0xF0)
                << char(((unicode.unicode_ >> 12) & 0x3F) | 0x80)
                << char(((unicode.unicode_ >> 6) & 0x3F) | 0x80)
                << char((unicode.unicode_ & 0x3F) | 0x80);
    }
    return os << 'x';
  }

  bool operator==(const Unicode &) const = default;
  bool operator!=(const Unicode &) const = default;
  operator bool() const { return unicode_ == unicode_char_invalid; }

protected:
  unicode_char_t unicode_{unicode_char_invalid};
};

class UnicodeString {
public:
  template <typename T> UnicodeString(T arg) {
    utf8_string_view_t sv(arg);
    while (!sv.empty()) {
      unicode_.push_back(get_next_unicode_char(sv));
    }
  }

  operator std::string() const {
    std::ostringstream result;
    for (auto &c : unicode_) {
      result << Unicode(c);
    }
    return result.str();
  }

  template <typename S>
  friend inline S &operator<<(S &os, const UnicodeString &unicode) {
    return os << std::string(unicode);
  }

  std::u32string unicode_;
};

#endif
