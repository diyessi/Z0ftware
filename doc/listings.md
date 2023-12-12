# Assembler Listings

Assembler listings are produced by the `sap23` assembler and reasonably approximate the listings produced by `UASAP` with some exceptions:
 - The `sap23` assembler uses ASCII input and produces ASCII output. Mixed-case comments are used for modern code annotations while lowercase comments are original.
 - Generated binary associated with pseudo-ops does not completely match `UASAP`.

## Assembler Input

The input to `UA SAP` is a deck of cards, or a deck of cards transferred to tape in BCD format. Each card corresponds to one line in a text file, with each BCD character converted to ASCII. Lines shorter than 80 characters are treated as though padded with spaces to 80 characters.

An assembler input line is divided into several fields based on 1-based column positions:
 - 1-6 is an optional label. Spaces are removed and infix operator characters may not be used. In one assembler if the label was an integer it was used as the location for the instruction.
 - 7 is blank.
 - 8-10 is the operation.
 - 11 is blank.
 - 12 to the first blank (recall that the line is treated as if it has spaces through column 80) is the "variable" field, which consists of up to three comma-separated expressions, the "address", the "tag" and the "decrement". A missing expression has the value `0`. The values of the three expressions are added to the magnitudes of the corresponding fields in the instruction.
 - After the first blank is an optional comment.
 - Some pseudo-ops replace the variable field and comment with a field having a special syntax.

In expressions, a `*` in a term position is used for the location of the line being assembled. A `**` in a term position has the value `0` and documents that the corresponding field in the instruction will be modified by the program before the instruction is actually executed.

All operations have a 15 bit address field and a three bit tag field. There are two types of operations, `Type A` which also have a 15 bit decrement field, and `Type B` which use the decrement field as an extension of the opcode. Most instructions subtract the or'd contents of the index registers selected in the tag field from the address, while a few instructions which manipulate index registers use the tag specify the index registers to use as sources/destinations.

The 704 word size is 36 bits, numbered `S`, `1-35` with `35` being the low-order bit position and `S` the sign. If `S=0` the sign is positive, while if `S=1` the sign is negative. Instructions are one word. In listings they are divided into four fields separated by spaces:
 - `prefix` is `S-2`. The prefix is written as the sign, if negative, or space if positive followed by the octal value for the remaining two bits. For example `001` is written ` 1` while `110` is written `-2`.
 - `decrement` is `3-17`, written in octal (five digits).
 - `tag` is `18-20`, written in octal (one digit).
 - `address` is `21-35`, written in octal (five digits).

In a listing, the contents of an assembled memory location are written as 
 - The location, as five octal digits.
 - A space.
 - The memory contents (described below).
 - A space.
 - The assembler input.

An assembler associates a base instruction with every instruction opcode. The value of the address expression is added to the address field, the value of the tag expression is added to the tag field, and the value of the decrement expression is added to the decrement field. Thus if a decrement is specified for an operation that does not have a decrement, it will still be added to the decrement field, changing the opcode. Likewise, specifying an address in a `RTB` (Read Tape Binary, which is a `RDS 221`) instruction is used to select a different tape unit.
