// MIT License
//
// Copyright (c) 2023 Scott Cyphers
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

#include "Z0ftware/op.hpp"

#include <map>

namespace {
std::vector<OpSpec> opSpecs704 = {
    // From IBM 704 reference
    {"ACL", +00361, "Add and Carry Logical Word"},
    {"ADD", +00400, "Add"},
    {"ADM", +00401, "Add Magnitude"},
    {"ALS", +00767, "Accumulator Left Shift"},
    {"ANA", -00320, "AND to Accumulator"},
    {"ANS", +00320, "AND to Storage"},
    {"ARS", +00771, "Accumulator Right Shift"},
    {"BST", +00764, 0220, "Backspace Tape (plus i)"},
    {"CAL", -00500, "Clear and Add Logical Word"},
    {"CAS", +00340, "Compare Accumulator with Storage"},
    {"CHS", +00760, 0002, "Change Sign"},
    {"CLA", +00500, "Clear and Add"},
    {"CLM", +00760, 0000, "Clear Magnitude"},
    {"CLS", +00502, "Clear and Subtract"},
    {"COM", +00760, 0006, "Complement Magnitude"},
    {"CPY", +00700, "Copy and Skip"},
    {"DCT", +00760, 0012, "Divide Check Test"},
    {"DVH", +00220, "Divide or Halt"},
    {"DVP", +00221, "Divide or Proceed"},
    {"ETM", +00760, 0007, "Enter Trapping Mode"},
    {"ETT", -00760, 0011, "End of Tape Test"},
    {"FAD", +00300, "Floating Add"},
    {"FDH", +00240, "Floating Divide or Halt"},
    {"FDP", +00241, "Floating Divide or Proceed"},
    {"FMP", +00260, "Floating Multiply"},
    {"FSB", +00302, "Floating Subtract"},
    {"HPR", +00420, "Halt and Proceed"},
    {"HTR", +00000, "Halt and Transfer"},
    {"LBT", +00760, 0001, "Low Order Bit Test"},
    {"LDA", +00460, "Locate Drum Address"},
    {"LDQ", +00560, "Load MQ"},
    {"LGL", -00763, "Logical Left"},
    {"LLS", +00763, "Long Left Shift"},
    {"LRS", +00765, "Long Right Shift"},
    {"LTM", -00760, 0007, "Leave Trapping Mode"},
    {"LXA", +00534, "Load Index from Address"},
    {"LXD", -00534, "Load Index from Decrement"},
    {"MPR", -00200, "Multiply and Round"},
    {"MPY", +00200, "Multiply"},
    {"MSE", -00760, "Minus Sense"},
    {"NOP", +00761, "No Operation"},
    {"ORA", -00501, "OR to Accumulator"},
    {"ORS", -00602, "OR to Storage"},
    {"PAX", +00734, "Place Address in Index"},
    {"PBT", -00760, 0001, "P Bit Test"},
    {"PDX", -00734, "Place Decrement in Index"},
    {"PSE", +00760, "Plus Sense"},
    {"PXD", -00754, "Place Index in Decrement"},
    {"RDS", +00762, "Read Select"},
    {"REW", +00772, 0220, "Rewind (plus i)"},
    {"RND", +00760, 0010, "Round"},
    {"RQL", -00773, "Rotate MQ Left"},
    {"RTT", -00760, 0012, "Redundancy Tape Test"},
    {"SBM", -00400, "Subtract Magnitude"},
    {"SLQ", -00620, "Store Left Half MQ"},
    {"SLW", +00602, "Store Logical Word"},
    {"SSM", -00760, 0003, "Set Sign Minus"},
    {"SSP", +00760, 0003, "Set Sign Plus"},
    {"STA", +00621, "Store Address"},
    {"STD", +00622, "Store Decrement"},
    {"STO", +00601, "Store"},
    {"STP", +00630, "Store Prefix"},
    {"STQ", -00600, "Store MQ"},
    {"SUB", +00402, "Subtract"},
    {"SXD", -00634, "Store Index in Decrement"},
    {"TIX", +02000, "Transfer on Index"},
    {"TLQ", +00040, "Transfer on Low MQ"},
    {"TMI", -00120, "Transfer on Minus"},
    {"TNO", -00140, "Transfer on No Overflow"},
    {"TNX", -02000, "Transfer on No Index"},
    {"TNZ", -00100, "Transfer on No Zero"},
    {"TOV", +00140, "Transfer on Overflow"},
    {"TPL", +00120, "Transfer on Plus"},
    {"TQO", +00161, "Transfer on MQ Overflow"},
    {"TQP", +00162, "Transfer on MQ Plus"},
    {"TRA", +00020, "Transfer"},
    {"TSX", +00074, "Transfer and Set Index"},
    {"TTR", +00021, "Trap Transfer"},
    {"TXH", +03000, "Transfer on Index High"},
    {"TXI", +01000, "Transfer with Index Incremented"},
    {"TXL", -03000, "Transfer on Index Low or Equal"},
    {"TZE", +00100, "Transfer on Zero"},
    {"UFA", -00300, "Unnormailzed Floating Add"},
    {"UFM", -00260, "Unnormailzed Floating Multiply"},
    {"UFS", -00302, "Unnormalized Floating Subtract"},
    {"WEF", +00770, 0220, "Write End of File (plus i)"},
    {"WRS", +00766, "Write Select"},
    // From instruction set in MIT documentation
    // http://bitsavers.org/pdf/mit/computer_center/Coding_for_the_MIT-IBM_704_Computer_Oct57.pdf
    //
    // Some are mentioned by name in SAP documentation
    {"CAC", -00700, 0000, "Copy add and carry"},
    {"CAD", -00700, 0000, "Copy add and carry"},
    {"CFF", +00760, 0030, "Change file frame"},
    {"IOD", +00766, 0333, "In-out delay"},
    {"RCD", +00762, 0321, "Read card"},
    {"RDR", +00762, 0300, "Read drum (plus i)"},
    {"RPR", +00762, 0361, "Read printer"},
    {"RTB", +00762, 0220, "Read tape binary (plus i)"},
    {"RTD", +00762, 0200, "Read tape BCD (plus i)"},
    {"SLF", +00760, 0140, "Sense lights off"},
    {"SLN", +00760, 0140, "Sense light on (plus i)"},
    {"SLT", -00760, 0140, "Sense light test (plus i)"},
    {"SPR", +00760, 0360, "Sense printer (plus i)"},
    {"SPT", +00760, 0360, "Sense printer test"},
    {"SPU", +00760, 0340, "Sense punch (plus i)"},
    {"SWT", +00760, 0160, "Sense switch test (plus i)"},
    {"WDR", +00766, 0300, "Write drum (plus i)"},
    {"WPR", +00766, 0361, "Write on printer"},
    {"WPU", +00766, 0341, "Write on punch"},
    {"WTB", +00766, 0220, "Write tape in binary (plus i)"},
    {"WTD", +00766, 0200, "Write tape in BCD (plus i)"},
    {"WTS", +00766, 0320, "Write on tape simultaneously (plus i)"},
    {"WTV", +00766, 0330, "Write on cathode-ray tube"},
    // Convenience ops from SAP documentation
    {true, "FOR", 04000, "Four"},
    {true, "FVE", 05000, "Five"},
    {true, "MON", -01000, "Minus one"},
    {true, "MTH", -03000, "Minus three"},
    {true, "MTW", -02000, "Minus two"},
    {true, "MZE", 04000, "Minus zero"},
    {true, "PON", +01000, "Plus one"},
    {true, "PTH", +03000, "Plus three"},
    {true, "PTW", +02000, "Plus two"},
    {true, "PZE", +00000, "Plus zero"},
    {true, "SIX", 06000, "Size"},
    {true, "SVN", 07000, "Seven"},
};
}
std::optional<const OpSpec *> OpSpec::getOpSpec(const std::string_view &name) {
  auto initOps = []() -> std::map<const std::string_view, const OpSpec *> {
    std::map<const std::string_view, const OpSpec *> ops;
    for (const auto &opSpec : opSpecs704) {
      ops[opSpec.getOperation()] = &opSpec;
    }
    return ops;
  };

  static std::map<const std::string_view, const OpSpec *> ops = initOps();

  auto it = ops.find(name);
  if (it != ops.end()) {
    return it->second;
  }
  return std::optional<const OpSpec *>();
}

const OpSpec *OpSpec::getOpSpec(word_t word) {
  using map_t = std::map<word_t, const OpSpec *, std::greater<word_t>>;
  auto initOps = []() -> map_t {
    map_t ops;
    for (const auto &opSpec : opSpecs704) {
      if (!opSpec.isConstant()) {
        ops[opSpec.getWord()] = &opSpec;
      }
    }
    return ops;
  };

  static map_t ops = initOps();

  return ops.lower_bound(word)->second;
}
