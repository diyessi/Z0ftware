# simh

[simh](https://github.com/open-simh/simh.git) is used for simulation.

## Building

```
cd simh
mkdir build
cd build
cmake .. -G Ninja
ninja
ctest
```

Note: Binaries will be put ound in `../simh/BIN`.

## The 704

To run the simulator,

```
../BIN/i704

IBM 704 simulator Open SIMH V4.1-0 Current        git commit id: b23cde9c
sim> 
```
### Showing the CPU configuration

To see information about the CPU,

```
sim>show cpu
IBM 704 simulator configuration

CPU     
        704, 32K
```

The CPU is configured with 32K of memory. A `704` can be configured with 4096, 8192, or 32768 words. The simulator memory can be changed with `set cpu (4k | 8k | 32k)`. With 32,768 words of memory, addresses are 15 bits, with 8K addresses are 13 bits, and with 4K addresses are 12 bits. Address-related registers described below only have as many bits as are required for the configured memory.

### Working with Memory

The `examine` command, or its shortened form `e`, shows what is in memory:

```
sim> e 0
0:	  000000000000
```

The `examine` command can also show the contents of a range of addresses:

```
sim> e 0-10
0:	  000000000000
1:	  000000000000
2:	  000000000000
3:	  000000000000
4:	  000000000000
5:	  000000000000
6:	  000000000000
7:	  000000000000
10:	  000000000000
```

The `deposit` command, or its shortened form `d`, sets values in memory. It has two arguments, the location and the value:

```
sim> d 0 123
sim> e 0
0:	  000000000123
```

## Memory

One word of `704` memory is 36 bits. The memory can be configured as 4096, 8192, or 32768 words. Addresses are 12, 13, or 15 bits respectively. If a larger address than the configured address is used, the extra bits are dropped. Accessing location 4096 on a `704` with `4096` words of memory will access address `0`.

The `704` is the first commercial computer to use core memory. A word can be accessed in only 12 microseconds. Reading is destructive, but the memory system will automatically rewrite the word. Core memory retains its contents without power. The `simh 704` uses a somewhat less expensive type of memory that does not retain its contents when the CPU is shut down.

Bit positions in a word are named `S` and `1` through `35` going from high to low:

| 35  | 34  | 33  | 32  | ... |  3  |  2  |  1  |  0  |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `S` | `1` | `2` | `3` |`...`|`32` |`33` |`34` |`35` |

Word contents are usually written as 12 octal digits, with `S` being written as `+` or left of when `0` and `-` when `1`, with the octal values starting at bit `1`.

## Registers

### Word Registers

The primary registers are the 38-bit accumulator, `AC`, and the 36-bit multiplier-quotient, `MQ`, which are paired for some operations. The two additional bits on the accumulator are between the `S` and `1` positions and called `Q` and `P`:

| 37  | 36  | 35  | 34  | 33  | 32  | ... |  3  |  2  |  1  |  0  |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `S` | `Q` | `P` | `1` | `2` | `3` |`...`|`32` |`33` |`34` |`35` |

The `AC` and `MQ` registers can be accessed with the `examine` and `deposit` commands. There are additional word registers that can be accessed from the console but not programmatically.

### Address Registers

Address registers are 12, 13, or 15 bits, according to the amount of installed memory. There are three index register, `XR[1]`, `XR[2]`, and `XR[4]`. Any value from `0` to `7` may be used as an index to `XR`; the value will be the `or` of the register. `XR[0]` is `0`, `XR[3]` is `XR[2]|XR[1]`, etc. In a later model there are seven index registers and a compatibility mode selects whether the `704` behavior is used versus selecting one of the seven registers. Most instructions add the specified index registers to the address specified in the instruction.

The instruction counter, `IC` is another address register. The contents of `IC` can be destructively read with the `TSI` instruction, which copies the twos-complement of `IC` into an index register and sets `IC` to a new value causing a branch. The `IC` can be written directly with the `TRA` instruction.

### Addresses

The number of bits in an address depends on the amount of memory. With 32,768 words, an address is 15 bits, with 8192 words of memory 13 bits, and with 4096 words of memory, 12 bits. The number of bits in registers that hold addresses really depend on the memory size.

Address offsets are twos-complement values.

Addresses are shown in octal.

### Words

A word on the `704` is 36 bits. Bit positions from high to low are `S` followed by `1` through `35`. Positions `1` through `35` are shown in octal, while `S` is shown as `-` when its value is `1`, and optionally shown as `+` when `0`.

The `704` has some instructions for treating a word as bits. These are called *logical operation* and *shifting operations*.

### Fixed-point

A fixed-point value is an integer scaled by a power of the base implicit from program context. When adding or subtracting fixed-point values with different scales, the representations of the values must first be adjusted to have the same scale. Then the integers can be added or subtracted and the result adjusted to have the desired scale for the output. In the case of multiplication and division, the integers can be directly multiplied or divided and the scales added or subtracted, followed by the integer adjusted for the desired output scale. An additional adjustment is needed on computers where multiplication and division have their own scale. For example, on some early computers n-bit multiplication and division are implemented as if the high-order bit is 1/2 rather than the low-order bit being 1. On the `704` the low-order bit is treated as 1.

Fixed-point values on the `704` are signed, but they do not use the twos-complement encoding. Instead, the left-most bit is the sign, `0` being positive and `1` being negative. The bit position names used on the `704` are derived from this encoding. The sign position is `S`. On a 36-bit word, the remaining bits are `1` through `35` left to right. The low-order bit is always `35` even in situations where there are more than 36 bits.


Like the earlier `701`, the `704` is built for scientific computing, i.e. serious numeric computation. The `704` replaces the unreliable Williams tube memory on the `701` with the first commercially available core memory. Any 36-bit word in memory can be read in only 12 microseconds.

Today's CPUs are designed to efficiently manipulate data in 8, 16, 32, 64, and 128 bit chunks. Characters were in the realm of CPUs designed for business, while the `704` was designed for scientific computing. John von Neumann believed that 32 bit fixed-point twos-complement arithmetic was all that was required for scientific computing, and that may have been the case for him. Within IBM, software floating point had been found to be very useful in spite of being very slow, and the `704` implemented 36-bit binary floating point in hardware in addition to 36-bit fixed-point. Each memory address corresponded to 4.5 8-bit bytes. In spite of the `704` being designed for scientific computing, both `FORTRAN` and `Lisp` were first implemented on the `704`.

If you cound the `0`s you will find 12, so each `0` corresponds to 36/12 = 3 bits. The memory values are octal. In fact, the left-most `0` only corresponds to two bits and is preceded by a space. More on this below.

A symptom of a common programming mistake is trying to read or write from memory at a small offset from 0. Today, when there is no shortage of memory addresses, compilers and linkers do not use the low addresses and the operating system works with the CPU to make sure that trying to read/write from low addresses will get an immediate error. With each 4K words of `704` memory cost about $61,000 per month in today's dollars, no one would have accepted making some of it inaccessible to reduce debugging time. Programmers were expected to very carefully check their programs before running them on the computer. On the `704` there wasn't even a way to cause accessing some memory an error. Even accessing memory outside of the installed memory size just ignores the address bits outside the installed range (NOTE: The simh console gives an error if you try to do this, while the simulator behaves like the actual `704` did.)

Notice that, in addition to the values, the addresses are octal.

## Writing Memory

## Fixed-point values

As mentioned previously, the `704` does not use twos complement for negative numbers. Instead, the high-order bit is a *sign* and the remaining bits are the *magnitude*. Bits were numbered in a big-endian style. The sign bit is called `S` and the remaining bits, high to low, are `1` to `35`:

| Value | `S` | `1`-`35`     |
|------:|:---:|:------------:|
|     1 | 0   | 000000000001 |
|    -1 | 1   | 000000000001 |
|     0 | 0   | 000000000000 |
|    -0 | 1   | 000000000000 |

When the `S` bit position is 1, a value will be displayed as negative:
```
sim> d 0 400000000000
sim> e 0
0:	 -000000000000
```
Instructions that treat the 36 bits like an unsigned value are called "logical".

## The accumulator

The `704` has a one-address architecture. Since most operations involve two or three values, instructions have implicit source/destination registers for some of the values. One of these registers is the accumulator, named `AC`. The `examine` and `deposit` commands provide access to it by using it in place of an address:
```
sim> e AC
AC:	0000000000000
```
The accumulator is a register that is an implicit source and the implicit destination of many arithmetic operations. It is named `AC` and can be accessed with `deposit` and `examine`:
```
sim> d AC 777777777777
sim> e AC
AC:	0777777777777
```
The `AC` has *38* bits, with the high bit being `S`, followed by `Q`, `P` and `1`-`35`.  The `Q` and `P` bits were meant to make it easier to handle arithmetic overflow.

## Running

Now let's try something more exciting.
```
sim> d 0 050000000100
sim> d 1 040000000101
sim> d 2 000000000000
sim> d 100 1
sim> d 101 2
sim> go 0

HALT instruction, IC: 00000 (  050000000100   CLA 00100)
sim> e AC
AC:	0000000000003
```
We put instructions in addresses 0 through 2, values in 100 and 101, and then told the `704` to start running at 0. When we were done, `AC` had 3 in it. What happened? The `examine` command has a `-m` option that will disassemble:
```
sim> e -m 0-2
0:	  050000000100   CLA 00100
1:	  040000000101   ADD 00101
2:	  000000000000   HTR 00000
```
The `CLA` instruction clears (zeros) `AC` and adds the contents of the specified address. The `ADD` instruction adds the contents of the specified address to `AC` and the `HTR` instruction halts and then branches to `00000` if you continue.

Like `examine`, The `deposit` command takes the `-m` option. We can use it to modify the program so that it stores the `AC` in 102:
```
sim> d -m 2 STO 102
sim> d -m 3 HTR 0
sim> e -m 0-3
0:	  050000000100   CLA 00100
1:	  040000000101   ADD 00101
2:	  060100000102   STO 00102
3:	  000000000000   HTR 00000
sim> go 0

HALT instruction, IC: 00000 (  050000000100   CLA 00100)
sim> e 102
102:	  000000000003
```

We can see `AC`'s overflow if we change the constants:
```
sim> d 101 377777777777
sim> go 0

HALT instruction, IC: 00000 (  050000000100   CLA 00100)
sim> e AC
AC:	0400000000000
sim> e 102
102:	  000000000000
```
We can see the overflow in the `P` bit. When `AC` is stored into `102` only `S` and `1` through `35` are stored.

Now lets subtract instead of add:
```
sim> d -m 1 sub 101
sim> d 101 2
sim> go 0

HALT instruction, IC: 00000 (  050000000100   CLA 00100)
sim> e 102
102:	 -000000000001
```

## Stopping execution

Use `^E` to get back to the simh prompt.

## Address and Decrement

A memory addresses is 15 bits, so two addresses can fit in a 36-bit word. Bits `21`-`35` are called the *address* and bits `3`-`17` are called the *decrement*. Various instructions can transfer the address or decrement of a memory location between that address, `AC` and index registers.
|             | Decrement |          | Address   | 
|:-----------:|:---------:|:--------:|:---------:|  
|`S` `1` `2`  | `3`-`17`  |`18`-`20` | `21`-`35` |

## Instruction Formats

There are two instruction formats, called *Type A* and *Type B*. The Type A format is:
| Prefix     | Decrement | Tag     | Address |
|:----------:|:---------:|:-------:|:-------:|
|`S` `1` `2` | `3`-`7`   |`18`-`20`|`21`-`35`|
|`*` `0` `1` |           |         |         |
|`*` `1` `0` |           |         |         |
|`*` `1` `1` |           |         |         |

The opcode is in the prefix and will not have `0` in both positions `1` and `2`. The Type B instructions are `0` in positions `1` and `2` and the opcode extends into the decrement positions:


| Prefix                 | Not used  | Tag     | Address |
|:----------------------:|:---------:|:-------:|:-------:|
|`S` `1` `2` `3`-`11`    | `12`-`17` |`18`-`20`|`21`-`35`|
|`*` `0` `0` `*`         |           |         |         |

## Index Registers

The `704` has three address-sized index registers, `1`, `2`, and `4`. With `examine` and `deposit` they are `XR[1]`, `XR[2]`, and `XR[4]`.
```
sim> e XR[1]
XR[1]:	00000
```
All instructions without an `X` in their name are *indexable*. In an indexable instruction, the tag has three bits, corresponding to index registers `4`, `2`, and `1`. The index registers selected in the tag are ored together (if no registers are selected, the result is `0`) and subtracted from the address to obtain the effective address that will be used in the instruction.

Non-indexable instructions are the Type A instructions and those that do not have a `1` in positions `8` and `9`.


## Loading memory from files

Repeatedly typing in values for memory quickly loses its thrill. Fortunately `simh` provides an alternative way to load values into memory, using a `.sym` file. If we create a `sub.sym` file using an editor containing:
```
100 OCT 1
101 OCT 2
0 CLA 100
1 SUB 101
2 STO 102
3 HTR 0
```
we can load it as
```
sim> load sub.sym
```
The format is `ADDRESS` `OP` `OPERAND`. For an "op" of `OCT` the operand is an octal value. Although this looks similar to an assembler format, you need to keep track of the addresses. If another operation is to be inserted after the `SUB` the `2` and `3` will need to be changed to `3` and `4`.  If you make a mistake on a line, the line is silently skipped. Don't keep your `.sym` files in the `simh/BIN` directory; `make clean` will remove them if you do.

## The MQ Register

Multiplication and division use the `MQ` and `AC` registers. The `LDQ` loads `MQ` from memory and `STQ` writes its value to memory. `MPY` multiplies. If we create `mult.sym`:
```
0 LDQ 100
1 MPY 101
2 STQ 102
3 HTR 0

100 OCT 2
101 OCT 3
```
and then load and run:
```
sim> load mult.sym 
sim> go 0

HALT instruction, IC: 00000 (  056000000100   LDQ 00100)
sim> e 102
102:	  000000000006
```
If the result is too big for `MQ`, `AC` will contain the upper half of the product:
```
simh> d 101 200000000000
simh> go 0
sim> e ac
AC:	0000000000001
sim> e 102
102:	  000000000000
sim> e ac
AC:	0000000000001
```




When an instruction specifies an index register, it specifies a value from `0` to `7`. The bits of the value determine which index registers are enabled and their values are or'd to determine the index value. Since `0` has no bits selected, it results in an index value of `0`. For most instructions, the index value is subtracted from the address.


MQ divide
decrement
subroutines

### Floating Point

Konrad Zuse's 1938 Z1 mechanical computer used binary floating point, and his Z3 relay computer even included arithmetic with infinity, halting when values were undefined. John von Neumann did not believe floating point was worth the complexity. This may have been true for him, but not for most people. The poularity of software implementations of floating point for some IBM computers before the `704` led to the `704` having built-in binary floating point.

Floating point values encode three components: a sign (S, +1 or -1), an exponent (E) and a mantissa (M). For binary floating point encodings, the value encoded is S2^EM. There are many ways to encode the three components. IEEE 754 is the most common today, although a number of variants have been proposed recently for machine learning.

Since S2^EM = S2^(E+k)(M/2^k), a value can have multiple representations by variying the exponent and mantissa. An encoding chooses a particular representation as the *normalized* versision. With IEEE 754, the normalized version is when 1 <= M < 2. The ensures that there is always a 1 in the first digit, so there is no need to encode the 1, which gives an extra bit of precision. On the `704`, 0 <= M < 1, so the mantissa also always starts with 1, but it is explicit in the encoding. In IEEE 754 32-bit the exponent is 8 bits, 64-bit 11 bits, represented as the two complement encoding of E+127 or E+1023. On the `704` the exponent is 8 bits, represented as the twos complement encoding of E+128.



The `704` floating point encoding:

| Sign|Characteristic|Fraction|
|:---:|:------------:|:------:|
| `S` | `1`-`8`      |`9`-`35`|

The characteristic/exponent is 8 bits with a bias of 128 and the fraction is 35 bits. When normalized, bit `9` will be `1` for non-zero values.  `0.0` is `+000000000000`.

For example, `.75` is binary `.11` which an exponent of `0`. With the bias, bits `1`-`8` will be `128`, or `200` (octal), giving `+200600000000`. 

`-.25` is binary `.01` which when normalized to `.1` has an exponent of `-1` which is biased to `127`, or `177`, giving `-177400000000`.

`1.0` is normalized to `.1` with an exponent of `1`, biased to `129` to give `+201400000000`.

### Characters

The `704` uses a six bit character encoding called `BCDIC` which is an extension of the four bit binary coded decimal (`BCD`) encoding. The encodings are easier to understand by following their development from punched cards to tape.

#### IBM Punched Cards

An IBM punched card is a rectangular piece of card stock that can have rectangular holes punched at any instersection of 80 columns and 12 rows. The columns are numbered from `1` to `80`, left to right with room at the top for printing the characters for each column:

|      |**1** |**2**|**3**|**4**|**5**|**6**|**7**|**8**| ... |**80**|
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|      |   | I | B | M |   | 7 | 0 | 4 |   |   |
|**12**|   | ▐ | ▐ |   |   |   |   |   |   |   |
|**11**|   |   |   | ▐ |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   | ▐ |   |   |   |
|**1** |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   |   |   |   |   |   |   |   |
|**4** |   |   |   | ▐ |   |   |   | ▐ |   |   |
|**5** |   |   |   |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   |   |   |   |   |
|**7** |   |   |   |   |   | ▐ |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   |   |   |
|**9** |   | ▐ |   |   |   |   |   |   |   |   |

Accounting machines are mechanical or electro-mechanical machines can work with datab in the form of stacks of punched cards, where each card is a record. Accounting machines are usually used for business but can also be used for scientific compuation. They can sort and collate cards based on their content. Reports can be printed, and new cards may be printed based on the contents of groups of cards. Because of an existing investment in punched card techniques, it is important that computers be able to work with the existing machines and data.

Punched cards and accounting machines were co-developed. The encoding for data needed to be in a format that was amenable to mechanical computation. Holes can be sensed mechanically with pins, electrically with brushes, and photo-electrically with light. Operations are performed starting mechanical movement in parallel with reading a card and stopping the movement when a hole is sense.

For example, on a card printer, a card passes over 80 sensors starting at the bottom of the card while the type bars for each column are raised. When a hole is sensed the type bar for that column stops rising, leaving the type bar for each column is positioned properly for printing the digit punched on the card. If no hole is sensed on a column, the type bar will have moved its full extent and be positioned for a blank. Then hammers strike all the type bars, printing the associated characters.

The encoding for a digit is a hole punched in the corresponding row, `0` through `9`.

|      |   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |   |
|**0** |   | ▐ |   |   |   |   |   |   |   |   |   |
|**1** |   |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |   |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |   |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |   |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |   |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |   |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |   |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   |   | ▐ |

#### BCD

Another decimal encoding is binary-coded decimal, or BCD, which uses four bits to represent each digit of a decimal number. So rather than encode 14 as `1110` in binary, it would be encoded as `0001 0100` in BCD. The BCDIC encoding is an extension of the BCD encoding.

### Alphameric

Cards also support `alphameric` values. This is a different mode of operation for a column, but the basic mechanism of card motion paired with mechanical motion remains. Rows `1` through `9` are called the `digits` and rows `12`, `11` and `0` are called the `zone`. This means the `zone` can have four values, `none`, `12`, `11` and `0`. Similarly, the digits can have 10 values, `none`, `1`, ..., `9` giving 40 possible characters. Ten digits, twenty-six letters, and space leave room for three additional characters.

|      | & | A | B | C | D | E | F | G | H | I |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**11**|   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |

|      | - | J | K | L | M | N | O | P | Q | R |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |
|**11**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**0** |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |

|      | 0 | / | S | T | U | V | W | X | Y | Z |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |
|**0** | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |

There are four values for the zones and ten for the digits (`1` through `9` pluse `none`) giving 40 possible characters. With 26 letters, 10 digits and space, that leaves room for `&`, `-` and `/`.

#### 7-Track Tape

The mechanics required to read and punch cards makes it slow compared to a computer. Magnetic tape is much faster and more compact. The tape is divided into 7 tracks, one for parity and six for data. When writing data to tape, a section of tape first passes erase heads, which force `0` onto all seven channels, and then passes record heads which write `1`s and `0`s on the appropriate tracks.

Tape is organized into a sequence of files. Each file is a sequence of records terminated by an end of file (EOF), which is 3.75" of blank tape, i.e. all tracks are `0`. A record is a sequence of values, followed immediately by a 7 bit even parity checksum and .75" of blank space.

The `704` supports two tape modes, `BIN` and `BCD`. The `BIN` format uses odd parity, which means the parity bit is set so there are an odd number of `1`s for each item written. For example, if the data were `010001` the parity would be `1` so the seven tracks would contain `1010001`. In the `BCD` mode, even parity is used, so `010001` would be written as `0010001`. Since blank tape, which is used to identify the ends of records and files, is the same as `0000000`, this means writing a sequence of `000000` values to tape is would be confused with end of record and end of file regions. To avoid this problem, `000000` cannot be used in `BCD` mode. In `BIN` mode with its odd parity, `0000000` will not occur.

Writing `000000`, `000001`, `000002`, `000003`, `000004`, `000005` in `BIN` mode:

|    |**P**|**5**|**4**|**3**|**2**|**1**|**0**|
|:--:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 000000 |  1  |  0  |  0  |  0  |  0  |  0  |  0  |
| 000001 |  0  |  0  |  0  |  0  |  0  |  0  |  1  |
| 000002 |  0  |  0  |  0  |  0  |  0  |  1  |  0  |
| 000003 |  1  |  0  |  0  |  0  |  0  |  1  |  1  |
| 000004 |  0  |  0  |  0  |  0  |  1  |  0  |  0  |
| 000005 |  1  |  0  |  0  |  0  |  1  |  0  |  1  |
| EOR |  1  |  0  |  0  |  0  |  0  |  0  |  1  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |

Writing `000001`, `000002`, `000003`, `000004`, `000005`, `000006` in `BCD` mode:

|    |**P**|**5**|**4**|**3**|**2**|**1**|**0**|
|:--:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 000001 |  1  |  0  |  0  |  0  |  0  |  0  |  1  |
| 000002 |  1  |  0  |  0  |  0  |  0  |  1  |  0  |
| 000003 |  0  |  0  |  0  |  0  |  0  |  1  |  1  |
| 000004 |  1  |  0  |  0  |  0  |  1  |  0  |  0  |
| 000005 |  0  |  0  |  0  |  0  |  1  |  0  |  1  |
| 000006 |  0  |  0  |  0  |  0  |  1  |  1  |  0  |
| EOR |  1  |  0  |  0  |  0  |  1  |  1  |  1  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  0  |  0  |  0  |  0  |  0  |  0  |  0  |
|    |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |  ⋮  |

The `BCD` tape mode is for character data and the encoding is almost the same as the encoding on punched cards. The digits part of the column is encoded as `BCD` with `none` encoded as `0000`, and forms the low four bits. There are four zones, `0`, `11`, `12`,and `none`, encoded as `01`, `01`, `03` and  `00` respectively. This makes the digits `1` through `9` encoded as their six bit binary value. Because `0` is a zone, it would be encoded as `01 0000` since `01` is the encoding for zone `0` and `0000` is the encoding for none of holes `1` through `9` being punched. `00 0000` would be a more natural place for `0` but since the `BCD` tape mode uses even parity, a string of `0`s on tape would look like an end of record or end of file. So `0` is encoded as `00 1010`.

#### The 8 punch

With six bits there are 64 possible characters, as compared to the 40 with the zone/digit punches for cards. The `8` row is used to extend the card encoding to 64 characters. If the `8` row is punched on a column, any of rows `1` through `7` may also be punched and the two `BCD` encodings are or'd or added together for the digits, giving 16 possible values for the digits, although only a few were being used on the `704`.


|      |   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | # | @ |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |

|      | & | A | B | C | D | E | F | G | H | I |+0 | . | ⌑ |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

|      | - | J | K | L | M | N | O | P | Q | R |-0 | $ | * |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**11**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

|      | 0 | / | S | T | U | V | W | X | Y | Z | ⧧ | , | % |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**0** | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

#### The Great Zone Swap

The BCDIC tape encoding is:

|      | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
|:----:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|**00**| NA | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 0  | #  | @  |    |    |    |
|**01**|    | /  | S  | T  | U  | V  | W  | X  | Y  | Z  | ⧧  | ,  | %  |    |    |    |
|**10**| -  | J  | K  | L  | M  | N  | O  | P  | Q  | R  |-0  | $  | *  |    |    |    | 
|**11**| &  | A  | B  | C  | D  | E  | F  | G  | H  | I  |+0  | .  | ⌑  |    |    |    |

Sorting into alphabetic order using theis BCDIC encoding would be messy, so zones `01` and `11` are swapped between tape and memory and `0` is moved to `00 0000` in memory.

The BCDIC `704` encoding is:

|      | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 |
|:----:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|**00**| 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  |    | #  | @  |    |    |    |
|**01**| &  | A  | B  | C  | D  | E  | F  | G  | H  | I  |+0  | .  | ⌑  |    |    |    |
|**10**| -  | J  | K  | L  | M  | N  | O  | P  | Q  | R  |-0  | $  | *  |    |    |    | 
|**11**|    | /  | S  | T  | U  | V  | W  | X  | Y  | Z  | ⧧  | ,  | %  |    |    |    |

 The `704` reads/writes words, so six six bit values are read/written every time a word is read/written. There is not a way to read/write less than 36 bits. If there is data available to write, the tape keeps moving. If the CPU provides data before the tape is ready, the CPU blocks until the tape is ready. If 336 microseconds elapse after a write, the tape writes an end of record checksum and .75" of blank tape and then stops. There is no other way to tell the tape unit to write an end of record. The checksum value for each channel is the exclusive or of the values written to that channel in the record.

The `704` supported two tape formats, binary and BCD. Binary used odd parity and BCD used even parity for the 7th bit, while the record checksum was always even parity. In BCD mode, the six bit values were treated as characters, with the two high bits called the zone (related to punched card encoding). Zones 01 and 11 are swapped between the `704` and tape. Also, because blank tape and zeros are the same thing and six zeros with even parity would have 0 parity, the value `000000` is converted to `001010`. A number of six bit values (such as `001010`) have no representation in BCD.



For a binary representation, the digit is represented in `BCD` as before, and two additional `zone` bits are added to the left:
 -  `none` : `00`
 -  `12` : `11`
 -  `11` : `10`
 -  `0` : `01` 

The digit part of the binary encoding wastes the binary values above `9`. These values are made accessible by adding the additional rule that if row `8` is punched then the `8` bit is set in the digit bits. Combined with punches in rows `1` through `7` this permits the digit bits to have any value from `0000` to `1111`.

Zone `none` loses the chracter `0` and picks up space:

|**00**|   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | # | @ |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |

Zone `12` is:

|**11**| & | A | B | C | D | E | F | G | H | I |+0 | . | ⌑ |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

Zone `11` is:

|**10**| - | J | K | L | M | N | O | P | Q | R |-0 | $ | * |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**11**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**0** |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

Zone `0` is:

|**01**| 0 | / | S | T | U | V | W | X | Y | Z | ⧧ | , | % |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**11**|   |   |   |   |   |   |   |   |   |   |   |   |   |
|**0** | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   | ▐ |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   | ▐ | ▐ | ▐ |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |

This almost gives the translation from punched card to tape format.  The string "HELLO" in BCD is: `11 1000 11 0101 10 0011 10 0011 10 0110`. Unfortunately, the `0` character ends up separated from the the other digits, at `01 0000`.




|      |*space*| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | & | A | B | C | D | E | F | G | H | I |
|:----:|:-----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|       |   |   |   |   |   |   |   |   |   |   | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**11**|       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**0** |       | ▐ |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**1** |       |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |       |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |       |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |       |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |       |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |       |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |       |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |       |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |       |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |

|      | - | J | K | L | M | N | O | P | Q | R | 0 | / | S | T | U | V | W | X | Y | Z |
|:----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|**12**|   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
|**11**| ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |   |   |   |   |   |   |   |   |   |   |
|**0** |   |   |   |   |   |   |   |   |   |   | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ | ▐ |
|**1** |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |
|**2** |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |
|**3** |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |
|**4** |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |
|**5** |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |
|**6** |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |
|**7** |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |   |
|**8** |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |   |
|**9** |   |   |   |   |   |   |   |   |   | ▐ |   |   |   |   |   |   |   |   |   | ▐ |


The tape encoding is:

|       | **0** | **1** | **2** | **3** | **4** | **5** | **6** | **7** |**10** |**11** |**12** |**13** |**14** |**15** |**16** |**17** |
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| **0** |       |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   0   |   #   |   @   |       |       |       |
| **1** |*space*|   /   |   S   |   T   |   U   |   V   |   W   |   X   |   Y   |   Z   |   ⧧   |   ,   |   %   |       |       |       |
| **2** |   -   |   J   |   K   |   L   |   M   |   N   |   O   |   P   |   Q   |   R   |  -0   |   $   |   *   |       |       |       |
| **3** |   &   |   A   |   B   |   C   |   D   |   E   |   F   |   G   |   H   |   I   |  +0   |   .   |   ⌑   |       |       |       |

The in-memory encoding is:

|       | **0** | **1** | **2** | **3** | **4** | **5** | **6** | **7** |**10** |**11** |**12** |**13** |**14** |**15** |**16** |**17** |
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| **0** |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |       |   #   |   @   |       |       |       |
| **1** |   &   |   A   |   B   |   C   |   D   |   E   |   F   |   G   |   H   |   I   |  +0   |   .   |   ⌑   |       |       |       |
| **2** |   -   |   J   |   K   |   L   |   M   |   N   |   O   |   P   |   Q   |   R   |  -0   |   $   |   *   |       |       |       |
| **3** |*space*|   /   |   S   |   T   |   U   |   V   |   W   |   X   |   Y   |   Z   |   ⧧   |   ,   |   %   |       |       |       |

# IBM 704 Information

- [Coding for the MIT-IBM 704 Computer][def]
- [Training manual][def6]
- [Programming and Coding the IBM-709-7090-7094](https://justine.lol/sectorlisp/ibm709.pdf)
- [Bitsavers IBM 704][def2]
  - [704 electronic data-processing machine][def3]
- [Piercefuller software][def4]
- [Fortran source][def5]
- [Sky Visions][def7]
- [BCD character codes][def8]

# Character Sets
```
01234567
89x#@xxx
&ABCDEFG
HI+.⌑xxx
-JKLMNOP
QR-$*xxx
 /STUVWX
YZ⧧,%xxx
```
[def]: http://bitsavers.org/pdf/mit/computer_center/Coding_for_the_MIT-IBM_704_Computer_Oct57.pdf
[def2]: https://bitsavers.org/pdf/ibm/704/
[def3]: https://bitsavers.org/pdf/ibm/704/24-6661-2_704_Manual_1955.pdf
[def4]: https://www.piercefuller.com/oldibm-shadow/709x.html
[def5]: https://www.softwarepreservation.org/projects/FORTRAN/index.html#Source_code
[def6]: https://dome.mit.edu/bitstream/handle/1721.3/48791/DTMB_1960_1368.pdf
[def7]: http://sky-visions.com/ibm/ibm704_soft.shtml
[def8]: https://en.wikipedia.org/wiki/BCD_(character_encoding)#IBM_704_BCD_code
[def9]: http://web.mit.edu/STS.035/www/PDFs/edvac.pdf