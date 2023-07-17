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

#ifndef Z0FTWARE_CARD
#define Z0FTWARE_CARD

#include "Z0ftware/field.hpp"
#include "Z0ftware/utils.hpp"

#include <algorithm>
#include <string>
#include <string_view>

inline std::string_view cardField(std::string &text,
                                  std::string::size_type beginColumn,
                                  std::string::size_type endColumn) {
  auto limit = text.size();
  return std::string_view(text.begin() + std::min(limit, beginColumn - 1),
                          text.begin() + std::min(limit, endColumn));
}

// Punched card in column format
class InputColumnCard {
public:
  virtual ~InputColumnCard() = default;

  virtual const std::string &getASCII() const = 0;
  virtual size_t getCardNumber() const = 0;
  // 1-based
  virtual std::string_view
  field(std::string_view::size_type pos = 1,
        std::string_view::size_type size = 80) const = 0;
};

// A readable text card initialized from a line of text
class InputTextCard : public InputColumnCard {
public:
  InputTextCard(const std::string_view text, size_t lineNumber = 0)
      : text_(text.begin(), text.end()), cardNumber_(lineNumber) {
    if (text_.size() < 80) {
      text_.append(80 - text_.size(), ' ');
    }
  }

  const std::string &getASCII() const override { return text_; }
  std::string &getASCII() { return text_; }

  size_t getCardNumber() const override { return cardNumber_; }

  // Columns are 1-based
  std::string_view field(std::string::size_type pos,
                         std::string::size_type size) const override {
    auto limit = text_.size();
    return std::string_view(text_.begin() + std::min(limit, pos - 1),
                            text_.begin() + std::min(limit, pos - 1 + size));
  }

private:
  std::string text_;
  size_t cardNumber_;
};

class InputBinaryRowCard;
class InputBinaryColumnCard {
  friend InputBinaryRowCard;

public:
  void readCBN(std::istream &input);
  // Initialize from card
  void fill(const InputBinaryRowCard &card);

private:
  std::array<uint16_t, 80> data_;
};

class InputBinaryRowCard {
  friend InputBinaryColumnCard;

public:
  // Initialize from card
  void fill(const InputBinaryColumnCard &card);
  const auto &getData() const { return data_; }

private:
  std::array<uint64_t, 24> data_;
};

class ASMCard {
public:
  virtual ~ASMCard() = default;

  [[nodiscard]] virtual std::string_view
  getASCII(const InputColumnCard &) const = 0;
  [[nodiscard]] virtual std::string_view
  getASCII72(const InputColumnCard &) const = 0;
  [[nodiscard]] virtual std::string_view
  getLocationSymbol(const InputColumnCard &) const = 0;
  [[nodiscard]] virtual std::string_view
  getOperation(const InputColumnCard &) const = 0;
  [[nodiscard]] virtual std::string_view
  getVariableAndComment(const InputColumnCard &) const = 0;
};

class SAPCard : public ASMCard {

public:
  static constexpr TextField field80{1, 80};
  static constexpr TextField field72{1, 72};
  static constexpr TextField fieldLocationSymbol{1, 6};
  static constexpr TextField fieldOperation{8, 3};
  static constexpr TextField fieldVariableAndComment{12, 60};

  SAPCard() = default;

  [[nodiscard]] std::string_view
  getASCII(const InputColumnCard &inputTextCard) const override {
    return field80(inputTextCard);
  }

  // First 72 chars of text
  [[nodiscard]] std::string_view
  getASCII72(const InputColumnCard &inputTextCard) const override {
    return field72(inputTextCard);
  }
  [[nodiscard]] std::string_view
  getLocationSymbol(const InputColumnCard &inputTextCard) const override {
    return trim(fieldLocationSymbol(inputTextCard));
  }
  [[nodiscard]] std::string_view
  getOperation(const InputColumnCard &inputTextCard) const override {
    return fieldOperation(inputTextCard);
  }
  [[nodiscard]] std::string_view
  getVariableAndComment(const InputColumnCard &inputTextCard) const override {
    return fieldVariableAndComment(inputTextCard);
  }
};

class SAPDeck {
public:
  SAPDeck(std::istream &stream);
  SAPDeck() = default;

  std::ostream &operator<<(std::ostream &os) const;

  const std::vector<InputTextCard> &getCards() const { return cards_; }

private:
  std::vector<InputTextCard> cards_;
};

inline std::ostream &operator<<(std::ostream &os, const SAPDeck &deck) {
  return deck.operator<<(os);
}

#endif
