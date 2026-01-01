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

#ifndef Z0FTWARE_T7BISTREAM_HPP
#define Z0FTWARE_T7BISTREAM_HPP

#include "Z0ftware/tape.hpp"

// Reads P7B format as records on PierceFuller IBM tapes
//
// Bit 7 is set for first byte of a record
// Bit 6 is parity, odd for binary, even for BCD
// Bits 5-0 are data
//
class P7BIStream : public Delegate<Reader, Reader, TapeIRecordStream> {

public:
  P7BIStream(Reader &input);

  // Rrturns true if there is a next record, false if not
  bool nextRecord() override;

  bool isEOR() const override { return eor_; }
  bool isEOT() const override { return eot_; }

  // Reads up to size bytes into buffer, not crossing a record boundary
  std::streamsize read(char *buffer, std::streamsize size) override;

  // Position in underlying stream for next read
  pos_type tellg() const override {
    return input_.tellg() - off_type(bufferEnd_ - bufferNext_);
  }

  // Position in underlying stream for start of record
  pos_type getRecordPos() const override { return recordPos_; }
  // 0-based record number
  size_t getRecordNum() const override { return recordNum_; }

protected:
  void fillTapeBuffer();

  // Scan for the next begin of record mark
  void findNextBOR();

  bool initialized_ = false;

  static constexpr size_t bufferSize_ = 1024;
  std::array<char, bufferSize_> tapeBuffer_{0};

  // Next buffer char to use
  char *bufferNext_;
  // Last valid char in buffer
  char *bufferEnd_;
  // Last char in record if less than bufferEnd_;
  char *recordEnd_;

  pos_type recordPos_;

  bool eor_{false};
  bool eot_{false};
  size_t recordNum_{0};
};

#endif
