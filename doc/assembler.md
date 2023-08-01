# 70x Assemblers

## NY AP 1

When programming is described in [Principles of Operation Type 701](https://bitsavers.org/pdf/ibm/701/24-6042-1_701_PrincOps.pdf) it is described in terms of where to punch the holes in a binary loader card; four 18 bit instructions per row, starting from the bottom of the card going left to right. Although at assemblers were developed for the 701, [Nathaniel Rochester and William McClelland both developed assemblers](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=4640454)

In contrast, [704 electronic data-processing machine](https://bitsavers.org/pdf/ibm/704/24-6661-2_704_Manual_1955.pdf), briefly describes "Symbolic Programming", stating that more details can be found in the `NY AP 1` write-up. Not having access to a copy of the `NY AP 1` write-up, the short description must must serve as the source of information on the initial 704 assembler.

Symbols are used to name locations, and should only be used to name locations that are referred to elsewhere in the program. Symbols are six characters from any of the 47 BCD characters (includes space). A word holds six 6-bit characters, so symbols can be compared as integers. Locations are specified as a pair of a `Symbolic` and an `Absolute`, whose values are added. A `Class` can be `O` to indicate that the `Absolute` is octal and a class of `-` indicates a negative value.

The examples show a paper form with columns, only some of which are explained. The first two columns, not described, are `Identification` and `Card Class`. These are followed by `Location`, which is where a symbol defining an address is written. This is followed by `Operation code`, the three character operation. These are followed by either`Class`, `Number`, `Exponent`, and `Binary Place` or `Class`, `Address`, `Tag` and `Decrement`. `Address` and `Decrement` use three columns, `Class`, `Symbolic` and `Absolute`. The two values are added when both are specified. The `Tag` is ` `, `A`, `B` or `C` or `7` to indicate all tags. The final column is for comments.

There are at least two pseudo-ops, `OCT`, for specifying an octal constant for a word and `FXD` for a fixed-point constant. Constants are written 6 characters in each of the `Symbolic` and `Absolute` columns.


