// MIT License
//
// Copyright (c) 2026 Scott Cyphers
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

#ifndef Z0FTWARE_TAPEEDITSTREAM_HPP
#define Z0FTWARE_TAPEEDITSTREAM_HPP

#include "Z0ftware/tape.hpp"

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

#endif
