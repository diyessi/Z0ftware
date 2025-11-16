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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/charset.hpp"
#include "Z0ftware/config.h"
#include "Z0ftware/tape.hpp"

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

llvm::cl::opt<bool> dumpEditInputReads("dump-edit-input-reads",
                                       llvm::cl::desc("dump edit input reads"),
                                       llvm::cl::init(false));

llvm::cl::opt<bool>
    dumpEditOutputReads("dump-edit-output-reads",
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

// Share tapes encode card decks on IBM70x tapes.
// Record lengths are multiples of:
// - 72 if IBM704 was used, which could only read/write 72 characters/card.
// - 80 if the full card is stored
// - 84? Is this actually used?
//
// Each deck consists of a one-card record indicating brief information about
// the deck and what format the deck is in.
// The deck consists of one or more multi-card records in the format specified
// in the header. Blank cards are used at the end if needed to pad to the record
// length.
// Interface for IBM 70x style tapes with decks encoded as 7-bit
// odd-parity of 6-bit values and BCD (character) records encoded as 7-bit even
// parity of 6-bit characters. An one card deck identifier record precedes each
// deck, which consists of one or more records or multiple cards each. Blank
// cards pad the record to uniform size.
class ShareExtractor : public LowLevelTapeParser {
public:
  ShareExtractor(TapeIRecordStream &tapeIStream, const CharsetForTape &charSet)
      : LowLevelTapeParser(tapeIStream),
        tapeChars_(charSet.getTapeCharset(true)) {
    // Show character set
    for (bcd_t i = bcd_t::min(); i <= bcd_t::max(); ++i) {
      std::cout << std::setw(2) << std::setfill('0') << i.value() << " "
                << std::oct << std::setw(3) << std::setfill('0')
                << evenParity(i.value()).value() << " " << std::setw(1)
                << std::dec << tapeChars_->at(evenParity(i.value()).value())
                << " " << tapeChars_->at(oddParity(i.value()).value()) << "\n";
    }
    std::cout << std::endl;
  }

  size_t getCurrentDeck() { return nextDeck_ - 1; }
  size_t getCardNumber() { return cardNumber_; }

  void onRecordData(char *buffer, size_t size) override {}
  void onBinaryRecordData() override {
    if (showThisDeck_) {
      showPosition();
      std::cout << "Binary\n";
    }
  }
  void onBCDRecordData() override {
    if (showHeaders_) {
      std::cout << "Record: " << getRecordNum() << " BCD: " << record_.size()
                << "\n";
    }
    if (record_.size() <= 84) {
      // Deck header
      lineSize_ = record_.size();

      std::ostringstream ostream;
      for (auto &it : record_) {
        ostream << tapeChars_->at(it);
      }
      auto view = ostream.view();

      // Identification for next library file
      nextDeck_++;
      deckName_ = "";
      showThisDeck_ = deckNumbers_.empty() ||
                      std::find(deckNumbers_.begin(), deckNumbers_.end(),
                                getCurrentDeck()) != deckNumbers_.end();
      if (!showThisDeck_) {
        return;
      }
      auto classification = view.substr(0, view.find(' ', 0));
      auto installation = view.substr(3, view.find(' ', 3) - 3);
      auto name = view.substr(6, view.find(' ', 6) - 6);
      auto id = view.substr(20, view.find(' ', 20) - 20);
      auto format = view.substr(33, 2);
      std::cout << "===========\n";
      showPosition();

      std::cout << view << "\n";
      std::ostringstream deckName;
      deckName << std::setw(4) << std::setfill('0') << getCurrentDeck();
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
      deckName_ = deckName.str();

      std::cout << "Current deck: " << getCurrentDeck() << " '" << deckName_
                << "'\n";
      std::cout << "Classification: '" << classification << "' Company: '"
                << installation << "' Name: '" << name << "' Id: '" << id
                << "' Format: '" << format << "'"
                << "\n";
      std::cout << "===========\n";
      cardNumber_ = 0;

      return;
    }

    std::ostringstream ostream;
    pos_type linePos = 0;
    size_t pos = 0;

    for (auto &it : record_) {
      ostream << tapeChars_->at(it);
      if (lineSize_ == ++pos) {
        auto view = ostream.view();
        pos = 0;
        if (view.end() != std::find_if(view.begin(), view.end(),
                                       [](char c) { return c != ' '; })) {

          if (showThisDeck_) {
            showPosition();
            std::cout << view << "\n";
          }
        }

        cardNumber_++;
        linePos += lineSize_;
        ostream.str("");
      }
    }
    if (showHeaders_) {
      std::cout << std::endl;
    }
  }
  void onBeginOfRecord() override {}
  void onEndOfRecord() override {}
  void onEndOfFile() override {}
  void onEndOfTape() override {}

protected:
  void showPosition() {
    if (showTapePos_) {
      std::cout << std::setw(12) << std::setfill('0') << getRecordPos() << " ";
    }
    if (showCardNumber_) {
      std::cout << std::setw(4) << std::setfill('0') << getCardNumber() << " ";
    }
  }

  std::unique_ptr<parity_glyphs_t> tapeChars_;
  bool showHeaders_{showHeaders};
  bool showCardNumber_{showCardNumber};
  size_t nextDeck_{0};
  std::string deckName_;
  bool showDeck_{showDeck};
  bool showThisDeck_{true};
  size_t cardNumber_{0};
  bool showTapePos_{showTapePos};
  std::vector<unsigned> deckNumbers_{deckNumbers};
  size_t lineSize_{0};
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

    auto hexDump = [](std::string title, size_t byteGroupSize,
                      size_t lineSize) {
      return [title, lineSize, byteGroupSize](
                 Reader::pos_type pos, char *buffer, std::streamsize count) {
        std::cout << "*** " << title << ": " << pos << ":" << count
                  << std::endl;
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

    auto octDump = [](std::string title, size_t charGroupSize,
                      size_t lineSize) {
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

    auto noteRead = [](std::string title) {
      return [title](P7BIStream::pos_type pos, char *buffer, size_t size) {
        std::cout << "*** " << title << "t: " << pos << ":" << size
                  << std::endl;
      };
    };

    IStreamReader iStreamReader(input);
    Reader *reader = &iStreamReader;

    std::unique_ptr<ReaderMonitor> inputReadMonitor;
    if (dumpInputReads) {
      inputReadMonitor = std::make_unique<ReaderMonitor>(*reader);
      reader = inputReadMonitor.get();
      inputReadMonitor->addReadEventListener(hexDump("Input", 4, 64));
    }

    std::unique_ptr<ReaderMonitor> editInputReadMonitor;
    std::unique_ptr<ReaderMonitor> editOutputReadMonitor;
    std::unique_ptr<TapeEditStream> editReader;
    if (!edits.empty()) {
      if (dumpEditInputReads) {
        editInputReadMonitor = std::make_unique<ReaderMonitor>(*reader);
        reader = editInputReadMonitor.get();
        editInputReadMonitor->addReadEventListener(noteRead("EditInput"));
      }

      std::ifstream editsFile(edits);
      json editObj = json::parse(editsFile);
      editReader = std::make_unique<TapeEditStream>(*reader);
      reader = editReader.get();
      auto &editList = editObj["edits"];
      for (auto &editItem : editList) {
        size_t start = editItem[0];
        size_t end = editItem[1];
        std::string replacement = editItem[2];
        editReader->addEdit(start, end, replacement);
      }

      if (dumpEditOutputReads) {
        editOutputReadMonitor = std::make_unique<ReaderMonitor>(*reader);
        reader = editOutputReadMonitor.get();
        editOutputReadMonitor->addReadEventListener(noteRead("EditOutput"));
      }
    }

    std::unique_ptr<ReaderMonitor> p7BInputReadMonitor;
    std::unique_ptr<ReaderMonitor> p7BOutputReadMonitor;

    if (dumpP7BInputReads) {
      p7BInputReadMonitor = std::make_unique<ReaderMonitor>(*reader);
      reader = p7BInputReadMonitor.get();
      p7BInputReadMonitor->addReadEventListener(octDump("P7B Input", 6, 72));
    }

    std::unique_ptr<P7BIStream> p7biStream =
        std::make_unique<P7BIStream>(*reader);
    reader = p7biStream.get();

    if (dumpP7BOutputReads) {
      p7BOutputReadMonitor = std::make_unique<ReaderMonitor>(*reader);
      reader = p7BOutputReadMonitor.get();
      p7BOutputReadMonitor->addReadEventListener(octDump("P7B Output", 6, 72));
    }

    ShareExtractor extractor(*p7biStream, collateGlyphCardTape);
    extractor.read();
    input.close();
  }

  return EXIT_SUCCESS;
}