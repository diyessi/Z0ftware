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
#include <functional>
#include <istream>
#include <set>
#include <utility>
#include <vector>

#include <iostream>

class InputReadEventGenerator {
public:
  using off_type = std::istream::off_type;
  using read_event_listener_t =
      std::function<void(off_type offset, char *buffer, size_t size)>;

  void addInputReadEventListener(read_event_listener_t listener) {
    listeners_.push_back(listener);
  }

protected:
  void onInputRead(off_type offset, char *buffer, size_t size) {
    for (auto listener : listeners_) {
      listener(offset, buffer, size);
    }
  }

  std::vector<read_event_listener_t> listeners_;
};

class OutputReadEventGenerator {
public:
  using off_type = std::istream::off_type;
  using read_event_listener_t =
      std::function<void(off_type offset, char *buffer, size_t size)>;

  void addOutputReadEventListener(read_event_listener_t listener) {
    listeners_.push_back(listener);
  }

protected:
  void onOutputRead(off_type offset, char *buffer, size_t size) {
    for (auto listener : listeners_) {
      listener(offset, buffer, size);
    }
  }

  std::vector<read_event_listener_t> listeners_;
};

// Interface for reading encodings of tapes.
class TapeIRecordStream {
public:
  using stream_type = std::istream;
  using char_type = stream_type::char_type;
  using traits_type = stream_type::traits_type;
  using int_type = stream_type::int_type;
  using pos_type = stream_type::pos_type;
  using off_type = stream_type::off_type;

  virtual ~TapeIRecordStream() = default;

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
  // Start of tape
  virtual pos_type getTapePos() const = 0;
  // Start of record position
  virtual pos_type getRecordPos() const = 0;
  // Record position relative to tape start
  virtual off_type getRecordOffset() const {
    return getRecordPos() - getTapePos();
  }
  // Offset for next read
  virtual off_type getOffset() const { return tellg() - getTapePos(); }
  // Record number
  virtual size_t getRecordNum() const = 0;
};

class DelegateTapeIRecordStream : public TapeIRecordStream {
public:
  template <typename T>
  DelegateTapeIRecordStream(T &&input) : input_(std::move(input)) {}

  bool nextRecord() override { return input_->nextRecord(); };
  // At end of record
  bool isEOR() const override { return input_->isEOR(); };
  // At end of tape
  bool isEOT() const override { return input_->isEOT(); };
  // Problem reading input stream
  bool isError() const override { return input_->isError(); };

  // Returns 0 at end of record
  size_t read(char *buffer, size_t size) override {
    return input_->read(buffer, size);
  };
  // Position for next read
  pos_type tellg() const override { return input_->tellg(); };
  // Start of tape
  pos_type getTapePos() const override { return input_->getTapePos(); };
  // Start of record position
  pos_type getRecordPos() const override { return input_->getRecordPos(); };
  // Record position relative to tape start
  off_type getRecordOffset() const override {
    return input_->getRecordOffset();
  }
  // Offset for next read
  off_type getOffset() const override { return input_->getOffset(); }
  // Record number
  size_t getRecordNum() const override { return input_->getRecordNum(); }

protected:
  std::unique_ptr<TapeIRecordStream> input_;
};

// Reads P7B format as records on PierceFuller IBM tapes
//
// Bit 7 is set for first byte of a record
// Bit 6 is parity, odd for binary, even for BCD
// Bits 5-0 are data
//
class P7BIStream : public TapeIRecordStream,
                   public InputReadEventGenerator,
                   public OutputReadEventGenerator {

public:
  P7BIStream(std::istream &input);

  // Rrturns true if there is a next record, false if not
  bool nextRecord() override;

  bool isEOR() const override {
    return bufferNext_ == recordEnd_ && recordEnd_ < bufferEnd_;
  }
  bool isEOT() const override { return eot_; }
  bool isError() const override { return error_; }

  // Reads up to size bytes into buffer, not crossing a record boundary
  size_t read(char *buffer, size_t size) override {
    off_type offset = getOffset();
    size_t readSize = readInternal(buffer, size);
    onOutputRead(offset, buffer, readSize);
    return readSize;
  }

  // Position in underlying stream for next read
  pos_type tellg() const override {
    return bufferPos_ + off_type(bufferNext_ - tapeBuffer_.data());
  }
  // Position in underlying stream for tape start
  pos_type getTapePos() const override { return tapePos_; }
  // Position in underlying stream for start of record
  pos_type getRecordPos() const override { return recordPos_; }
  // 0-based record number
  size_t getRecordNum() const override { return recordNum_; }

protected:
  inline size_t inputRead(char *buffer, size_t size) {
    off_type offset = getOffset();
    input_.read(buffer, size);
    size_t readSize = input_.gcount();
    onInputRead(offset, buffer, readSize);
    return readSize;
  }

  size_t readInternal(char *buffer, size_t size);

  void fillTapeBuffer();

  // Scan for the next begin of record mark
  void findNextBOR();

  static constexpr size_t bufferSize_ = 1024;

  bool initialized_ = false;
  std::istream &input_;
  std::array<char, bufferSize_> tapeBuffer_{0};

  // Position in underlying stream for start of tape
  pos_type tapePos_;

  // Next buffer char to use
  char *bufferNext_{tapeBuffer_.data()};
  // Pos of next buffer to use
  pos_type bufferPos_;
  // Last valid char in buffer
  char *bufferEnd_{tapeBuffer_.data()};
  // Last char in record if less than bufferEnd_;
  char *recordEnd_;
  pos_type recordPos_;

  bool eot_{false};
  bool error_{false};
  size_t recordNum_{0};
};

