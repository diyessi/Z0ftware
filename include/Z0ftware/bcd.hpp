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

// BCD characters
//
// A number of 6-bit "BCD" character encodings were used, depending on the
// application, storage medium, and CPU. Not all characters became ASCII
// characters or even Unicode.
//
// Here, 6-bit characters (7-bit with parity) are embedded in std::uint8_t
// Six 6-bit characters were big-endian packed into a 36-bit word, embedded here
// in a std::uint64_t.

#ifndef Z0FTWARE_BCD_HPP
#define Z0FTWARE_BCD_HPP

#include <cstdint>

using bcd_t = std::uint8_t;

char charFromBCD(bcd_t bcd);
bcd_t BCDFromChar(char ascii);

#endif
