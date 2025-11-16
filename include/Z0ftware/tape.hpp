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
#include <vector>

#include <iostream>

class CharStreamTypes {
public:
  using stream_type = std::istream;
  using char_type = stream_type::char_type;
  using traits_type = stream_type::traits_type;
  using int_type = stream_type::int_type;
  using pos_type = stream_type::pos_type;
  using off_type = stream_type::off_type;
};

class Reader : public CharStreamTypes {
public:
  virtual ~Reader() = default;

  virtual std::streamsize read(char_type *s, std::streamsize count) = 0;
  virtual pos_type tellg() const = 0;
  virtual bool eof() const = 0;
  virtual bool fail() const = 0;
};

class IStreamReader : public Reader {
public:
  IStreamReader(stream_type &input) : input_(input) {}

  std::streamsize read(typename CharStreamTypes::char_type *s,
                       std::streamsize count) override {
    input_.read(s, count);
    return input_.gcount();
  };

  pos_type tellg() const override { return input_.tellg(); };

  bool eof() const override { return input_.eof(); };
  bool fail() const override { return input_.fail(); };

protected:
  stream_type &input_;
};

class ReaderMonitor : public Reader {
public:
  ReaderMonitor(Reader &input) : input_(input), base_(input.tellg()) {}

  using read_event_listener_t = std::function<void(
      off_type offset, char *buffer, std::streamsize numRead)>;

  void addReadEventListener(read_event_listener_t listener) {
    listeners_.push_back(listener);
  }

  std::streamsize read(char *buffer, std::streamsize count) override {
    off_type offset = input_.tellg() - base_;
    std::streamsize numRead = input_.read(buffer, count);
    onInputRead(offset, buffer, numRead);
    return numRead;
  }

  pos_type tellg() const override { return input_.tellg(); }

  bool eof() const override { return input_.eof(); }
  bool fail() const override { return input_.fail(); }

protected:
  void onInputRead(off_type offset, char *buffer, std::streamsize numRead) {
    for (auto listener : listeners_) {
      listener(offset, buffer, numRead);
    }
  }

  Reader &input_;
  pos_type base_;
  std::vector<read_event_listener_t> listeners_;
};

class TapeEditStream : public Reader {
public:
  TapeEditStream(Reader &input) : input_(input) {}

  // Replace chars in [first, last) with replacement
  void addEdit(pos_type first, pos_type last, std::string replacement) {
    edits_.insert({first, last, replacement});
  }

  pos_type tellg() const override { return tellg_; }
  bool eof() const override { return input_.eof(); }
  bool fail() const override { return input_.fail(); }

  inline std::streamsize read(char *buffer, std::streamsize count) override {
    if (!initialized_) {
      tellg_ = input_.tellg();
      editIt_ = edits_.begin();
      nextEdit_ = Edit{0, 0, ""};
      initialized_ = true;
      if (editIt_ == edits_.end()) {
        nextEdit_.begin = std::numeric_limits<off_type>::max();
        nextEdit_.end = std::numeric_limits<off_type>::max();
        nextEdit_.replacement = "";
      }
    }

    if (input_.eof() || input_.fail()) {
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

      off_type pos = input_.tellg();
      if (pos < nextEdit_.begin) {
        // Read up to next edit position
        auto readSize = input_.read(
            buffer, std::min(count, std::streamsize(nextEdit_.begin - pos)));
        tellg_ = tellg_ + readSize;
        return readSize;
      }
      while (pos < nextEdit_.end) {
        // Skip over deletion
        auto readSize = input_.read(
            buffer, std::min(count, std::streamsize(nextEdit_.end - pos)));
        pos = input_.tellg();
        nextEdit_.begin = pos;
      }
      if (!nextEdit_.replacement.empty()) {
        auto copySize =
            std::min(count, std::streamsize(nextEdit_.replacement.size()));
        std::copy(nextEdit_.replacement.begin(),
                  nextEdit_.replacement.begin() + copySize, &buffer[0]);
        tellg_ = tellg_ + pos_type(copySize);
        nextEdit_.replacement = nextEdit_.replacement.substr(copySize);
        return copySize;
      }
    }
  }

protected:
  struct Edit {
    off_type begin;
    off_type end;
    std::string replacement;

