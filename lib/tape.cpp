// MIT License
//
// Copyright (c) 2024 Scott Cyphers
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

#include "Z0ftware/tape.hpp"
#include "Z0ftware/bcd.hpp"
#include "Z0ftware/parity.hpp"

#include <istream>

P7BIStream::P7BIStream(std::istream &input) : input_(input) {
  fillTapeBuffer();
  tapeBORPos_ = tapeBufferPos_;
  if (0x80 == (*tapeBORPos_ & 0x80)) {
    *tapeBORPos_ &= 0x7F;
    findNextBOR();
  }
}

void P7BIStream::fillTapeBuffer() {
  if (!(error_ || eot_) && tapeBufferPos_ == tapeBufferEnd_) {
    tapeBufferPos_ = tapeBuffer_.data();
    input_.read(tapeBufferPos_, tapeBuffer_.size());
    tapeBufferEnd_ = tapeBufferPos_ + input_.gcount();
    if (tapeBufferPos_ == tapeBufferEnd_) {
      // End of input before EOF marker
      eot_ = true;
      return;
    }
  }
}

void P7BIStream::findNextBOR() {
  fillTapeBuffer();
  tapeBORPos_ = std::find_if(tapeBufferPos_, tapeBufferEnd_,
                             [](char c) { return 0x80 == (c & 0x80); });
  if (tapeBORPos_ < tapeBufferEnd_) {
    *tapeBORPos_ &= 0x7F;
  }
}

bool P7BIStream::nextRecord() {
  if (error_ || eot_) {
    return false;
  }

  if (tapeBORPos_ == tapeBufferEnd_) {
    do {
      // Skip to the next record
      findNextBOR();
      if (error_ || eot_) {
        break;
      }
    } while (tapeBORPos_ == tapeBufferEnd_);
    return false;
  } else {
    findNextBOR();
    return true;
  }
}

size_t P7BIStream::read(char *buffer, size_t size) {
  if (error_ || eot_) {
    return 0;
  }
  char *bufferPos = buffer;
  char *bufferEnd = buffer + size;
  while (bufferPos < bufferEnd) {
    if (tapeBufferPos_ < tapeBORPos_) {
      size_t toCopy =
          std::min(bufferEnd - bufferPos, tapeBORPos_ - tapeBufferPos_);
      std::copy(tapeBufferPos_, tapeBufferPos_ + toCopy, bufferPos);
      bufferPos += toCopy;
      tapeBufferPos_ += toCopy;
    }
    if (tapeBufferPos_ == tapeBufferEnd_) {
      // Refill tapeBuffer
      findNextBOR();
    }
    if (tapeBufferPos_ == tapeBORPos_) {
      break;
    }
  }
  return bufferPos - buffer;
}

void TapeReadAdapter::read() {
  char buffer[40];
  reading_ = true;
  size_t pos = 0;
  onBeginOfRecord();
  while (reading_) {
    size_t size = tapeIStream_.read(buffer, sizeof(buffer));
    onRead(pos, buffer, size);
    pos += size;
    if (1 == size) {
      // Marker
      char mark = buffer[0] & 0x0F;
      if (0x0F == mark) {
        onEndOfFile();
        continue;
      }
    }
    if (size > 0) {
      onRecordData(buffer, size);

      std::copy(&buffer[0], &buffer[size], std::back_inserter(record_));
    } else {
      if (tapeIStream_.nextRecord()) {
        onEndOfRecord();
        size_t evenParityCount =
            std::count_if(record_.begin(), record_.end(),
                          [](char c) { return isEvenParity(parity_bcd_t(c)); });
        if (2 * evenParityCount < record_.size()) {
          onBinaryRecordData();
        } else {
          onBCDRecordData();
        }
        record_.clear();
        recordStartPos_ = tapePos_;
        onBeginOfRecord();
      } else {
        onEndOfTape();
        break;
      }
    }
    tapePos_ += size;
  }
}
