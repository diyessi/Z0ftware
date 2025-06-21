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

#ifndef Z0FTWARE_TAPE_HPP
#define Z0FTWARE_TAPE_HPP

#include <array>
#include <cstddef>
#include <vector>

class TapeIRecordStream {
public:
  // Returns 0 at end of record
  virtual size_t read(char *buffer, size_t size) = 0;
  // Returns true if already at EOR and positions for next record
  virtual bool nextRecord() = 0;
  // At end of record
  virtual bool isEOR() const = 0;
  // At end of tape
  virtual bool isEOT() const = 0;
  // Problem reading input stream
  virtual bool isError() const = 0;
};

// Reads P7B format on PierceFuller IBM tapes
//
// Bit 7 is set for first byte of a record
// Bit 6 is parity, odd for binary, even for BCD
// Bits 5-0 are data
class P7BIStream : public TapeIRecordStream {

public:
  P7BIStream(std::istream &input);
  bool nextRecord() override;
  size_t read(char *buffer, size_t size) override;

  bool isEOR() const override {
    return tapeBufferPos_ == tapeBORPos_ && tapeBORPos_ != tapeBufferEnd_;
  }
  bool isEOT() const override { return eot_; }
  bool isError() const override { return error_; }

protected:
  void fillTapeBuffer();

  // Scan for the next begin of record mark
  void findNextBOR();

  std::istream &input_;
  std::array<char, 1024> tapeBuffer_{0};
  char *tapeBufferPos_{tapeBuffer_.data()};
  char *tapeBufferEnd_{tapeBufferPos_};
  char *tapeBORPos_{nullptr};

  bool eot_{false};
  bool error_{false};
};

class TapeReadAdapter {
public:
  TapeReadAdapter(TapeIRecordStream &tapeIStream) : tapeIStream_(tapeIStream) {}

  void read();
  void stopReading() { reading_ = false; }
  size_t getRecordStartPos() const { return recordStartPos_; }
  size_t getTapePos() const { return tapePos_; }

  // Events

  // Buffer read from input
  // pos is file position
  virtual void onRead(size_t pos, char *buffer, size_t size) {}
  virtual void onRecordData(char *buffer, size_t size) {}
  virtual void onBinaryRecordData() {}
  virtual void onBCDRecordData() {}
  virtual void onBeginOfRecord() {}
  virtual void onEndOfRecord() {}
  virtual void onEndOfFile() {}
  virtual void onEndOfTape() {}

protected:
  TapeIRecordStream &tapeIStream_;
  bool reading_{true};
  size_t recordStartPos_{0};
  size_t tapePos_{0};
  std::vector<char> record_;
};

#endif