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

// Helpers for working with bit fields

#ifndef Z0FTWARE_FIELD_HPP
#define Z0FTWARE_FIELD_HPP

#include <cstdint>
#include <string_view>

using sixbit_t = std::byte;
using sevenbit_t = std::byte;

using bcd_t = sixbit_t;
using tbcd_t = sevenbit_t;

// Size of a bit size
using bit_size_size_t = std::uint_least8_t;
// size of a little-endian bit position
using bit_pos_size_t = std::uint_least8_t;

template <bit_size_size_t bit_size, typename unsigned_imp_t,
          typename signed_imp_t>
struct BitSizeTraitsImp {
  static constexpr bit_size_size_t size() { return bit_size; }
  using unsigned_t = unsigned_imp_t;
  using signed_t = signed_imp_t;
};

template <bit_size_size_t bit_size> struct BitSizeTraits;

// Define the signed/unsigned int types to use for each field size
// int8_t types are not used because of character overloadings
#define BITSIZETRAITS(BITSIZE, UNSIGNED_TYPE, SIGNED_TYPE)                     \
  template <>                                                                  \
  struct BitSizeTraits<BITSIZE>                                                \
      : public BitSizeTraitsImp<BITSIZE, UNSIGNED_TYPE, SIGNED_TYPE> {}

