# Punched Cards

Before they were known for computers, IBM was known for its mechanical and electromechanical business machines, based on punched cards. A file was a collection of records, each record being a card. A file was sorted by physically rearranging the cards. A stack of cards is called a *deck*.

In the 1950s an IBM punched card had a 12x80 grid of locations where a rectangular hole could be punched out by various kinds of card punch equipment. Card readers could sense the holes mechanically, electrically, or optically.

Columns are numbered from 1 to 80, left to right, and divided into fields. Earlier cards had 10 rows, called digits, numbered 0 near the top to 9 near the bottom. A hole punched in a row encodes the corresponding digit for the column. To support characters other than digits, two additional rows were used at the top of the card, 12 and 11 (also called Y and X). The top three rows, 12, 11, and 0 (or 10) are called zone rows. Alphabetic characters used two holes in a column, one in a zone row and one in rows 1 through 9. The combination 12-1 through 12-9 were *A* through *I*, 11-1 through 11-9 were *J* through *R* and 0-2 through 0-9 were *S* through *Z*, avoiding having any characters encoded with adjacent rows. Over time, additional characters were added by using only a zone punch and by using three punches in a column. There was some variation in character sets and their encodings, but 70x character sets have at most 64 characters.

Raw binary data can also be stored on punched cards. Each of the 80 columns can hold 12 bits, or, equivalently, each of the 12 rows can hold 80 bits. The 704 uses columns 1 through 72, splitting the card into a left half, 1-36, and a right half, 37-72, to hold 24 36 bit values, starting with the left side of row 9, then the right side, then the left side of row 8, etc. If the data is to be interpreted as text, it must be transposed into columns and converted to the BCD encoding.

# File formats for decks

Since punched card equipment is not even compatible with "vintage" computers today, card decks have been converted to a variety of file formats. In addition to encoding the raw data on each card, the file formats may include meta-data to indicate events that could occur while reading a deck of cards, such as coming to the end of the deck.

## ASCII Text

The easiest to work with file format is an ASCII text file. Each line corresponds to one card. If the line is longer than 80 characters, it is truncated, and if it is shorter than 80 characters it is padded with spaces. Tabs are treated as 8 spaces. Since there is not a 1-1 correspondence between ASCII and Hollerith/BCD, a 64 character subset of ASCII is chosen to correspond to the 64 characters in a BCD character set. A program can work with the ASCII representation and then convert to the selected BCD encoding when converting to 70x encodings.

Lines that start with the *~* character are special-cased. A line with just a *~* is treated as an end of file. *~raw* is followed by 4 digit octal values, representing binary columns with row 12 the high order. *~eor* is 7-8-9, *~eof* is 6-7-9, and *~eoi* is 6-7-8-9.

## .cbn format

Here, bit positions are little-endian starting at 0.


|**Byte**| 7 | 6        |    5 |    4 |   3 |   2 |   1 | 0   |
|:------:|:-:|:--------:|:----:|:----:|:---:|:---:|:---:|:---:|
|**0**   |`1`|Odd Parity|**12**|**11**|**0**|**1**|**2**|**3**|
|**1**   |`0`|Odd Parity|**4** |**5** |**6**|**7**|**8**|**9**|

Each card is 160 bytes, 2 bytes per column. Bit 7 is set on the first byte of each card. Bits 5-0 of the first byte in a pair corresponds to rows 12-11-0-1-2-3 and bits 5-0 of the second byte correspond to rows 4-5-6-7-8-9. Bit 6 of each byte provides odd parity for the byte.

# APIs (rough)

A card image has 80 16-bit columns, with bit positions corresponding to rows as follows:

|15 |14 |13 |12 |11    |10    |9    |8    |7    |6    |5    |4    |3    |2    |1    |0    |
|:-:|:-:|:-:|:-:|:----:|:----:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|`0`|`0`|`0`|`0`|**12**|**11**|**0**|**1**|**2**|**3**|**4**|**5**|**6**|**7**|**8**|**9**|

This 16-bit format will be called Hollerith and can be set directly in the card image.
CBN can be read/written as two bytes.

Binary row data is treated as 24 36-bit values starting at row 9 column 1.