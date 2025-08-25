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
#include "Z0ftware/bcd.hpp"
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
    assert(odd_parity_bcd_t(b0) == oddParity(bcd_t(b0)));
    b0 &= 0x3f;
    uint8_t b1 = buffer[i++];
    assert(odd_parity_bcd_t(b1) == oddParity(bcd_t(b1)));
    b1 &= 0x3f;
    columns_[j++] = hollerith_t(b0) << 6 | hollerith_t(b1);
  }
}

void BinaryColumnCard::fill(const BinaryRowCard &card) {
  std::fill(columns_.begin(), columns_.end(), 0);
}

void BinaryRowCard::fill(const BinaryColumnCard &card) {
  // The 9L row is mapped to columns 1, 2, 3 of a binary card with bits 1 to
  // 12 of a word going with rows 12 to 9.
  //
  // We pair Hollerith row numbers (12, 11, 0, 1, 2, ..., 9) with bit
  // little-endian positions (11, 10, ..., 0), so column 1
  //
  // 12 :  9L1 9L13 9L25 * 11
  // 11 :  9L2 9L14 9L26 * 10
  //  * :   *    *    *  *  *
  //  9 : 9L12 9L24 9L36 *  0
  //
  // 9L : 1.12 1.11 1.0 ... 1.9 2.12 2.11 2.0 ... 2.9 ... 3.12 3.11 3.0 ... 3.9
  int startCol = 1;
  for (int row = 0; row < 12; ++row) {
    for (int side = 0; side < 2; ++side) {
      word_t &word = rowWords_[side][row];
      word = 0;
      for (int col = startCol - 1; col < startCol + 3 - 1; ++col) {
        word = word << 12 | card.getColumns().at(col).value();
      }
      startCol += 3;
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
    result = (result << 1) | ((data_[column].value() & mask) >> bitpos);
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
    assert(odd_parity_bcd_t(b0) == oddParity(bcd_t(b0)));
    b0 &= 0x3f;
    uint8_t b1 = buffer[i++];
    assert(odd_parity_bcd_t(b1) == oddParity(bcd_t(b1)));
    b1 &= 0x3f;
    cardImage[j++] = hollerith_t(b0) << 6 | hollerith_t(b1);
  }
  return cardImage;
}

void writeCBN(std::ostream &output, const CardImage &cardImage) {
  std::array<char, 160> buffer{0};
  auto bufferp = &buffer[0];
  for (int column = 1; column <= 80; column++) {
    auto column_value = cardImage[column];
    // First byte has bit 7 set
    auto high = bcd_t(ldb<6, 6>(column_value));
    if (column == 1) {
      high = evenParity(high) | even_parity_bcd_t(0x80);
    } else {
      high = oddParity(high);
    }
    *bufferp++ = high.value();
    auto low = oddParity(bcd_t(ldb<0, 6>(column_value)));
    *bufferp++ = low.value();
  }
  output.write(buffer.data(), 160);
}
