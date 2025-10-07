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
#include <istream>
#include <vector>

// Interface for reading encodings of tapes.
class TapeIRecordStream {
public:
  using stream_type = std::istream;
  using char_type = stream_type::char_type;
  using traits_type = stream_type::traits_type;
  using int_type = stream_type::int_type;
  using pos_type = stream_type::pos_type;
  using off_type = stream_type::off_type;

  // Returns true if already at EOR and positions for next record
  virtual bool nextRecord() = 0;
  // At end of record
  virtual bool isEOR() const = 0;
  // At end of tape
  virtual bool isEOT() const = 0;
  // Problem reading input stream
  virtual bool isError() const = 0;

  // Returns 0 at end of record
  virtual size_t read(char *buffer, size_t size) = 0;
  // Position for next read
  virtual pos_type tellg() const = 0;
  // Start of record
  virtual pos_type getRecordPos() const = 0;
  // Record number
  virtual pos_type getRecordNum() const = 0;
};

// Reads P7B format on PierceFuller IBM tapes
//
// Bit 7 is set for first byte of a record
// Bit 6 is parity, odd for binary, even for BCD
// Bits 5-0 are data
//
class P7BIStream : public TapeIRecordStream {

public:
  P7BIStream(std::istream &input);
  bool nextRecord() override;

  bool isEOR() const override {
    return tapeBufferPos_ == tapeBORPos_ && tapeBORPos_ != tapeBufferEnd_;
  }
  bool isEOT() const override { return eot_; }
  bool isError() const override { return error_; }

  size_t read(char *buffer, size_t size) override;
  pos_type tellg() const override { return input_.tellg(); }
  pos_type getRecordPos() const override { return recordPos_; }
  pos_type getRecordNum() const override { return recordNum_; }

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

  pos_type bufferPos_{0};
  pos_type recordPos_{0};
  pos_type nextRecordPos_{0};
  pos_type recordNum_{0};
};

class TapeEdit {
public:
  virtual ~TapeEdit() = default;
  virtual void apply(size_t offset, std::vector<char> &record) const = 0;
};

// Forms full records and categorizes as binary/BCD.
// TODO: Perform position-based edits to correct tape read errors here or in the
// input stream.
class TapeReadAdapter {
public:
  using stream_type = TapeIRecordStream;
  using char_type = stream_type::char_type;
  using traits_type = stream_type::traits_type;
  using int_type = stream_type::int_type;
  using pos_type = stream_type::pos_type;
  using off_type = stream_type::off_type;

  TapeReadAdapter(stream_type &tapeIStream) : tapeIStream_(tapeIStream) {}

  void read();
  void stopReading() { reading_ = false; }
  pos_type getRecordPos() const { return tapeIStream_.getRecordPos(); }
  pos_type tellg() const { return tapeIStream_.tellg(); }
  size_t getRecordNum() const { return tapeIStream_.getRecordNum(); }

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
  std::vector<char> record_;
};

#endif