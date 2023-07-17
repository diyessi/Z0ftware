# Z0ftware
Tools for working with IBM 704, 709, 7090 and 7094 (70x) software.

## The 70x Computers

If you have heard of [Amdahl's Law](https://en.wikipedia.org/wiki/Amdahl%27s_law), the IBM 704 is Gene Amdahl's second computer and his first with IBM ( he also played a role in the earlier IBM 701). A [typical 704 system](https://ed-thelen.org/comp-hist/BRL61-ibm0704.html) rented for about $44,000/month, or about $11/minute in today's dollars. It used at least 60,000 watts, which meant large power costs even before air conditioning costs were added. While the power needs alone made the computer expensive to run, the high monthly rental made it expensive not to have the computer performing valuable work as much as possible. For the IBM 704, this valuable work was numeric computation.

Like the IBM 701, the 70x computers are 36 bit processors. The computer can be configured with 4096, 8192, or 32768 36 bit words of core memory, often called registers. An entire word is always read/written. Addresses are one of 12, 13, or 15 bit addresses, depending on the memory configuration.  When additional storage is needed, 8 magnietic drums can be used, each able to hold 2048 words. When even that is not enough memory, tapes can be used. One tape can hold up to 900,000 words.

Integers are encoded in a sign magnitude format. The high bit, called `S` is 0 for positive integers and 1 for negative integers. The remaining 35 bits, called 1-35 from high to low, are the magnitude of the value. Because of the sign-magnitude format, there is a +0 and a -0. The convention is to write values in octal, except that the sign is written as `+` or `-`.

Without a supply of programs to perform numeric computations the 704 would be idle. Amdahl, John Backus and others realized that computer improvements could be used to make program development and maintenance more efficient. Before the 704, commercial computers used fixed-point arithmetic. Part of programming was the tedious task of working out the ranges of values at each part of the program and scale the values to avoid overflow and rounding problems. If the program were changed or the ranges changed for a new application of the program, the scaling would also need to be recomputed and changed. Amdahl had build a computer with floating point arithmetic at the University of Wisconsin (Konrad Zuse had much earlier built the mechanical Z1 computer with floating point) and Backus had written floating point libraries for earlier IBM computers. For many problems, floating point automates the scaling, trading some execution speed for faster development and simpler maintenance. The 704 had floating point instructions, reducing the speed tradeoff.

Array access was often implemented by modifying the instruction that accessed memory to have the address of the element to be accessed. The 704 had three index registers whose contents could automatically be added to the address in any instruction that accessed memory, as well as being tested and decremented in some branch instructions.

Assembler programs were already being used to convert symbolic representations of instructions opcodes into their binary representations. Two pass assemblers could also automatically assign addresses to symbolic names for locations. Whirlwind had a program that could convert algebraic expressions into programs. Backus wanted an expanded version for the already developed 701, but limited memory and missing features made it impractical. Improvements to the 704, such as floating point, index registers, and core memory made the first implementation of FORTRAN, being devloped at the same time, possible on the 704 with the available software technology. Some details can be found in [Lorenzo's Abstracting Away the Machine](https://www.amazon.com/Abstracting-Away-Machine-Programming-TRANslation/dp/1082395943) and in the [Charles Babbage Institute's interview with Gene Amdahl](https://conservancy.umn.edu/bitstream/handle/11299/104341/oh107gma.pdf). Later, Lisp was developed on the 704 at MIT.

### Loading

The 704 has a `Load Card` button for "booting" off a card. It performs the following four instructions:

```
Select card reader for reading
Read an instruction into address 0
Read an instruction into address 1
Branch to 0
```
In modern terms, the 704 had a four instruction BIOS that loaded a two instruction boot loader.

### File Formats

Since USB punched card equipment is difficult to obtain, several file formats have been developed for representing card data. The simplest is an ASCII file where each line corresponds to one alphanumeric card containing the same characters. If the line is less than 80 characters, it is assumed to be padded with spaces to 80 characters, and if it is longer than 80 characters, it is truncated to 80 characters.


