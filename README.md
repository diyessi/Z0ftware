# 70ftware
Tools for working with IBM 704, 709, 7090 and 7094 (70x) software.

## The 70x Computers

If you have heard of [Amdahl's Law](https://en.wikipedia.org/wiki/Amdahl%27s_law), the IBM 704 is Gene Amdahl's second computer and his first with IBM ( he also played a role in the earlier IBM 701). A [typical 704 system](https://ed-thelen.org/comp-hist/BRL61-ibm0704.html) rented for about $44,000/month, or about $11/minute in today's dollars. It used at least 60,000 watts, which meant large power costs even before air conditioning costs were added. While the power needs alone made the computer expensive to run, the high monthly rental made it expensive not to have the computer performing valuable work as much as possible. For the IBM 704, this valuable work was numeric computation.

Like the IBM 701, the 70x computers are 36 bit processors. The computer can be configured with 4096, 8192, or 32768 36 bit words of core memory, often called registers. An entire word is always read/written. Addresses are one of 12, 13, or 15 bit addresses, depending on the memory configuration.  When additional storage is needed, 8 magnietic drums can be used, each able to hold 2048 words. When even that is not enough memory, tapes can be used. One tape can hold up to 900,000 words.

Integers are encoded in a sign magnitude format. The high bit, called `S` is 0 for positive integers and 1 for negative integers. The remaining 35 bits, called 1-35 from high to low, are the magnitude of the value. Because of the sign-magnitude format, there is a +0 and a -0. The convention is to write values in octal, except that the sign is written as `+` or `-`.

Without a supply of programs to perform numeric computations the 704 would be idle. Amdahl, John Backus and others realized that computer improvements could be used to make program development and maintenance more efficient. Before the 704, commercial computers used fixed-point arithmetic. Part of programming was the tedious task of working out the ranges of values at each part of the program and scale the values to avoid overflow and rounding problems. If the program were changed or the ranges changed for a new application of the program, the scaling would also need to be recomputed and changed. Amdahl had build a computer with floating point arithmetic at the University of Wisconsin (Konrad Zuse had much earlier built the mechanical Z1 computer with floating point) and Backus had written floating point libraries for earlier IBM computers. For many problems, floating point automates the scaling, trading some execution speed for faster development and simpler maintenance. The 704 had floating point instructions, reducing the speed tradeoff.

Array access was often implemented by modifying the instruction that accessed memory to have the address of the element to be accessed. The 704 had three index registers whose contents could automatically be added to the address in any instruction that accessed memory, as well as being tested and decremented in some branch instructions.

Assembler programs were already being used to convert symbolic representations of instructions opcodes into their binary representations. Two pass assemblers could also automatically assign addresses to symbolic names for locations. Whirlwind had a program that could convert algebraic expressions into programs. Backus wanted an expanded version for the already developed 701, but limited memory and missing features made it impractical. Improvements to the 704, such as floating point, index registers, and core memory made the first implementation of FORTRAN, being devloped at the same time, possible on the 704 with the available software technology. Some details can be found in [Lorenzo's Abstracting Away the Machine](https://www.amazon.com/Abstracting-Away-Machine-Programming-TRANslation/dp/1082395943) and in the [Charles Babbage Institute's interview with Gene Amdahl](https://conservancy.umn.edu/bitstream/handle/11299/104341/oh107gma.pdf). Later, Lisp was developed on the 704 at MIT.

## Punched Cards

Before they were know for computers, IBM was known for its mechanical and electromechanical business machines, based on punched cards. At the time, an IBM punched card had a 12x80 grid of locations where a rectangular hole could be punched out by various kinds of card punch equipment. Card readers could sense the holes mechanically, electrically, or optically. Originally, each column was either blank (no holes punched) or had a hole in a single row, 0 near the top through 9 at the bottom. As a card passed through card reading equipment, wheels or rods for each column moved at the same time until a hole was sensed, leaving the wheel/rod in a physical position corresponding to the value. For example, when accumulating, if the 3 hole in the card were punched, an accumulating wheel would be advanced by three positions. Jumpers on a control panel specified how equipment should react to values in specific columns.

Rows 12 and 11 were added at the top of cards to support alphanumeric (called alphameric) data. From top to bottom the rows were numbered 12, 11, 0, 1, 2, ..., 9. A single hole in rows 0 through 9 represented a digit, while a pair of a *zone* punch (rows 12, 11, 0) and a digit punch (rows 1 thorugh 9) encoded a letter or symbol. 12-1 was `A`, 12-2 was `B`, 12-9 was `I`, 11-1 was `J`, ... 11-9 was `R`. Notice that none of these combinations had the holes in adjacent rows, which would weaken the card. `S` was 0-2, avoiding the weak 0-1 combination. 0-3 was `T`, ... 0-9 was `Z`. A single hole in 12 became `&` while a single hole in 11 was `-`. 0-1 was given to `/`. Additional characters were added by using combinations involving row 8, three hole combinations, etc. There was some variance between sites as to how symbols were encoded; as long as the card punches and printers were in agreement and data was not interchanged between sites, this was not a problem.

Recall that the cards were read starting with the bottom row, since this was the most effective way to build equipment that processed data on cards. Today a computer reading data from a card would just read the entire card and put the data in memory in column order. Memory was very expensive so data was sent to the computer row by row. The left 36 columns of row nine were sent, followed by the right 36 columns, then row 8 would be sent, up thorugh row 12 for a total of 24 36-bit values. The last 8 columns were not accessible. If the card data came from a key punch, the computer would need to be transpose it from 24 36 bit values into 72 encodings of characters.

Computer controlled card punch equipment also worked row at a time, unlike the column at a time manual key punch equipment. To punch characters onto a card they character encodings would need to first be transposed to be column major. On the other hand, if binary data were to be punched on cards for later reading, 24 consecutive values could be durectly punched and later read in without any additional processing. An important use for binary data on cards was putting machine instructions. The instructions could be easily loaded into memory and executed.

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


