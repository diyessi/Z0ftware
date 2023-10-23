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
#include <string>
#include <string_view>
#include <type_traits>

// Size of field in bits
using bit_field_size_t = std::uint_least8_t;
// Little-endian bit position
using bit_field_pos_t = std::uint_least8_t;

// typename UIntRep<n>
//   word_t is the integer type that is at least size bits
//   word_t mask is a bitmask for the field
template <bit_field_size_t size> struct FieldTraits;

template <bit_field_size_t param_size, typename param_field_t>
struct FieldTraitsImp {
  static constexpr bit_field_size_t size() { return param_size; }
  using field_type = param_field_t;
  // A bitmask for the field
  static constexpr field_type bitmask = field_type(
      (typename FieldTraits<size() + 1>::field_type(1) << size()) - 1);
  // Mask all but the field in value
  template <typename value_t> static constexpr value_t mask(value_t value) {
    return bitmask & value;
  }
};

// Define the int type to use for each field size
// int8_t types are not used because of character overloadings
#define FIELDTRAITS(SIZE, TYPE)                                                \
  template <> struct FieldTraits<SIZE> : public FieldTraitsImp<SIZE, TYPE> {}

FIELDTRAITS(1, std::uint_least16_t);
FIELDTRAITS(2, std::uint_least16_t);
FIELDTRAITS(3, std::uint_least16_t);
FIELDTRAITS(4, std::uint_least16_t);
FIELDTRAITS(5, std::uint_least16_t);
FIELDTRAITS(6, std::uint_least16_t);
FIELDTRAITS(7, std::uint_least16_t);
FIELDTRAITS(8, std::uint_least16_t);
FIELDTRAITS(9, std::uint_least16_t);
FIELDTRAITS(10, std::uint_least16_t);
FIELDTRAITS(11, std::uint_least16_t);
FIELDTRAITS(12, std::uint_least16_t);
FIELDTRAITS(13, std::uint_least16_t);
FIELDTRAITS(14, std::uint_least16_t);
FIELDTRAITS(15, std::uint_least16_t);
FIELDTRAITS(16, std::uint_least16_t);
FIELDTRAITS(17, std::uint_least32_t);
FIELDTRAITS(18, std::uint_least32_t);
FIELDTRAITS(19, std::uint_least32_t);
FIELDTRAITS(20, std::uint_least32_t);
FIELDTRAITS(21, std::uint_least32_t);
FIELDTRAITS(22, std::uint_least32_t);
FIELDTRAITS(23, std::uint_least32_t);
FIELDTRAITS(24, std::uint_least32_t);
FIELDTRAITS(25, std::uint_least32_t);
FIELDTRAITS(26, std::uint_least32_t);
FIELDTRAITS(27, std::uint_least32_t);
FIELDTRAITS(28, std::uint_least32_t);
FIELDTRAITS(29, std::uint_least32_t);
FIELDTRAITS(30, std::uint_least32_t);
FIELDTRAITS(31, std::uint_least32_t);
FIELDTRAITS(32, std::uint_least32_t);
FIELDTRAITS(33, std::uint_least64_t);
FIELDTRAITS(34, std::uint_least64_t);
FIELDTRAITS(35, std::uint_least64_t);
FIELDTRAITS(36, std::uint_least64_t);
FIELDTRAITS(37, std::uint_least64_t);
FIELDTRAITS(38, std::uint_least64_t);
FIELDTRAITS(39, std::uint_least64_t);
FIELDTRAITS(40, std::uint_least64_t);
FIELDTRAITS(41, std::uint_least64_t);
FIELDTRAITS(42, std::uint_least64_t);
FIELDTRAITS(43, std::uint_least64_t);
FIELDTRAITS(44, std::uint_least64_t);
FIELDTRAITS(45, std::uint_least64_t);
FIELDTRAITS(46, std::uint_least64_t);
FIELDTRAITS(47, std::uint_least64_t);
FIELDTRAITS(48, std::uint_least64_t);
FIELDTRAITS(49, std::uint_least64_t);
FIELDTRAITS(50, std::uint_least64_t);
FIELDTRAITS(51, std::uint_least64_t);
FIELDTRAITS(52, std::uint_least64_t);
FIELDTRAITS(53, std::uint_least64_t);
FIELDTRAITS(54, std::uint_least64_t);
FIELDTRAITS(55, std::uint_least64_t);
FIELDTRAITS(56, std::uint_least64_t);
FIELDTRAITS(57, std::uint_least64_t);
FIELDTRAITS(58, std::uint_least64_t);
FIELDTRAITS(59, std::uint_least64_t);
FIELDTRAITS(60, std::uint_least64_t);
FIELDTRAITS(61, std::uint_least64_t);
FIELDTRAITS(62, std::uint_least64_t);
FIELDTRAITS(63, std::uint_least64_t);
FIELDTRAITS(64, __uint128_t);
FIELDTRAITS(65, __uint128_t);
FIELDTRAITS(66, __uint128_t);
FIELDTRAITS(67, __uint128_t);
FIELDTRAITS(68, __uint128_t);
FIELDTRAITS(69, __uint128_t);
FIELDTRAITS(70, __uint128_t);
FIELDTRAITS(71, __uint128_t);
FIELDTRAITS(72, __uint128_t);
FIELDTRAITS(73, __uint128_t);
FIELDTRAITS(74, __uint128_t);

