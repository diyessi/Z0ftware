# BCDIC Encodings

The BCD character encodings are derived from the encodings used for cards. The encodings for cards were developed for the electro-mechanical data processing equipment that preceded computers.

## Digits

Initially, there were 10 rows, numbered 0 through 9 from near the top of the card to the bottom. A punch in a row designated the corresponding digit. There was room at the top of the card for two additional rows, which were called *zone* rows, which could be punched for special purposes, such as indicating a sign.

## Alphabetic characters

To accomodate encoding alphabetic characters, the encoding was extended to include punches in two rows. With electro-mechanical equipment, this was easiest to do by using a combination of one zone row and one digit row. However, with only two available zones this would only support twenty additional characters. A third zone was needed and row 0, the row next to the two zone rows, was used. The available encodings consisted of a single punch in a digit row (0-9) or a combination of an extended zone punch and a digit punch in one of the non-zone digit rows (1-9). With two punches, row 0 had different semantics and was treated by the equipment more like a row 10, with rows 11 and 12 being the original zone rows, 12 being at the top of the card.

The characters `A` through `I` were encoded was punches `12-1` through `12-9`, `J` through `R` with `11-1` through `11-9`, and `S` through `Z` with `10-2` through `10-9`. The combination `10-1` may have been skipped to avoid adjacent holes. A column with no punches was a space.

## BCD for digits

The BCD encoding for digits uses four bits per digit, `0000` through `1001` for `0` through `9`.

## BCD for non-digits

Encoding letters and digits requires six bits per character. Following card terminolgy, the two additional bits are called zone bits and are in the high-order position. To simplify arithmetic with characters that are just digits, zone `00` is used for digits, i.e. `00 0000` through `00 1001` are used as the six bit encodings for the digits. Card zone `12` maps to BCD zone `11`, zone `11` to `10`, and zone `10` to `01`. Thus `A` is encoded as `110001`.

As described, the encodings `01 0000`, `10 0000` and `11 0000` are unnused since when a zone punch is used, the punched digit must be `1` through `9`. For zones `11` and `12`, a column with only a zone punch and no digit punch is treated as a though the digit component of the encoding is `0`. For zone `10`, which is also digit `0`, a lone zone punch is already interpreted as the character `0`. However, recall that a column with no holes punched is a space and needs an encoding. The encoding `00 0000` might feel natural for no holes punched, but `00 0000` is already the character `0`. However, the `01 0000` encoding that cannot correspond to any zone/digit punch is available and is used for space.

## Card encodings for remaining BCD encodings

At this point, all 40 possible blank, single punch and zone/digit punch card combinations have corresponding BCD encodings, but there are 64 possible BCD values, namely those of the form `** 1010` through `** 1111`. If we consider the subset of card encodings corresponding to BCD encodings `** 0***`, all we need is a way to indicate on a card that the `0` should be turned to a `1`, i.e. that the BCD encoding should be increased by `8`. This can be accomplished by adding card encodings with row `8` punch for all the card encodings with a digit punch less than `8`.

In [Reference Manual, IBM Magnetic Tape Units, 1961 on page 8, Standard BCD Interchange Code rows 25 and 35](https://bitsavers.org/pdf/ibm/magtape/A22-6589-1_magTapeReference_Jun62.pdf) , zones `12` and `11` are combined with digit `0`. Since a single zone punch already uses `0000` for the low four BCD bits, the `0` is treated as a `10` digit, so `12-0` is `11 1010`, which is also the encoding of `12-2-8`.

## BCD for tape

When the BCD encoding is used on tape, there is one additional complication. The end of a record is denoted by a region of blank tape. Tape is seven track, with the extra track being a parity track. For BCD data, even parity is used, so `00 0000` with parity is `0 00 0000` which is what is used for blank tape at the end of a record. A series of `0`s would be interpreted as the end of a record. When recorded on tape, `00 0000` is first converted to `00 1010` which is `0 00 1010` with even parity.

## BCD for computing

When computing with BCD, there is another complication. The BCD character encodings are not in alphabetical order. The letters `S-Z` are followed by `J-R` which are followed by `A-Z`. Evidently they hadn't been thinking of non-numeric operations when they mapped the card zones to the BCD zones and it was too late to change by the time the problem was realized. When tape BCD data is read/written by computer, BCD zones `01` and `11` are swapped, putting the alphabetic subset of encodings in alphabetic order.

## Characters represented by encodings

The encodings for the digits, uppercase alphabetic characters, and space are fairly consistent, but for other characters there is signicant inconsistency. Since IBM text began on cards, the key punch mappings are what would be used in programs. Some of these mappings changed with the introduction of the model 29 key punch. Parsers mapped the different encodings to the same semantics.

For example, on the `pr130-6` tape, 

```
       ...
       VFD     O6/15,30/QTCDE      ' - 029 QUOTE                     (6)4I613500    
       VFD     O6/16,30/EQCDE      @ - 029 EQUALS                    (6)4I613600    
       ...
       VFD     O6/35,30/LPCDE      ( - 029 LEFT PARENTHESIS          (6)4I615100    
       VFD     O6/36,30/PSCDE      + - 029 PLUS                      (6)4I615200    
       ...
       VFD     O6/55,30/RPCDE      ) - 029 RIGHT PARENTHESIS         (6)4I616700    
       ...
```

Comments, such as in `fb273`

```
       TXH     CB37N,1,7      IS DIGIT 8 OR 9?  YES.                 (A)5A619320    
```
indicate `11 1111` is a `?`.