    friend auto operator<=>(const Edit &e1, const Edit &e2) {
      auto high = (e1.begin <=> e2.begin);
      return 0 == high ? (e1.end <=> e2.end) : high;
    }
  };

  Reader &input_;
  std::set<Edit> edits_;
  std::set<Edit>::iterator editIt_;
  Edit nextEdit_{0, 0, ""};
  bool initialized_{false};
  off_type tellg_{0};
};

// Interface for reading encodings of tapes.
class TapeIRecordStream : public Reader {
public:
  virtual ~TapeIRecordStream() = default;

  // Returns true if already at EOR and positions for next record
  virtual bool nextRecord() = 0;
  // At end of record
  virtual bool isEOR() const = 0;
  // At end of tape
  virtual bool isEOT() const = 0;
  // Problem reading input stream
  virtual bool fail() const = 0;

  virtual pos_type getRecordPos() const = 0;
  // Record number
  virtual size_t getRecordNum() const = 0;
};

// Reads P7B format as records on PierceFuller IBM tapes
//
// Bit 7 is set for first byte of a record
// Bit 6 is parity, odd for binary, even for BCD
// Bits 5-0 are data
//
class P7BIStream : public TapeIRecordStream {

public:
  P7BIStream(Reader &input);

  // Rrturns true if there is a next record, false if not
  bool nextRecord() override;

  bool isEOR() const override {
    return bufferNext_ == recordEnd_ && recordEnd_ < bufferEnd_;
  }
  bool isEOT() const override { return eot_; }
  bool eof() const override { return input_.eof(); }
  bool fail() const override { return error_; }

  // Reads up to size bytes into buffer, not crossing a record boundary
  std::streamsize read(char *buffer, std::streamsize size) override {
    off_type offset = tellg();
    size_t readSize = readInternal(buffer, size);
    return readSize;
  }

  // Position in underlying stream for next read
  pos_type tellg() const override {
    return bufferPos_ + off_type(bufferNext_ - tapeBuffer_.data());
  }

  // Position in underlying stream for start of record
  pos_type getRecordPos() const override { return recordPos_; }
  // 0-based record number
  size_t getRecordNum() const override { return recordNum_; }

protected:
  inline size_t inputRead(char *buffer, size_t size) {
    off_type offset = tellg();
    size_t readSize = input_.read(buffer, size);
    return readSize;
  }

  size_t readInternal(char *buffer, size_t size);

  void fillTapeBuffer();

  // Scan for the next begin of record mark
  void findNextBOR();

  static constexpr size_t bufferSize_ = 1024;

  bool initialized_ = false;
  Reader &input_;
  std::array<char, bufferSize_> tapeBuffer_{0};

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

class ShareReader : public TapeIRecordStream {
public:
  // Positions at the first deck.
  ShareReader(TapeIRecordStream &input) : input_(input) {}

  // Moves to the next deck. Returns true if successful. Otherwise eod() or
  // fail() will be true.
  bool nextDeck();
  // Returns true if end of deck is reached
  bool eod() const;
  // No more decks
  bool eof() const override;
  // Something bad happened
  bool fail() const override;
  // Reads 7-bit from the deck header. Returns 0 at end of header.
  std::streamsize header(char *buffer, std::streamsize count);
  // Reads 7-bit from the deck data. Returns 0 at end of record.
  std::streamsize read(char *buffer, std::streamsize count) override;
  // Moves to the next record in the deck, returning false at the end of the
  // deck.
  bool nextRecord() override;

protected:
  TapeIRecordStream &input_;
};

// Reads a 7-bit tape stream generating events for records
class LowLevelTapeParser : public virtual CharStreamTypes {
public:
  LowLevelTapeParser(TapeIRecordStream &tapeIStream)
      : tapeIStream_(tapeIStream) {}

  void read();
  void stopReading() { reading_ = false; }
  pos_type getRecordPos() const { return tapeIStream_.getRecordPos(); }
  pos_type tellg() const { return tapeIStream_.tellg(); }
  size_t getRecordNum() const { return tapeIStream_.getRecordNum(); }

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