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

#include "Z0ftware/sharereader.hpp"
#include "Z0ftware/parity.hpp"
#include "Z0ftware/tape.hpp"

ShareReader::ShareReader(TapeIRecordStream &input) : delegate_t(input) {}

void ShareReader::fillRecordBuffer() {
  if (fail()) {
    return;
  }

  if (input_.isEOR()) {
    if (!input_.nextRecord()) {
      return;
    }
  }

  recordBufferEnd_ = recordBufferStart_;
  recordBufferNext_ = recordBufferStart_;
  char *bufferEnd = recordBufferEnd_ + recordBuffer_.size();
  while (recordBufferEnd_ < bufferEnd) {
    auto numRead = input_.read(recordBufferEnd_, bufferEnd - recordBufferEnd_);
    if (numRead == 0) {
      break;
    }
    recordBufferEnd_ += numRead;
  }
  size_t evenParityCount =
      std::count_if(recordBufferStart_, recordBufferEnd_,
                    [](char c) { return isEvenParity(even_parity_bcd_t(c)); });

  size_t size = recordBufferEnd_ - recordBufferStart_;
  isBCD_ = evenParityCount * 2 > size;
  if (isBCD_ && size <= 84) {
    recordBufferHasHeader_ = true;
  }
}

void ShareReader::initialize() {
  if (!initialized_) {
    fillRecordBuffer();
    nextDeck();
    deckNum_ = 0;
    initialized_ = true;
  }
}

std::streamsize ShareReader::read(char *buffer, std::streamsize count) {
  initialize();
  if (recordBufferNext_ == recordBufferEnd_) {
    fillRecordBuffer();
  }

  if (recordBufferHasHeader_) {
    // End of deck
    return 0;
  }

  if (recordBufferStart_ == recordBufferEnd_) {
    // No more records
    return 0;
  }

  std::size_t toCopy =
      std::min<std::size_t>(count, recordBufferEnd_ - recordBufferNext_);
  std::copy(recordBufferNext_, recordBufferNext_ + toCopy, buffer);
  recordBufferNext_ += toCopy;
  return toCopy;
}

bool ShareReader::nextDeck() {
  if (!recordBufferHasHeader_) {
    // TODO: Read records until deck header is found
    return false;
  }
  size_t size = recordBufferEnd_ - recordBufferStart_;
  if (!isBCD_ || size > 84) {
    return false;
  }
  std::copy(recordBufferStart_, recordBufferEnd_, headerBufferStart_);
  headerBufferEnd_ = headerBufferStart_ + size;

  recordBufferNext_ = recordBufferEnd_;
  recordBufferHasHeader_ = false;

  ++deckNum_;

  return true;
}

std::string_view ShareReader::getDeckHeader() {
  initialize();
  return std::string_view(headerBufferStart_,
                          headerBufferEnd_ - headerBufferStart_);
}
