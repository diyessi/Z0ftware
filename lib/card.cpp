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

// Everything shares the original encoding
// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/024-026/A24-0520-3_24_26_Card_Punch_Reference_Manual_Oct1965.pdf
// Page 6
const std::vector<ColumnChar> &getBaseCardEncoding() {
  static std::vector<ColumnChar> table = {
      // Blank
      {{}, ' '},
      // Digits
      {{0}, '0'},
      {{1}, '1'},
      {{2}, '2'},
      {{3}, '3'},
      {{4}, '4'},
      {{5}, '5'},
      {{6}, '6'},
      {{7}, '7'},
      {{8}, '8'},
      {{9}, '9'},
      // Alphabetis
      {{12, 1}, 'A'},
      {{12, 2}, 'B'},
      {{12, 3}, 'C'},
      {{12, 4}, 'D'},
      {{12, 5}, 'E'},
      {{12, 6}, 'F'},
      {{12, 7}, 'G'},
      {{12, 8}, 'H'},
      {{12, 9}, 'I'},
      {{11, 1}, 'J'},
      {{11, 2}, 'K'},
      {{11, 3}, 'L'},
      {{11, 4}, 'M'},
      {{11, 5}, 'N'},
      {{11, 6}, 'O'},
      {{11, 7}, 'P'},
      {{11, 8}, 'Q'},
      {{11, 9}, 'R'},
      {{0, 2}, 'S'},
      {{0, 3}, 'T'},
      {{0, 4}, 'U'},
      {{0, 5}, 'V'},
      {{0, 6}, 'W'},
      {{0, 7}, 'X'},
      {{0, 8}, 'Y'},
      {{0, 9}, 'Z'},
  };
  return table;
}

namespace {
std::vector<ColumnChar> createBCDEncoding(std::vector<ColumnChar> &&symbols) {
  auto table = getBaseCardEncoding();
  table.reserve(table.size() + symbols.size());
  std::move(symbols.begin(), symbols.end(), std::inserter(table, table.end()));
  return table;
}
} // namespace

// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/024-026/A24-0520-3_24_26_Card_Punch_Reference_Manual_Oct1965.pdf
// Page 6
const std::vector<ColumnChar> &get026CommercialEncoding() {
  static std::vector<ColumnChar> table = createBCDEncoding({
      //
      {{12}, '&'},
      {{12, 3, 8}, '.'},
      {{12, 4, 8}, u'¤'},
      //
      {{11}, '-'},
      {{11, 3, 8}, '$'},
      {{11, 4, 8}, '*'},
      //
      {{0, 1}, '/'},
      {{0, 3, 8}, ','},
      //
      {{3, 8}, '#'},
      {{4, 8}, '@'}
      //
  });
  return table;
}

