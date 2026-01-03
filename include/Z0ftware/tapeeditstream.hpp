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
class ReaderEditor : public Delegate<Reader, Reader, Reader> {
public:
  ReaderEditor(Reader &input) : Delegate(input) {}

  // Replace chars in [begin, end) with replacement
  void addEdit(pos_type begin, pos_type end, std::string replacement);

  std::streamsize read(char *buffer, std::streamsize count) override;

protected:
  struct Edit {
    off_type begin{0};
    off_type end{0};
    std::string replacement{""};

    friend auto operator<=>(const Edit &e1, const Edit &e2) {
      auto high = (e1.begin <=> e2.begin);
      return 0 == high ? (e1.end <=> e2.end) : high;
    }
  };

  std::set<Edit> edits_;
  std::set<Edit>::iterator editIt_;
  Edit nextEdit_{.begin = 0, .end = 0, .replacement = ""};
  bool initialized_{false};
  off_type tellg_{0};
};

class TapeIRecordStreamEditor
    : public Delegate<TapeIRecordStream, TapeIRecordStream, TapeIRecordStream> {
public:
  TapeIRecordStreamEditor(TapeIRecordStream &input) : Delegate(input) {}

  // Replace [first, last) in recordNum with replacement
  void addEdit(size_t recordNum, pos_type begin, pos_type end,
               std::string replacement);

  std::streamsize read(char *buffer, std::streamsize count) override;

protected:
  struct Edit {
    size_t recordNum{0};
    off_type begin{0};
    off_type end{0};
    std::string replacement{""};

    friend auto operator<=>(const Edit &e1, const Edit &e2) {
      auto record = (e1.recordNum <=> e2.recordNum);
      auto high = 0 == record ? (e1.begin <=> e2.begin) : record;
      return 0 == high ? (e1.end <=> e2.end) : high;
    }
  };

  std::set<Edit> edits_;
  std::set<Edit>::iterator editIt_;
  Edit nextEdit_{.recordNum = 0, .begin = 0, .end = 0, .replacement = ""};
  bool initialized_{false};
  off_type tellg_{0};
};

#endif
