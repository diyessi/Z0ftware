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

#include "Z0ftware/p7bistream.hpp"
#include "Z0ftware/parity.hpp"

P7BIStream::P7BIStream(Reader &input) : Delegate(input) {
  initialized_ = false;
}

void P7BIStream::fillTapeBuffer() {
  if (!(fail() || eot_) && bufferNext_ == bufferEnd_) {
    bufferNext_ = tapeBuffer_.data();
    bufferEnd_ = bufferNext_ + input_.read(bufferNext_, tapeBuffer_.size());

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
    // Next record starts in this buffer
    *recordEnd_ &= 0x7F;
  }
}

bool P7BIStream::nextRecord() {
  while (true) {
    if (fail() || eot_) {
      return false;
    }

    if (recordEnd_ == bufferEnd_) {
      // Current record not finished yet, skip rest of buffer
      bufferNext_ = bufferEnd_;
      fillTapeBuffer();
    } else {
      break;
    }
  }
  // Now skip to the end of this record
  bufferNext_ = recordEnd_;
  recordPos_ = tellg();
  recordNum_++;
  findNextBOR();
  return true;
}

std::streamsize P7BIStream::read(char *buffer, std::streamsize size) {
  if (!initialized_) {
    bufferNext_ = tapeBuffer_.data();
    bufferEnd_ = bufferNext_;
    recordEnd_ = bufferNext_;
    recordPos_ = tellg();
    recordNum_ = 0;
    fillTapeBuffer();
    findNextBOR();
    initialized_ = true;
  }
  if (fail() || eot_) {
    return 0;
  }
  if (bufferNext_ == bufferEnd_) {
    fillTapeBuffer();
  }
  std::streamsize toCopy =
      std::min<std::streamsize>(recordEnd_ - bufferNext_, size);
  std::copy(bufferNext_, bufferNext_ + toCopy, buffer);
  bufferNext_ += toCopy;
  eor_ = bufferNext_ == recordEnd_ && recordEnd_ < bufferEnd_;
  return toCopy;
}
