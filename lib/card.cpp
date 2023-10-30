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
    columns_[j++] = uint16_t(b0) << 6 | uint16_t(b1);
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
    std::array<uint16_t, 36> columns;
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
