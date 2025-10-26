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
#include "Z0ftware/parity.hpp"

#include <istream>

P7BIStream::P7BIStream(std::istream &input) : input_(input) {
  bufferNext_ = nullptr;
  bufferEnd_ = nullptr;
  recordEnd_ = nullptr;
  tapePos_ = input_.tellg();
  nextRecord();
  recordNum_ = 0;
}

void P7BIStream::fillTapeBuffer() {
  if (!(error_ || eot_) && bufferNext_ == bufferEnd_) {
    bufferPos_ = input_.tellg();
    input_.read(tapeBuffer_.data(), tapeBuffer_.size());
    bufferNext_ = tapeBuffer_.data();
    bufferEnd_ = bufferNext_ + input_.gcount();

    if (bufferNext_ == bufferEnd_) {
      // End of input before EOF marker
      eot_ = true;
      return;
    }
    findNextBOR();
  }
}

void P7BIStream::findNextBOR() {
  recordEnd_ = std::find_if(bufferNext_, bufferEnd_,
                            [](char c) { return 0x80 == (c & 0x80); });
  if (recordEnd_ < bufferEnd_) {
    *recordEnd_ &= 0x7F;
  }
}

bool P7BIStream::nextRecord() {
  while (true) {
    if (error_ || eot_) {
      return false;
    }

    if (recordEnd_ == bufferEnd_) {
      fillTapeBuffer();
    } else {
      break;
    }
  }
  bufferNext_ = recordEnd_;
  recordPos_ = bufferPos_ + off_type(bufferNext_ - tapeBuffer_.data());
  recordNum_++;
  findNextBOR();
  return true;
}

size_t P7BIStream::read(char *buffer, size_t size) {
  while (true) {
    if (error_ || eot_) {
      return 0;
    }
    if (bufferNext_ == bufferEnd_) {
      fillTapeBuffer();
    }
    break;
  }
  size_t toCopy = std::min<size_t>(recordEnd_ - bufferNext_, size);
  std::copy(bufferNext_, bufferNext_ + toCopy, buffer);
  bufferNext_ += toCopy;
  return toCopy;
}

void LowLevelTapeParser::read() {
  char buffer[40];
  reading_ = true;
  onBeginOfRecord();
  while (reading_) {
    auto pos = getOffset();
    size_t size = tapeIStream_.read(buffer, sizeof(buffer));
    onRead(pos, buffer, size);
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
      pos_type recordStartPos = getRecordPos();
      size_t evenParityCount =
          std::count_if(record_.begin(), record_.end(), [](char c) {
            return isEvenParity(even_parity_bcd_t(c));
          });
      if (2 * evenParityCount < record_.size()) {
        onBinaryRecordData();
      } else {
        onBCDRecordData();
      }
      onEndOfRecord();
      record_.clear();
      if (tapeIStream_.nextRecord()) {
        onBeginOfRecord();
      } else {
        onEndOfTape();
        break;
      }
    }
  }
}
