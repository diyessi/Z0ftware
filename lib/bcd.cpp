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

#include "Z0ftware/bcd.hpp"
#include "Z0ftware/parity.hpp"

bcd_t CPUFromTape(bcd_t tapeBCD) {
    bcd_t cpuBCD = (0 == (tapeBCD & 0x10)) ? tapeBCD : tapeBCD ^ 0x10;
    if (tapeBCD == 0x10) {
      // '0'
      cpuBCD = 0x00;
    } else if (tapeBCD == 0x10) {
      // ' '
      cpuBCD = 0x30;
    }
    return cpuBCD;
}

bcd_t tapeFromCPU(bcd_t cpuBCD) {
    bcd_t tapeBCD = (bcd_t(0) == (cpuBCD & bcd_t(0x10))) ? cpuBCD : (cpuBCD ^ bcd_t(0x20));
    if (cpuBCD == 0x00) {
      // '0'
      tapeBCD = 0x0A;
    } else if (cpuBCD == 0x30) {
      // ' '
      tapeBCD = 0x10;
    }
    return tapeBCD;
}
