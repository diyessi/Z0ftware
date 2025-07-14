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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/field.hpp"
#include "Z0ftware/word.hpp"

#include <array>
#include <string>
#include <vector>

// Pairs a Hollerith encoding with a unicode character
struct HollerithChar {

  HollerithChar(const card_column_t &column, char32_t unicode)
      : column(column), unicode(unicode) {}
  card_column_t column;
  char32_t unicode;
};

const std::vector<HollerithChar> &get029Encoding();
const std::vector<HollerithChar> &get026CommercialEncoding();
const std::vector<HollerithChar> &getFAPEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding();
const std::vector<HollerithChar> &getFORTRANIVEncoding();
const std::vector<HollerithChar> &getFORTRAN704Encoding4();
const std::vector<HollerithChar> &getBCDIC1();

using CardTextField = TextField<cardColumnFirst, cardColumnLast>;

class CardImage {
public:
  CardImage() { clear(); }

  // Row: 12 11  0  1  2  3  4  5  6  7  8  9
  // Bit: 11 10  9  8  7  6  5  4  3  2  1  0
  using data_t = std::array<card_column_t, numCardColumns>;
  card_column_t &operator[](int column) { return data_[column - 1]; }
  const card_column_t &operator[](int column) const {
    return data_[column - 1];
  }
  data_t &getData() { return data_; }
  const data_t &getData() const { return data_; }

  // The 704 reads/writes a card as 24 words:
  // Word  0 (9: 1-36)
  //       1 (9:37-72)
  //       2 (8: 1-36)
  // ...
  //      23 (12:37-72)
  word_t getWord(int position) const;
  void setWord(int position, word_t value);
  void clear() { data_.fill(0); }

private:
  data_t data_{0};
};

CardImage readCBN(std::istream &input);
void writeCBN(std::ostream &output, const CardImage &cardImage);

class BinaryRowCard;
class BinaryColumnCard {
  friend BinaryRowCard;

public:
  BinaryColumnCard() = default;
  BinaryColumnCard(const BinaryRowCard &card) { fill(card); }

  const auto &getColumns() const { return columns_; }
  auto &getColumns() { return columns_; }

  void readCBN(std::istream &input);

  BinaryColumnCard &operator=(const BinaryRowCard &card) {
    fill(card);
    return *this;
  }

private:
  // Initialize from card
  void fill(const BinaryRowCard &card);

  std::array<card_column_t, numCardColumns> columns_{0};
};

class BinaryRowCard {
  friend BinaryColumnCard;

public:
  using column_num_t = int;
  using row_num_t = int;

  BinaryRowCard(const BinaryColumnCard &card) { fill(card); }

  const auto &getRowWords() const { return rowWords_; }
  auto &getRowWords() { return rowWords_; }

  BinaryRowCard &operator=(BinaryColumnCard &card) {
    fill(card);
    return *this;
  }

private:
  // Initialize from card
  void fill(const BinaryColumnCard &card);

  std::array<std::array<word_t, 12>, 3> rowWords_{{0}};
};

class SAPDeck {
public:
  SAPDeck(std::istream &stream);
  SAPDeck() = default;

  std::ostream &operator<<(std::ostream &os) const;

  const std::vector<std::string> &getCards() const { return cards_; }

private:
  std::vector<std::string> cards_;
};

inline std::ostream &operator<<(std::ostream &os, const SAPDeck &deck) {
  return deck.operator<<(os);
}

#endif
