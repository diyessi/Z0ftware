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

// Delegates methods for DELEGATES (extending INTERFACE) to INPUT (extends
// DELEGATES)
template <typename DELEGATES, typename INPUT, typename INTERFACE>
class Delegate;

class Reader : public CharStreamTypes {
public:
  virtual ~Reader() = default;

  virtual std::streamsize read(char_type *s, std::streamsize count) = 0;
  virtual pos_type tellg() const = 0;
  virtual bool eof() const = 0;
  virtual bool fail() const = 0;
};

template <typename INPUT, typename INTERFACE>
class Delegate<Reader, INPUT, INTERFACE> : public INTERFACE {
public:
  Delegate(INPUT &input) : input_(input) {}

  std::streamsize read(typename INPUT::char_type *s,
                       std::streamsize count) override {
    return input_.read(s, count);
  }
  typename INPUT::pos_type tellg() const override { return input_.tellg(); }
  bool eof() const override { return input_.eof(); }
  bool fail() const override { return input_.fail(); }

protected:
  INPUT &input_;
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

template <typename INTERFACE>
class Observer : public Delegate<INTERFACE, INTERFACE, INTERFACE> {
  using delgate_t = Delegate<INTERFACE, INTERFACE, INTERFACE>;

public:
  Observer(INTERFACE &input) : delgate_t(input), base_(input.tellg()) {}

  using read_event_listener_t =
      std::function<void(typename delgate_t::off_type offset, char *buffer,
                         std::streamsize numRead)>;

  void addReadEventListener(read_event_listener_t listener) {
    listeners_.push_back(listener);
  }

  std::streamsize read(char *buffer, std::streamsize count) override {
    typename delgate_t ::off_type offset = delgate_t::tellg() - base_;
    std::streamsize numRead = delgate_t::read(buffer, count);
    onInputRead(offset, buffer, numRead);
    return numRead;
  }

protected:
  void onInputRead(delgate_t::off_type offset, char *buffer,
                   std::streamsize numRead) {
    for (auto listener : listeners_) {
      listener(offset, buffer, numRead);
    }
  }

  delgate_t::pos_type base_;
  std::vector<read_event_listener_t> listeners_;
};

using ReaderObserver = Observer<Reader>;

// This should work on TapeIRecordStream since since record encoding might not
// correspond to bytes in the input
class TapeEditStream : public Delegate<Reader, Reader, Reader> {
public:
  TapeEditStream(Reader &input) : Delegate(input) {}

  // Replace chars in [first, last) with replacement
  void addEdit(pos_type first, pos_type last, std::string replacement) {
    edits_.insert({first, last, replacement});
  }

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

  std::set<Edit> edits_;
  std::set<Edit>::iterator editIt_;
  Edit nextEdit_{0, 0, ""};
  bool initialized_{false};
  off_type tellg_{0};
};

// Interface for reading encodings of tapes.
// Constructs ready to read the first record
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

template <typename INPUT, typename INTERFACE>
class Delegate<TapeIRecordStream, INPUT, INTERFACE>
    : public Delegate<Reader, INPUT, INTERFACE> {
  using delgate_t = Delegate<Reader, INPUT, INTERFACE>;

public:
  using delgate_t::Delegate;

  // Returns true if already at EOR and positions for next record
  bool nextRecord() override { return delgate_t::input_.nextRecord(); }
  // At end of record
  bool isEOR() const override { return delgate_t::input_.isEOR(); };
  // At end of tape
  bool isEOT() const override { return delgate_t::input_.isEOT(); };
  // Problem reading input stream
  bool fail() const override { return delgate_t::input_.fail(); }

  delgate_t::pos_type getRecordPos() const override {
    return delgate_t::input_.getRecordPos();
  };
  // Record number
  size_t getRecordNum() const override {
    return delgate_t::input_.getRecordNum();
  };
};

using TapeIRecordStreamObserver = Observer<TapeIRecordStream>;

// How to tell when the next deck has started? Short symbolic record?
// Records should be multiples of 72/80/84(?)
// BCD/binary is determined by whether majority of chars are even/odd parity
// Deck header is 1 card record
// Deck data is 1 or more multi-card records. Deck header should indicate
// BCD/binary
class ShareReader
    : public Delegate<TapeIRecordStream, TapeIRecordStream, TapeIRecordStream> {
  using delegate_t =
      Delegate<TapeIRecordStream, TapeIRecordStream, TapeIRecordStream>;

public:
  // Positions at the first deck.
  ShareReader(TapeIRecordStream &input) : delegate_t(input) {}

  // Moves to the next deck. Returns true if successful. Otherwise eod() or
  // fail() will be true.
  bool nextDeck() {
    bool haveHeader = input_.nextRecord();
    if (!haveHeader) {
      return false;
    }
    readingHeader_ = true;
    readingRecord_ = false;
    return true;
  }
  // Returns true if end of deck is reached
  bool eod() const;

  // Reads 7-bit from the deck header. Returns 0 at end of header.
  std::streamsize header(char *buffer, std::streamsize count) {
    if (readingHeader_) {
      auto numRead = input_.read(buffer, count);
      if (0 == numRead) {
        readingHeader_ = false;
        readingRecord_ = true;
      }
      return numRead;
    } else {
      return 0;
    }
  }

  // Position in underlying stream for next read
  pos_type tellg() const override { return input_.tellg(); }

  // Position in underlying stream for start of record
  pos_type getRecordPos() const override { return input_.getRecordPos(); }
  // 0-based record number
  size_t getRecordNum() const override { return input_.getRecordNum(); }

  bool isEOR() const override { return input_.isEOR(); }
  bool isEOT() const override { return input_.isEOT(); }

  // Reads 7-bit from the deck data. Returns 0 at end of record.
  std::streamsize read(char *buffer, std::streamsize count) override {
    if (!readingRecord_) {
      return 0;
    }
    auto numRead = input_.read(buffer, count);
    return numRead;
  }
  // Moves to the next record in the deck, returning false at the end of the
  // deck.
  bool nextRecord() override {
    if (readingRecord_) {
      return input_.nextRecord();
    }
    return false;
  }

protected:
  bool readingHeader_{false};
  bool readingRecord_{false};
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