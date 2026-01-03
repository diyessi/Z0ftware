// MIT License
//
// Copyright (c) 2025 Scott Cyphers
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

// https://www.piercefuller.com/oldibm-shadow/709x.html
// https://www.piercefuller.com/library/magtape7.html

#include "Z0ftware/charset.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/p7bistream.hpp"
#include "Z0ftware/sharereader.hpp"
#include "Z0ftware/tape.hpp"
#include "Z0ftware/tapeeditstream.hpp"
#include "Z0ftware/utils.hpp"

#include "llvm/Support/CommandLine.h"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {
llvm::cl::list<std::string> inputFileNames(llvm::cl::Positional,
                                           llvm::cl::desc("<Input files>"),
                                           llvm::cl::OneOrMore);

llvm::cl::opt<bool> dumpInputReads("dump-input-reads",
                                   llvm::cl::desc("dump file reads"),
                                   llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpOffsetEditInputs("dump-offset-edit-inputs",
                         llvm::cl::desc("dump edit input reads"),
                         llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpOffsetEditOutputds("dump-offset-edit-outputs",
                           llvm::cl::desc("dump edit output reads"),
                           llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpRecordOffsetEditInputs("dump-record-offset-edit-inputs",
                               llvm::cl::desc("dump edit input reads"),
                               llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpRecordOffsetEditOutputs("dump-record-offset-edit-outputs",
                                llvm::cl::desc("dump edit output reads"),
                                llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpP7BInputReads("dump-p7b-input-reads",
                      llvm::cl::desc("dump read p7b input reads"),
                      llvm::cl::init(false));

llvm::cl::opt<bool> dumpP7BOutputReads("dump-p7b-output-reads",
                                       llvm::cl::desc("dump p7b output reads"),
                                       llvm::cl::init(false));

llvm::cl::opt<bool> listFiles("list", llvm::cl::desc("list files on tape"),
                              llvm::cl::init(false));

llvm::cl::opt<bool> showHeaders("show-headers",
                                llvm::cl::desc("Show record headers"),
                                llvm::cl::init(false));

llvm::cl::opt<bool> showDeck("show-deck", llvm::cl::desc("Show deck number"),
                             llvm::cl::init(false));

llvm::cl::opt<bool>
    showCardNumber("show-card-number",
                   llvm::cl::desc("Show number card within deck"),
                   llvm::cl::init(false));

llvm::cl::list<unsigned>
    deckNumbers("deck-number", llvm::cl::desc("Only show specified deck"));

llvm::cl::opt<bool> showTapePos("show-tape-pos",
                                llvm::cl::desc("Show tape position"));

llvm::cl::opt<std::string> edits("edits",
                                 llvm::cl::desc("Edits for tape file"));

} // namespace

static auto hexDump(std::string title, size_t byteGroupSize, size_t lineSize) {
  return [title, lineSize, byteGroupSize](Reader::pos_type pos, char *buffer,
                                          std::streamsize count) {
    std::cout << "*** " << title << ": " << pos << ":" << count << std::endl;
    for (size_t i = 0; i < count; ++i) {
      if (i > 0) {
        if (0 == i % lineSize) {
          std::cout << '\n';
        } else if (0 == i % byteGroupSize) {
          std::cout << ' ';
        }
      }
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << uint16_t(uint8_t(buffer[i]));
    }
    std::cout << "\n";
  };
};

static auto octDump(std::string title, size_t charGroupSize, size_t lineSize) {
  return [title, charGroupSize, lineSize](P7BIStream::pos_type pos,
                                          char *buffer, size_t size) {
    std::cout << "*** " << title << ": " << pos << ":" << size << std::endl;
    for (size_t i = 0; i < size; ++i) {
      if (i > 0) {
        if (0 == i % lineSize) {
          std::cout << '\n';
        } else if (0 == i % charGroupSize) {
          std::cout << ' ';
        }
      }
      std::cout << std::oct << std::setw(2) << std::setfill('0')
                << uint16_t(buffer[i] & 0x3F);
    }
    std::cout << "\n";
  };
};

static auto noteRead(std::string title) {
  return [title](P7BIStream::pos_type pos, char *buffer, size_t size) {
    std::cout << "*** " << title << ": " << pos << ":" << size << std::endl;
  };
};

int main(int argc, const char **argv) {
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "Version " << Z0ftware_VERSION_MAJOR << "." << Z0ftware_VERSION_MINOR
       << "." << Z0ftware_VERSION_PATCH << "\n";
  });

  llvm::cl::ParseCommandLineOptions(
      argc, argv,
      "SHARE tape extractor for IBM 704\n\n"
      "  This program extracts information from SHARE tapes.\n");

  std::setlocale(LC_ALL, "");

  for (auto &inputFileName : inputFileNames) {
    std::ifstream input(inputFileName,
                        std::ifstream::binary | std::ifstream::in);
    if (!input.is_open()) {
      std::cerr << "Could not open " << inputFileName << "\n";
      continue;
    }

    IStreamReader iStreamReader(input);
    Reader *reader = &iStreamReader;

    std::unique_ptr<ReaderObserver> inputReadObserver;
    if (dumpInputReads) {
      inputReadObserver = std::make_unique<ReaderObserver>(*reader);
      reader = inputReadObserver.get();
      inputReadObserver->addReadEventListener(hexDump("Input", 4, 64));
    }

    json editObj;
    if (!edits.empty()) {
      std::ifstream editsFile(edits);
      editObj = json::parse(editsFile);
    }

    std::unique_ptr<ReaderEditor> offsetEditor;
    std::unique_ptr<ReaderObserver> offsetEditorInputObserver;
    std::unique_ptr<ReaderObserver> offsetEditorOutputObserver;
    if (!edits.empty()) {
      if (dumpOffsetEditInputs) {
        offsetEditorInputObserver = std::make_unique<ReaderObserver>(*reader);
        reader = offsetEditorInputObserver.get();
        offsetEditorInputObserver->addReadEventListener(
            noteRead("Edit offset input"));
      }

      auto &readerEditList = editObj["offsets"];
      if (!readerEditList.empty()) {
        offsetEditor = std::make_unique<ReaderEditor>(*reader);
        reader = offsetEditor.get();
        for (auto &editItem : readerEditList) {
          size_t start = editItem[0];
          size_t end = editItem[1];
          std::string replacement = editItem[2];
          offsetEditor->addEdit(start, end, replacement);
        }
      }

      if (dumpOffsetEditOutputds) {
        offsetEditorOutputObserver = std::make_unique<ReaderObserver>(*reader);
        reader = offsetEditorOutputObserver.get();
        offsetEditorOutputObserver->addReadEventListener(
            noteRead("Edit offset output"));
      }
    }

    std::unique_ptr<ReaderObserver> p7BInputReadObserver;
    std::unique_ptr<TapeIRecordStreamObserver> p7BOutputReadObserver;

    if (dumpP7BInputReads) {
      p7BInputReadObserver = std::make_unique<ReaderObserver>(*reader);
      reader = p7BInputReadObserver.get();
      p7BInputReadObserver->addReadEventListener(octDump("P7B Input", 6, 72));
    }

    std::unique_ptr<P7BIStream> p7biStream =
        std::make_unique<P7BIStream>(*reader);
    TapeIRecordStream *tapeReader = p7biStream.get();

    if (dumpP7BOutputReads) {
      p7BOutputReadObserver =
          std::make_unique<TapeIRecordStreamObserver>(*tapeReader);
      tapeReader = p7BOutputReadObserver.get();
      p7BOutputReadObserver->addReadEventListener(octDump("P7B Output", 6, 72));
    }

    std::unique_ptr<TapeIRecordStreamEditor> recordOffsetEditor;
    std::unique_ptr<TapeIRecordStreamObserver> recordOffsetEditorInputObserver;
    std::unique_ptr<TapeIRecordStreamObserver> recordOffsetEditorOutputObserver;
    if (!edits.empty()) {
      auto &tapeIRecordEditList = editObj["record-offsets"];
      if (!tapeIRecordEditList.empty()) {

        if (dumpRecordOffsetEditInputs) {
          recordOffsetEditorInputObserver =
              std::make_unique<TapeIRecordStreamObserver>(*tapeReader);
          tapeReader = recordOffsetEditorInputObserver.get();
          recordOffsetEditorInputObserver->addReadEventListener(
              noteRead("Edit record input"));
        }

        recordOffsetEditor =
            std::make_unique<TapeIRecordStreamEditor>(*tapeReader);
        tapeReader = recordOffsetEditor.get();
        for (auto &editItem : tapeIRecordEditList) {
          size_t recordNum = editItem[0];
          size_t start = editItem[1];
          size_t end = editItem[2];
          std::string replacement = editItem[3];
          recordOffsetEditor->addEdit(recordNum, start, end, replacement);
        }

        if (dumpRecordOffsetEditOutputs) {
          recordOffsetEditorOutputObserver =
              std::make_unique<TapeIRecordStreamObserver>(*tapeReader);
          tapeReader = recordOffsetEditorOutputObserver.get();
          recordOffsetEditorOutputObserver->addReadEventListener(
              noteRead("Edit record output"));
        }
      }
    }

    std::unique_ptr<parity_glyphs_t> tapeChars =
        collateGlyphCardTape.getTapeCharset(true);
    size_t cardNumber = 0;
    ShareReader shareReader(*tapeReader);

    auto showPosition = [&shareReader, &cardNumber](Reader::pos_type offset) {
      if (showTapePos) {
        std::cout << std::setw(12) << std::setfill('0')
                  << shareReader.getRecordPos() + offset << " ";
      }
      if (showCardNumber) {
        std::cout << std::setw(4) << std::setfill('0')
                  << shareReader.getRecordNum() << ":" << std::setw(4)
                  << std::setfill('0') << cardNumber << " ";
      }
    };

    char buffer[4096];
    while (!shareReader.eof()) {
      cardNumber = 0;
      // Deck header
      std::string_view header = shareReader.getDeckHeader();
      auto lineSize = header.size();

      std::ostringstream ostream;
      for (auto &it : header) {
        ostream << tapeChars->at(it);
      }
      auto view = ostream.view();

      // Identification for next library file
      auto classification = view.substr(0, view.find(' ', 0));
      auto installation = view.substr(3, view.find(' ', 3) - 3);
      auto name = view.substr(6, view.find(' ', 6) - 6);
      auto id = view.substr(20, view.find(' ', 20) - 20);
      auto format = view.substr(33, 2);

      bool showThisDeck =
          deckNumbers.empty() ||
          std::find(deckNumbers.begin(), deckNumbers.end(),
                    shareReader.getDeckNum()) != deckNumbers.end();

      if (showThisDeck) {
        std::cout << "===========\n";

        std::cout << view << "\n";
      }
      std::ostringstream deckName;
      deckName << std::setw(4) << std::setfill('0') << shareReader.getDeckNum();
      if (!classification.empty()) {
        deckName << "-" << classification;
      }
      if (!installation.empty()) {
        deckName << "-" << installation;
      }
      deckName << "-" << name;
      if (!id.empty()) {
        deckName << "-" << id;
      }
      deckName << "." << format;
      std::string deckNameStr = deckName.str();
      if (showThisDeck) {
        std::cout << "Current deck: " << shareReader.getDeckNum() << " '"
                  << deckName.str() << "'\n";
        std::cout << "Classification: '" << classification << "' Company: '"
                  << installation << "' Name: '" << name << "' Id: '" << id
                  << "' Format: '" << format << "'"
                  << "\n";
        std::cout << "===========\n";
      }

      while (true) {
        auto size = shareReader.read(buffer, sizeof(buffer));
        if (size == 0) {
          break;
        }
        if (shareReader.isBinary()) {
          if (showThisDeck) {
            showPosition(0);
            std::cout << "Binary\n";
          }
          cardNumber += size / lineSize;
        } else {
          std::ostringstream ostream;
          ShareReader::pos_type linePos = 0;
          size_t pos = 0;

          for (char *it = &buffer[0]; it < &buffer[size]; ++it) {
            ostream << tapeChars->at(*it);
            if (lineSize == ++pos) {
              auto view = ostream.view();
              pos = 0;
              if (view.end() != std::find_if(view.begin(), view.end(),
                                             [](char c) { return c != ' '; })) {

                if (showThisDeck) {
                  showPosition(linePos);
                  std::cout << view << "\n";
                }
              }

              cardNumber++;
              linePos += lineSize;
              ostream.str("");
            }
          }
        }
      }
      if (!shareReader.nextDeck()) {
        return 0;
      }
    }

    input.close();
  }

  return EXIT_SUCCESS;
}