// Load byte
template <bit_field_pos_t pos, bit_field_size_t size,
          typename word_t = typename FieldTraits<pos + size - 1>::field_type,
          typename field_t = typename FieldTraits<size>::field_type>
constexpr field_t ldb(word_t word) {
  return FieldTraits<size>::mask(word >> pos);
}

// Return word with byte replaced by field
template <bit_field_pos_t pos, bit_field_size_t size, typename word_t,
          typename field_t>
constexpr word_t zipb(field_t field, word_t word = 0) {
  field_t maskedField = FieldTraits<size>::mask(field);
  return (word & ~(word_t(FieldTraits<size>::bitmask) << pos)) |
         (word_t(maskedField) << pos);
}

// Deposit byte in word, returning deposited field
template <bit_field_pos_t pos, bit_field_size_t size, typename word_t,
          typename field_t>
constexpr field_t dpb(field_t field, word_t &word = 0) {
  field_t maskedField = FieldTraits<size>::mask(field);
  word = (word & ~(word_t(FieldTraits<size>::bitmask) << pos)) |
         (word_t(maskedField) << pos);
  return maskedField;
}

/// A BitFieldRef is a type for a bitfield stored within an integer.
template <typename FIELD, typename word_param_t = typename FIELD::word_t,
          typename field_param_t = typename FIELD::field_t>
class BitFieldRef {
public:
  using field_t = field_param_t;
  using word_t = word_param_t;
  static constexpr bit_field_pos_t pos = FIELD::pos;
  static constexpr bit_field_size_t size = FIELD::size;
  BitFieldRef(word_t &word) : word_(word) {}
  operator auto() const { return ldb<pos, size, word_t, field_t>(word_); }
  auto operator=(typename FIELD::field_t field) {
    return dpb<pos, size>(field, word_);
  }

  template <typename word_ref_t, typename field_ref_t = field_t>
  static auto ref(word_ref_t &word) {
    return BitFieldRef<FIELD, word_ref_t, field_ref_t>(word);
  }

private:
  word_t &word_;
};

/// A BitField is a type for a field stored as a sequence of bits within an
/// unsigned integer (the word).
template <bit_field_pos_t param_pos, bit_field_size_t param_size,
          typename field_p_t = typename FieldTraits<param_size>::field_type>
class BitField {
public:
  template <typename FIELD, typename word_param_t = typename FIELD::word_type,
            typename field_param_t = typename FIELD::field_type>
  class Ref {
  public:
    using field_type = field_param_t;
    using word_type = word_param_t;
    static constexpr bit_field_pos_t pos = FIELD::pos;
    static constexpr bit_field_size_t size = FIELD::size;
    Ref(word_type &word) : word_(word) {}
    operator auto() const { return ldb<pos, size, word_type, field_type>(word_); }
    auto operator=(typename FIELD::field_type field) {
      return dpb<pos, size>(field, word_);
    }

    template <typename word_ref_t, typename field_ref_t = field_type>
    static auto ref(word_ref_t &word) {
      return Ref<FIELD, word_ref_t, field_ref_t>(word);
    }

  private:
    word_type &word_;
  };

  static constexpr std::uint8_t pos = param_pos;
  static constexpr std::uint8_t size = param_size;
  using field_type = field_p_t;
  using word_type = typename FieldTraits<size + pos - 1>::field_type;

  template <typename param_word_t, typename param_field_t = field_type>
  static auto ref(param_word_t &word) {
    return Ref<BitField<pos, size, param_field_t>, param_word_t, param_field_t>(
        word);
  }
};

template<typename word_t=std::uint64_t>
constexpr word_t bits() { return 0; }

template <typename word_t, typename Bit, typename... MoreBits>
constexpr word_t bits(Bit bit, MoreBits... moreBits) {
  return (word_t(1) << bit) | bits(moreBits...);
}

// Contiguous
class TextField {
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
      return text_.field(textField_.pos_, textField_.size_);
    }

    auto &operator=(const V &sv) {
      return text_.setField(textField_.pos_, textField_.size_, sv);
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

#endif