class TapeEditStream : public DelegateTapeIRecordStream,
                       public InputReadEventGenerator,
                       public OutputReadEventGenerator {
public:
  using DelegateTapeIRecordStream::DelegateTapeIRecordStream;

  // Replace chars in [first, last) with replacement
  void addEdit(pos_type first, pos_type last, std::string replacement) {
    edits_.insert({first, last, replacement});
  }

  size_t read(char *buffer, size_t size) override {
    off_type offset = getOffset();
    size_t readSize = readInternal(buffer, size);
    onOutputRead(offset, buffer, readSize);
    return readSize;
  }

  pos_type tellg() const override { return tellg_; }

  pos_type tellgBase() const { return input_->tellg(); }

protected:
  size_t inputRead(char *buffer, size_t size) {
    off_type offset = input_->getOffset();
    size_t readSize = input_->read(buffer, size);
    onInputRead(offset, buffer, readSize);
    return readSize;
  }

  inline size_t readInternal(char *buffer, size_t size) {
    if (!initialized_) {
      tellg_ = input_->tellg();
      editIt_ = edits_.begin();
      nextEdit_ = Edit{0, 0, ""};
      initialized_ = true;
      if (editIt_ == edits_.end()) {
        nextEdit_.begin = std::numeric_limits<off_type>::max();
        nextEdit_.end = std::numeric_limits<off_type>::max();
        nextEdit_.replacement = "";
      }
    }

    if (input_->isEOT() || input_->isError()) {
      return 0;
    }

    while (true) {
      while (nextEdit_.begin == nextEdit_.end &&
             nextEdit_.replacement.empty()) {
        if (editIt_ == edits_.end()) {
          nextEdit_.begin = std::numeric_limits<off_type>::max();
          nextEdit_.end = std::numeric_limits<off_type>::max();
          nextEdit_.replacement = "";
          break;
        } else {
          nextEdit_ = *editIt_++;
        }
      }

      off_type pos = tellgBase();
      if (pos < nextEdit_.begin) {
        // Read up to next edit position
        auto readSize =
            inputRead(buffer, std::min(size, size_t(nextEdit_.begin - pos)));
        tellg_ = tellg_ + readSize;
        return readSize;
      }
      while (pos < nextEdit_.end) {
        // Skip over deletion
        auto readSize =
            inputRead(buffer, std::min(size, size_t(nextEdit_.end - pos)));
        pos = tellgBase();
        nextEdit_.begin = pos;
      }
      if (!nextEdit_.replacement.empty()) {
        auto copySize = std::min(size, nextEdit_.replacement.size());
        std::copy(nextEdit_.replacement.begin(),
                  nextEdit_.replacement.begin() + copySize, &buffer[0]);
        tellg_ = tellg_ + pos_type(copySize);
        nextEdit_.replacement = nextEdit_.replacement.substr(copySize);
        return copySize;
      }
    }
  }

  struct Edit {
    off_type begin;
    off_type end;
    std::string replacement;

    friend auto operator<=>(const Edit &e1, const Edit &e2) {
      auto high = (e1.begin <=> e2.begin);
      return 0 == high ? (e1.end <=> e2.end) : high;
    }
  };

  std::set<Edit> edits_;
  std::set<Edit>::iterator editIt_;
  Edit nextEdit_{0, 0, ""};
  bool initialized_{false};
  off_type tellg_{0};
};

// Reads a 7-bit tape stream generating events for records
class LowLevelTapeParser {
public:
  using stream_type = TapeIRecordStream;
  using char_type = stream_type::char_type;
  using traits_type = stream_type::traits_type;
  using int_type = stream_type::int_type;
  using pos_type = stream_type::pos_type;
  using off_type = stream_type::off_type;

  LowLevelTapeParser(stream_type &tapeIStream) : tapeIStream_(tapeIStream) {}

  void read();
  void stopReading() { reading_ = false; }
  pos_type getRecordPos() const { return tapeIStream_.getRecordPos(); }
  off_type getRecordOffset() const { return tapeIStream_.getRecordOffset(); }
  pos_type tellg() const { return tapeIStream_.tellg(); }
  size_t getRecordNum() const { return tapeIStream_.getRecordNum(); }
  off_type getOffset() const { return tapeIStream_.getOffset(); }

  // Events

  // Buffer read from input
  // pos is file position
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