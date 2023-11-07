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

#include "Z0ftware/card.hpp"
#include "Z0ftware/field.hpp"
#include "Z0ftware/parity.hpp"

#include <cassert>
#include <iostream>

SAPDeck::SAPDeck(std::istream &stream) {
  char cardText[81];
  while (stream.good()) {
    stream.getline(cardText, sizeof(cardText));
    auto count = stream.gcount();
    if (count == 0) {
      continue;
    }
    cards_.emplace_back(std::string_view(cardText, cardText + count - 1));
  }
}

std::ostream &SAPDeck::operator<<(std::ostream &os) const {
  for (const auto &card : cards_) {
    os << card << "\n";
  }
  return os;
}

void BinaryColumnCard::readCBN(std::istream &input) {
  std::array<char, 160> buffer;
  input.read(buffer.data(), buffer.size());
  auto count = input.gcount();
  if (count == 0) {
    return;
  }
  if (count < 160) {
    std::fill(buffer.begin() + count, buffer.end(), 0);
  }
  size_t i = 0;
  size_t j = 0;
  while (i < buffer.size()) {
    uint8_t b0 = buffer[i++];
    if (i == 1) {
      // First byte has bit 7 set
      assert((b0 & 0x80) != 0);
      b0 &= 0x7f;
    }
    assert(sevenbit_t(b0) == oddParity(sixbit_t(b0)));
    b0 &= 0x3f;
    uint8_t b1 = buffer[i++];
    assert(sevenbit_t(b1) == oddParity(sixbit_t(b1)));
    b1 &= 0x3f;
    columns_[j++] = column_t(b0) << 6 | column_t(b1);
  }
}

void BinaryColumnCard::fill(const BinaryRowCard &card) {
  std::fill(columns_.begin(), columns_.end(), 0);
  const auto &rows = card.getRows();
}

void BinaryRowCard::fill(const BinaryColumnCard &card) {
  std::fill(rows_.begin(), rows_.end(), 0);
  // Left/right side of card
  for (int i = 0; i < 2; ++i) {
    std::array<column_t, 36> columns;
    std::copy(card.getColumns().begin() + 36 * i,
              card.getColumns().begin() + 36 * i + 36, columns.begin());
    for (int row = 0; row < 12; row++) {
      for (int column = 0; column < 36; column++) {
        rows_[i + 2 * row] =
            (rows_[i + 2 * row] << 1) | (columns[column] & 0x001);
        columns[column] = columns[column] >> 1;
      }
    }
  }
}

word_t CardImage::getWord(int position) const {
  // 0-based row/startColumn
  auto bitpos = position / 2;
  auto startColumn = (position % 2) * 36;
  auto endColumn = startColumn + 36;
  word_t result{0};
  word_t mask = 1 << bitpos;
  for (auto column = startColumn; column < endColumn; ++column) {
    result = (result << 1) | ((data_[column] & mask) >> bitpos);
  }
  return result;
}

void CardImage::setWord(int position, word_t value) {
  // 0-based row/startColumn
  auto bitpos = position / 2;
  auto startColumn = (position % 2) * 36;
  auto endColumn = startColumn + 36;
  word_t result{0};
  word_t mask = ~(1 << bitpos);
  for (auto column = endColumn - 1; column >= startColumn; --column) {
    data_[column] = (data_[column] & mask) | ((value & 0x1) << bitpos);
    value >>= 1;
  }
}

CardImage readCBN(std::istream &input) {
  CardImage cardImage;
  std::array<char, 160> buffer;
  input.read(buffer.data(), buffer.size());
  auto count = input.gcount();
  if (count < 160) {
    std::fill(buffer.begin() + count, buffer.end(), 0);
  }
  size_t i = 0;
  size_t j = 1;
  while (i < buffer.size()) {
    uint8_t b0 = buffer[i++];
    if (i == 1) {
      // First byte has bit 7 set
      assert((b0 & 0x80) != 0);
      b0 &= 0x7f;
    }
    assert(sevenbit_t(b0) == oddParity(sixbit_t(b0)));
    b0 &= 0x3f;
    uint8_t b1 = buffer[i++];
    assert(sevenbit_t(b1) == oddParity(sixbit_t(b1)));
    b1 &= 0x3f;
    cardImage[j++] = column_t(b0) << 6 | column_t(b1);
  }
  return cardImage;
}

void writeCBN(std::ostream &output, const CardImage &cardImage) {
  std::array<char, 160> buffer;
  buffer.fill(0);
  auto bufferp = &buffer[0];
  for (int column = 1; column <= 80; column++) {
    auto column_value = cardImage[column];
    // First byte has bit 7 set
    auto high = sixbit_t(ldb<6, 6>(column_value));
    if (column == 1) {
      high = evenParity(high) | sevenbit_t(0x80);
    } else {
      high = oddParity(high);
    }
    *bufferp++ = char(high);
    auto low = oddParity(sixbit_t(ldb<0, 6>(column_value)));
    *bufferp++ = char(low);
  }
  output.write(buffer.data(), 160);
}
