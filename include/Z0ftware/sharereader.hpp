// MIT License
//
// Copyright (c) 2026 Scott Cyphers
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

#ifndef Z0FTWARE_SHAREREADER_HPP
#define Z0FTWARE_SHAREREADER_HPP

#include "Z0ftware/tape.hpp"

// How to tell when the next deck has started? Short symbolic record?
// Records should be multiples of 72/80/84(?)
// BCD/binary is determined by whether majority of chars are even/odd parity
// Deck header is 1 card record
// Deck data is 1 or more multi-card records. Deck header should indicate
// BCD/binary

// Each deck consists of a one-card record indicating brief information about
// the deck and what format the deck is in.
// The deck consists of one or more multi-card records in the format specified
// in the header. Blank cards are used at the end if needed to pad to the record
// length.
// Interface for IBM 70x style tapes with decks encoded as 7-bit
// odd-parity of 6-bit values and BCD (character) records encoded as 7-bit even
// parity of 6-bit characters. An one card deck identifier record precedes each
// deck, which consists of one or more records or multiple cards each. Blank
// cards pad the record to uniform size.
//
// TODO: Don't implement TapeIRecordStream; instead use a record-invisible
// deck/card interface where the header card record size determines the card
// width for the deck.
class ShareReader
    : public Delegate<TapeIRecordStream, TapeIRecordStream, TapeIRecordStream> {
  using delegate_t =
      Delegate<TapeIRecordStream, TapeIRecordStream, TapeIRecordStream>;

public:
  // Positions at the first record of the first deck.
  ShareReader(TapeIRecordStream &input);

  // Moves to the next deck. Returns true if successful. Otherwise eod() or
  // fail() will be true.
  bool nextDeck();

  std::string_view getDeckHeader();

  // Returns true if end of deck has been reached
  bool eod() const;

  // Reads 7-bit from the deck data. Returns 0 at end of record.
  std::streamsize read(char *buffer, std::streamsize count) override;

  bool isBCD() const { return isBCD_; }
  bool isBinary() const { return !isBCD_; }

  size_t getDeckNum() const { return deckNum_; }

protected:
  void fillRecordBuffer();
  bool readRecord();
  void readHeader();
  void initialize();

  bool initialized_{false};
  size_t deckNum_{0};

  // Long enough for any record
  static constexpr size_t recordBufferSize_ = 4096;
  std::array<char, recordBufferSize_> recordBuffer_{0};
  char *recordBufferStart_{recordBuffer_.data()};
  char *recordBufferEnd_;
  char *recordBufferNext_;
  bool recordBufferHasHeader_{false};
  bool isBCD_{false};

  static constexpr size_t headerBufferSize_ = 84;
  std::array<char, headerBufferSize_> headerBuffer_{0};
  char *headerBufferStart_{headerBuffer_.data()};
  char *headerBufferEnd_;
};

#endif