BITSIZETRAITS(1, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(2, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(3, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(4, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(5, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(6, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(7, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(8, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(9, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(10, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(11, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(12, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(13, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(14, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(15, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(16, std::uint_least16_t, std::int_least16_t);
BITSIZETRAITS(17, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(18, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(19, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(20, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(21, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(22, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(23, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(24, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(25, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(26, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(27, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(28, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(29, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(30, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(31, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(32, std::uint_least32_t, std::int_least32_t);
BITSIZETRAITS(33, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(34, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(35, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(36, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(37, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(38, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(39, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(40, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(41, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(42, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(43, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(44, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(45, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(46, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(47, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(48, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(49, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(50, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(51, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(52, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(53, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(54, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(55, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(56, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(57, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(58, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(59, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(60, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(61, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(62, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(63, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(64, std::uint_least64_t, std::int_least64_t);
BITSIZETRAITS(65, __uint128_t, __int128_t);
BITSIZETRAITS(66, __uint128_t, __int128_t);
BITSIZETRAITS(67, __uint128_t, __int128_t);
BITSIZETRAITS(68, __uint128_t, __int128_t);
BITSIZETRAITS(69, __uint128_t, __int128_t);
BITSIZETRAITS(70, __uint128_t, __int128_t);
BITSIZETRAITS(71, __uint128_t, __int128_t);
BITSIZETRAITS(72, __uint128_t, __int128_t);
BITSIZETRAITS(73, __uint128_t, __int128_t);
BITSIZETRAITS(74, __uint128_t, __int128_t);
BITSIZETRAITS(75, __uint128_t, __int128_t);
BITSIZETRAITS(76, __uint128_t, __int128_t);
BITSIZETRAITS(77, __uint128_t, __int128_t);
BITSIZETRAITS(78, __uint128_t, __int128_t);
BITSIZETRAITS(79, __uint128_t, __int128_t);
BITSIZETRAITS(80, __uint128_t, __int128_t);

// unsigned_t<size> is the type to use for a size-bit value
template <bit_size_size_t field_size>
using unsigned_t = BitSizeTraits<field_size>::unsigned_t;

template <bit_size_size_t field_size>
using signed_t = BitSizeTraits<field_size>::signed_t;

template <bit_size_size_t bit_size, bit_pos_size_t bit_pos = 0>
constexpr unsigned_t<bit_size + bit_pos> bitmask =
    unsigned_t<bit_size + bit_pos>(
        ((unsigned_t<bit_size + 1>(1) << bit_size) - 1) << bit_pos);

// ldb<bit_pos, bit_size>
template <bit_pos_size_t bit_pos, bit_size_size_t bit_size,
          typename value_t = unsigned_t<bit_pos + bit_size>,
          typename field_t = unsigned_t<bit_size>>
constexpr field_t ldb(value_t value) {
  return field_t(bitmask<bit_size> & (value >> bit_pos));
}

// Return word with byte replaced by field
template <bit_pos_size_t bit_pos, bit_size_size_t bit_size, typename value_t,
          typename field_t>
constexpr value_t zipb(field_t field, value_t value = 0) {
  return (value & ~(value_t(bitmask<bit_size>) << bit_pos)) |
         (value_t(bitmask<bit_size> & field) << bit_pos);
}

// Deposit byte in word, returning deposited field
template <bit_pos_size_t bit_pos, bit_size_size_t bit_size, typename value_t,
          typename field_t>
constexpr field_t dpb(field_t field, value_t &value = 0) {
  field_t maskedField = bitmask<bit_size> & field;
  value = (value & ~(value_t(bitmask<bit_size>) << bit_pos)) |
          (value_t(maskedField) << bit_pos);
  return maskedField;
}

/// A BitFieldRef is a type for a bitfield stored within an integer.
template <typename BitField_t, typename value_t_ = typename BitField_t::word_t,
          typename field_t_ = typename BitField_t::field_t>
class BitFieldRef {
public:
  using field_t = field_t_;
  using value_t = value_t_;
  static constexpr bit_pos_size_t bit_pos = BitField_t::bit_pos;
  static constexpr bit_size_size_t bit_size = BitField_t::bit_size;
  BitFieldRef(value_t &word) : value_(word) {}
  operator auto() const {
    return ldb<bit_pos, bit_size, value_t, field_t>(value_);
  }
  auto operator=(typename BitField_t::field_t field) {
    return dpb<bit_pos, bit_size>(field, value_);
  }

  template <typename word_ref_t, typename field_ref_t = field_t>
  static auto ref(word_ref_t &word) {
    return BitFieldRef<BitField_t, word_ref_t, field_ref_t>(word);
  }

private:
  value_t &value_;
};

/// A BitField is a type for a field stored as a sequence of bits within an
/// unsigned integer (the word).
template <bit_pos_size_t bit_pos_, bit_size_size_t bit_size_,
          typename field_t_ = unsigned_t<bit_size_>>
class BitField {
public:
  template <typename BitField_, typename value_t_ = typename BitField_::value_t,
            typename ref_field_t = typename BitField_::field_t>
  class Ref {
  public:
    using field_t = ref_field_t;
    using value_t = value_t_;
    static constexpr bit_pos_size_t bit_pos = BitField_::bit_pos;
    static constexpr bit_size_size_t bit_size = BitField_::bit_size;
    Ref(value_t &value) : value_(value) {}
    operator auto() const {
      return ldb<bit_pos, bit_size, value_t, field_t>(value_);
    }
    auto operator=(typename BitField_::field_t field) {
      return dpb<bit_pos, bit_size>(field, value_);
    }

    template <typename value_ref_t, typename field_ref_t = field_t>
    static auto ref(value_ref_t &value) {
      return Ref<BitField_, value_ref_t, field_ref_t>(value);
    }

  private:
    value_t &value_;
  };

  static constexpr std::uint8_t bit_pos = bit_pos_;
  static constexpr std::uint8_t bit_size = bit_size_;
  using field_t = field_t_;

  template <typename ref_value_t, typename ref_field_t = field_t>
  static auto ref(ref_value_t &value) {
    return Ref<BitField<bit_pos, bit_size, ref_field_t>, ref_value_t,
               ref_field_t>(value);
  }
};

template <typename value_t = std::uint64_t> constexpr value_t bits() {
  return 0;
}

template <typename value_t, typename Bit, typename... MoreBits>
constexpr value_t bits(Bit bit, MoreBits... moreBits) {
  return (value_t(1) << bit) | bits(moreBits...);
}

// Contiguous
template <size_t text_begin_pos, size_t text_size> class TextField {
public:
  constexpr TextField(std::string_view::size_type pos,
                      std::string_view::size_type size)
      : pos_(pos), size_(size) {}

  // Like a reference
  template <typename T, typename V = std::string_view> class Ref {
  public:
    Ref(const TextField &textField, T &text)
        : textField_(textField), text_(text) {}
    Ref(const Ref &) = default;
    Ref(Ref &&) = default;
    Ref &operator=(const Ref &) = default;
    Ref &operator=(Ref &&) = default;

    operator V() const {
      auto limit = text_.end();
      return {std::min(limit, text_.begin() + textField_.pos_ - text_begin_pos),
              std::min(limit, text_.begin() + textField_.pos_ - text_begin_pos +
                                  textField_.size_)};
    }

  private:
    const TextField &textField_;
    T &text_;
  };

  template <typename T> Ref<T> operator()(T &text) const {
    return Ref<T>(*this, text);
  }

private:
  std::string_view::size_type pos_;
  std::string_view::size_type size_;
};

using bcd_zone_t = unsigned_t<2>;

inline bcd_zone_t bcd_zone(bcd_t bcd) { return ldb<4, 2>(unsigned(bcd)); }

using bcd_digits_t = unsigned_t<4>;

inline bcd_digits_t bcd_digits(bcd_t bcd) { return ldb<0, 4>(unsigned(bcd)); }

// 12 11 0
using hollerith_zone_t = unsigned_t<3>;

// 9 8 7 6 5 4 3 2 1 0
using hollerith_digits_t = unsigned_t<10>;

#endif
