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

#ifndef Z0FTWARE_OP_HPP
#define Z0FTWARE_OP_HPP

#include "Z0ftware/field.hpp"
#include "Z0ftware/word.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class OpSpec {
public:
  using Prefix = BitField<35 - 2, 3>;
  using OpCode = BitField<35 - 11, 12>;
  using Decrement = BitField<35 - 17, 15>;
  using Tag = BitField<35 - 20, 3>;
  using Address = BitField<35 - 35, 15>;

  OpSpec(std::string &&operation, int16_t opCode, addr_t address,
         std::string &&description, bool isConstant = false)
      : operation_(std::move(operation)), description_(std::move(description)),
        isConstant_(isConstant) {
    OpCode::ref(word_) = (opCode < 0 ? (04000 | -opCode) : opCode);
    Address::ref(word_) = address;
  }

  OpSpec(std::string &&operation, int16_t opCode, std::string &&description)
      : OpSpec(std::move(operation), opCode, 0, std::move(description), false) {
  }

  OpSpec(bool isConstant, std::string &&operation, int16_t opCode,
         std::string &&description)
      : OpSpec(std::move(operation), opCode, 0, std::move(description),
               isConstant) {}

  const std::string &getOperation() const { return operation_; }
  const std::string &getDescription() const { return description_; }
  word_t getWord() const { return word_; }
  bool isConstant() const { return isConstant_; }

  auto getPrefix() const { return Prefix::ref(word_); }
  auto getOpCode() const { return OpCode::ref(word_); }
  auto getDecrement() const { return Decrement::ref(word_); }
  auto getTag() const { return Tag::ref(word_); }
  auto getAddress() const { return Address::ref(word_); }

  static std::optional<const OpSpec *> getOpSpec(const std::string_view &name);
  static const OpSpec *getOpSpec(word_t word);

private:
  word_t word_{0};
  std::string operation_;
  std::string description_;
  bool isConstant_{false};
};

#endif
