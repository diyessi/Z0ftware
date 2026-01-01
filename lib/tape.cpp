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

void LowLevelTapeParser::read() {
  char buffer[40];
  reading_ = true;
  onBeginOfRecord();
  while (reading_) {
    auto pos = tellg();
    size_t size = tapeIStream_.read(buffer, sizeof(buffer));
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