// From:
// https://bitsavers.org/pdf/ibm/punchedCard/Keypunch/029/A24-3332-3_29_Reference_Man.pdf
// page 5
const std::vector<ColumnChar> &get029Encoding() {
  static std::vector<ColumnChar> table = createBCDEncoding({
      //
      {{12}, '&'},
      {{12, 2, 8}, u'¢'},
      {{12, 3, 8}, '.'},
      {{12, 4, 8}, '<'},
      {{12, 5, 8}, '('},
      {{12, 6, 8}, '+'},
      {{12, 7, 8}, '|'},
      //
      {{11}, '-'},
      {{11, 2, 8}, '!'},
      {{11, 3, 8}, '$'},
      {{11, 4, 8}, '*'},
      {{11, 5, 8}, ')'},
      {{11, 6, 8}, ';'},
      {{11, 7, 8}, u'¬'},
      //
      {{0, 1}, '/'},
      {{0, 2, 8}, ' '},
      {{0, 3, 8}, ','},
      {{0, 4, 8}, '%'},
      {{0, 5, 8}, '_'},
      {{0, 6, 8}, '>'},
      {{0, 7, 8}, '?'},
      //
      {{2, 8}, ':'},
      {{3, 8}, '#'},
      {{4, 8}, '@'},
      {{5, 8}, '\''},
      {{6, 8}, '='},
      {{7, 8}, '"'}
      //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/704/704_FortranProgRefMan_Oct56.pdf
// Page 49
const std::vector<ColumnChar> &getFORTRAN704Encoding() {
  static std::vector<ColumnChar> table = createBCDEncoding({
      {{8, 3}, '='},
      // This - cannot be used as a - operation in FORTRAN
      {{8, 4}, '-'},
      {{12}, '+'},
      {{12, 8, 3}, '.'},
      {{12, 8, 4}, ')'},
      {{11}, '-'},
      {{11, 8, 3}, '$'},
      {{11, 8, 4}, '*'},
      {{0, 1}, '/'},
      {{0, 8, 3}, ','},
      {{0, 8, 4}, '('},
  });
  return table;
}

// Coded Character Sets History and Development, C. E. MacKenzie, 1980
// https://archive.org/details/mackenzie-coded-char-sets
// Page 101
const std::vector<ColumnChar> &getFORTRAN704Encoding4() {
  static std::vector<ColumnChar> table =
      createBCDEncoding({{{8, 3}, '='},
                         // This - cannot be used as a - operation in FORTRAN
                         {{8, 4}, '\''},
                         {{8, 5}, ':'},
                         {{8, 6}, '>'},
                         {{12}, '+'},
                         {{12, 8, 2}, '?'},
                         {{12, 8, 3}, '.'},
                         {{12, 8, 4}, ')'},
                         {{12, 8, 6}, '<'},
                         {{11}, '-'},
                         {{11, 8, 2}, '!'},
                         {{11, 8, 3}, '$'},
                         {{11, 8, 4}, '*'},
                         {{11, 8, 6}, ';'},
                         {{0, 1}, '/'},
                         {{0, 8, 3}, ','},
                         {{0, 8, 4}, '('},
                         {{0, 8, 7}, '"'}});
  return table;
}

// http://www.bitsavers.org/pdf/ibm/7090/C28-6235-2_7090_FAP.pdf
// Page 68
// https://bitsavers.org/pdf/ibm/7090/C28-6054-4_7090_FORTRANII.pdf
// Page 46
// https://bitsavers.org/pdf/ibm/7090/C28-6311-4_MAP_Oct64.pdf
// Page 56
// https://bitsavers.org/pdf/ibm/7090/GC28-6392-4_MAP_Dec66.pdf
// Page 58
const std::vector<ColumnChar> &getFAPEncoding() {
  static std::vector<ColumnChar> table = createBCDEncoding(
      {{columnFromRows({}), ' '},
       {{12}, '+'},
       {{11}, '-'},
       {{0, 1}, '/'},
       {{8, 3}, '='},
       // Was `-` in older versions, note not corrected on page
       {{8, 4}, '\''},
       {{12, 8, 3}, '.'},
       {{12, 8, 4}, ')'},
       {{11, 8, 3}, '$'},
       {{11, 8, 4}, '*'},
       {{0, 8, 3}, ','},
       {{0, 8, 4}, '('}});
  return table;
}

// https://bitsavers.org/pdf/ibm/7090/C28-6274-1_7090_FORTRANIV.pdf
// Page 33
const std::vector<ColumnChar> &getFORTRANIVEncoding() {
  static std::vector<ColumnChar> table = createBCDEncoding({
      //
      {{8, 3}, '='},
      {{8, 4}, '\''},
      {{12}, '+'},
      {{12, 8, 3}, '.'},
      {{12, 8, 4}, ')'},
      {{11}, '-'},
      {{11, 8, 3}, '$'},
      {{11, 8, 4}, '*'},
      {{0, 1}, '/'},
      {{0, 8, 3}, ','},
      {{0, 8, 4}, '('}
      //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8
// Comercial collating sequence
// See https://bitsavers.org/pdf/ibm/7090/C28-6365-1_genSort_Sep64.pdf
// Page 7
const std::vector<ColumnChar> &getBCDIC1() {
  static std::vector<ColumnChar> table = createBCDEncoding({
      //
      {{12, 3, 8}, '.'},
      {{12, 4, 8}, u'¤'},
      {{12, 5, 8}, '['},
      {{12, 6, 8}, '<'},
      // Triple dagger
      {{12, 7, 8}, u'\u2E4B'},
      {{12}, '&'},
      {{11, 3, 8}, '$'},
      {{11, 4, 8}, '*'},
      {{11, 5, 8}, ']'},
      {{11, 6, 8}, ';'},
      // Triangle
      {{11, 7, 8}, u'\u25B3'},
      {{11}, '-'},
      {{0, 1}, '/'},
      {{0, 3, 8}, ','},
      {{0, 4, 8}, '%'},
      // Gamma-like thing
      {{0, 5, 8}, u'\u0194'},
      {{0, 6, 8}, '\\'},
      // Triple plus
      {{0, 7, 8}, u'\u29FB'},
      // Encoding used for 0 on tape
      {{2, 8}, '0'},
      {{3, 8}, '#'},
      {{4, 8}, '@'},
      {{5, 8}, ':'},
      {{6, 8}, '>'},
      // square root
      {{7, 8}, u'\u221A'},
      // Equivalent to 12-2-8
      {{12, 0}, '?'},
      {{11, 0}, '!'},
      {{0, 2, 8}, u'‡'},
      //
  });
  return table;
}

// https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf
// Page 8
// Comercial collating sequence
// See https://bitsavers.org/pdf/ibm/7090/C28-6365-1_genSort_Sep64.pdf
// Page 7
const std::vector<ColumnChar> &getBCDIC2() {
  static std::vector<ColumnChar> table = createBCDEncoding({

      //
      {{}, ' '},
      {{12, 3, 8}, '.'},
      {{12, 4, 8}, ')'},
      {{12, 5, 8}, '['},
      {{12, 6, 8}, '<'},
      // Triple dagger
      {{12, 7, 8}, u'\u2E4B'},
      {{12}, '+'},
      {{11, 3, 8}, '$'},
      {{11, 4, 8}, '*'},
      {{11, 5, 8}, ']'},
      {{11, 6, 8}, ';'},
      // Triangle
      {{11, 7, 8}, u'\u25B3'},
      {{11}, '-'},
      {{0, 1}, '/'},
      {{0, 3, 8}, ','},
      {{0, 4, 8}, '('},
      // Gamma-like thing
      {{0, 5, 8}, u'\u0194'},
      {{0, 6, 8}, '\\'},
      // Triple plus
      {{0, 7, 8}, u'\u29FB'},
      // Used for 0 on tape. Strange character in table
      {{2, 8}, '0'},
      {{3, 8}, '='},
      {{4, 8}, '\''},
      {{5, 8}, ':'},
      {{6, 8}, '>'},
      // square root
      {{7, 8}, u'\u221A'},
      // Equivalent to 12-2-8
      {{12, 0}, '?'},
      {{11, 0}, '!'},
      {{0, 2, 8}, u'‡'},
      //
  });
  return table;
}

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
