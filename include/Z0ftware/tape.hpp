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

#include "Z0ftware/utils.hpp"

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

#endif