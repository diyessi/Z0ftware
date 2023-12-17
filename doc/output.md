# Running a job on the IBM 704

Modern computers start executing instructions about as soon as they have power and continue executing instructions for about as long as they have power. The first instructions are in some kind of non-volatile read-only memory and start the process of making the CPU and boot devices usable, eventually getting to the point where they can capture user input that indicates an application program is to be run, create a process for the program, initialize its memory based on a description of the application program file and arguments from the user, and then transfer control to the application program.

When an IBM 704 is turned on, no instructions start executing. Core memory is non-volatile and will retain the contents it had when powered off, but it is ordinary read/write memory. An operator can use the front panel to put a value in the `MQ` register or specify an instruction (not an address for an instruction) to execute, or the operator can press one of the three `LOAD` keys, one for card unit 0, one for tape unit 0, and one for drum unit 0. Pushing the `LOAD CARD` key will cause the computer to act is if execution were started at location 0 after initializing the first four words of memory to:

```
00000  0 76200 0 00321        RCD     is RDS 209 (0321)
00001  0 70000 0 00000        CPY 0
00002  0 70000 0 00001        CPY 1
00003  0 02000 0 00000        TRA 0
```
[Listing format](listings.md)

The `RCD` operation is an "extended operation" that is the same as the `Read Select` instruction `RDS 209` which selects and starts the card reader for reading one record (card). The device is specified in the address field of the instruction. The `LOAD TAPE` and `LOAD DRUM` keys use the device for the first tape and drum respectively.

After the `RDS` executes, words are read with `CPY`, (Copy and Skip) which must execute before the device times out. If executed before a word is ready, `CPY` will block until a word is available. The `CPY` instruction is somewhat complicated. When a device has been selected for read (`RDS`), a `CPY` at location `L` will:
 - If too much time has elapsed, the computer halts.
 - If there is a word to be read in the record, it is copied to the `MQ` reguster and then written to the indicated address. Execution continues with the next instruction, `L+1`.
 - If there is no media in the device, execution continues at `L+2`. Once one `CPY` has been successful after an `RDS` there will be media in the device for the rest of the record.
 - If there are no more words in the record, execution continues at `L+3`. For a card, this will occur on the 25th `CPY`. The first 24 executions of a `CPY` will always succeed in reading a word unless there is a timeout.

The boot sequence assumes there is a card in the card reader, copies the first two words of the card into locations `00000` and `00001` and transfers execution to `00000`.

Cards are read row by row, starting at row `9` on the bottom and continuing through rows `8-0` and then `11-12`. The first word is from columns `1-36` (`L`) and the second from columns `37-72` (`R`). Columns `73-80` cannot not be read by the 704 without rewiring the plug panel. The words are labeled `9L`, `9R`, `8L`, `8R`, ..., `12R`.

NOTE: The 704 might not actually implement the keys by setting memory. The simh emulator executes the instructions directly without changing memory.

## A simple self-loading program

A program that can be started this with a `LOAD` key is called a "self-loading program." A self-loading program must be able to complete its work with words 0 and 1 of the first record copied to addresses 0 and 1, the input device positioned at word 2 of the first record.

A simple self-loading program uses the two-word program to load the rest of a program that can copy the real program into memory and then execute it. For example, a self-loading program that loads 7 into `AC` and halts would be:

```
                              ORG 0
                              FUL
00000  0 53400 4 00000        LXA 0,4
00001  0 70000 4 00002        CPY *+1,4
00002  1 77777 4 00001        TXI *-1,4,-1
00003  0 00000 0 00003        HTR *
00004  0 50000 0 00006 START  CLA SEVEN
00005  0 00000 0 00005        HTR *
00006  0 00000 0 00007 SEVEN  DEC 7
                              END
```
The `ORG 0` is a pseudo-op that tells the assembler to treat the next instruction as though it is in location 0 in memory. The `FUL` is a pseudo-op that tells the assembler to write its output for each section in 24 as many word records as are needed for the section. Each record will have exactly 24 wordsin the order they appear in the section, but the records will have no meta-information about memory locations or how many actual words there should be.

The use of non-infix `*` in an expression substitutes the current location for the instruction for the `*`. For example, in `CPY *+1,4` the location is `00001` so `*+1` is `00002` as seen in the address part of the instruction.

When the `LOAD CARD` sequence begins, `RCD` is executed, which starts a read of the first card. This means all `CPY` instructions will read from that record. There is no buffering, so `CPY` instructions must be executed before the device has moved on to the next word. However, a `CPY` instruction will block if the next word is not yet available on the input device.

When the `LOAD` sequence executes `CPY 0` it transfers the first word of the record to address 0, so memory will contain
```
00000  0 53400 4 00000        LXA 0,4
```
The `LOAD` sequence next executes `CPY 1`, transferring the next word of the record to address 1. Memory will contain
```
00000  0 53400 4 00000        LXA 0,4
00001  0 70000 4 00002        CPY 2,4
```
Finally, the `LOAD` sequence executes `TRA 0` which sets the instruction counter (`IC`, the PC) to 0, which will be indicated with `>`:
```
>00000  0 53400 4 00000        LXA 0,4
 00001  0 70000 4 00002        CPY 2,4
```
Now the instruction at address 0, `LXA 0,4`, is executed. This sets index register 4 (`IR4`) to the address part of the word at address 0, which is 0. If the instruction had been `LXA 1,4` it would have set index register 4 to the address part of the word at address 1, which would be 2. The state is now:
```
 00000  0 53400 4 00000        LXA 0,4
>00001  0 70000 4 00002        CPY 2,4
IR4 = 0
```
Now, `CPY 2,4` in address 1 is executed. This instruction specifies `IR4` which was just set. When an index register is specified, its value is *subtracted* from the specified address. So far, we have been treating the copy instruction like an ordinary instruction that increments the instruction counter. In addition to reading the next word from a device record and copying it to the specified address, it also sets the `MQ` register to the read value and acts like a conditional:
 - If the record has not already ended, execution continues on the next instruction, in this case at address 2,
 - If the record has ended, execution continues two instructions later, in this case at address 4,
 - If there is no record, for example no card, execution continues one instruction later, i.e. at address 3. This can only happen on the first read after a `RDS` so this is not possible here since there have already been successful two reads in the record.
In this case, there is a word in the record to copy, so the `CPY` will proceed to the next instruction, which it has just put at address 2. There are no instruction caches to worry about on the 704:
```
 00000  0 53400 4 00000        LXA 0,4
 00001  0 70000 4 00002        CPY 2,4
>00002  1 77777 4 00001        TXI 1,4,-1
IR4 = 00000
```
The `TXI` subtracts one from `IR4` and branches back to address 1.
```
 00000  0 53400 4 00000        LXA 0,4
>00001  0 70000 4 00002        CPY 2,4
 00002  1 77777 4 00001        TXI 1,4,-1
IR4 = 77777 (-1)
```
Since `IR4 = -1` the next word of the record will be copied to address 3:
```
 00000  0 53400 4 00000        LXA 0,4
 00001  0 70000 4 00002        CPY 2,4
>00002  1 77777 4 00001        TXI 1,4,-1
 00003  0 00000 0 00003        HTR 3
IR4 = 77777 (-1)
```
The `HTR` instruction halts execution and sets the IC to its address field, in this case 3. This instruction is the one that would follow a `CPY` that encountered no more records after the `RDS`. This shouldn't be able happen in this example, but, if it does happen somehow, examinig the IC will show where the program halted.

The `TXI` again adds -1 to `IR4` and branches back to the `CPY`:
```
 00000  0 53400 4 00000        LXA 0,4
>00001  0 70000 4 00002        CPY 2,4
 00002  1 77777 4 00001        TXI 1,4,-1
 00003  0 00000 0 00003        HTR 3
IR4 = 77776 (-2)
```
This process will continue until 20 more words have been read. This makes 24 total words, two from the `LOAD` key plus the two iterations of the loop already performed. In `FUL` binary format, the record beyond addresses where the source stops will be empty, i.e. contain zeros, so the entire range of `00000` through `00027` will be overwritten, even though only `00000` through `00006` contain the program.
```
 00000  0 53400 4 00000        LXA 0,4
>00001  0 70000 4 00002        CPY 2,4
 00002  1 77777 4 00001        TXI 1,4,-1
 00003  0 00000 0 00003        HTR 3
 00004  0 50000 0 00006        CLA SEVEN
 00005  0 00000 0 00005        HTR *
 00006  0 00000 0 00007        SEVEN  DEC 7
 00007  0 00000 0 00000
 ...
 00027  0 00000 0 00000
IR4 = 77752 (-22)
```
This time the `CPY` will not read a word since the record was completed. Instead, it will continue execution with the instruction at address 4, which is the main program.

## A two card self-loading program

As a second example, we modify the previous self-loading program to move the application program to a second card.

```
                              ORG 0
                              FUL
00000  0 53400 4 00000        LXA *,4  
00001  0 70000 4 00002        CPY *+1,4
00002  1 77777 4 00001        TXI *-1,4,-1
00003  0 00000 0 00003        HTR *
00004  0 76200 0 00321        RCD
00005  0 53400 4 00000        LXA 0,4
00006  0 70000 4 00144        CPY START,4
00007  1 77777 4 00006        TXI *-1,4,-1
00010  0 00000 0 00010        HTR *
00011  0 02000 0 00144        TRA START
                              ORG 100
00144  0 50000 0 00147 START  CLA SEVEN
00145  0 40000 0 00150        ADD ONE
00146  0 00000 0 00146        HTR *
00147  0 00000 0 00007 SEVEN  DEC 7
00150  0 00000 0 00001 ONE    DEC 1
```
The `ORG 100` after address `00011` starts a new assembly section that will start in memory at address 100 decimal, or octal `00144`. The assembler will start a new record, continuing to use `FUL` format. The first record will still be 24 words and the first `CPY` loop will write into `00000` through `00027` as before.
```
 00000  0 53400 4 00000        LXA 0,4  
 00001  0 70000 4 00002        CPY 2,4
 00002  1 77777 4 00001        TXI 1,4,77777
 00003  0 00000 0 00003        HTR 3
>00004  0 76200 0 00321        RCD
 00005  0 53400 4 00000        LXA 0,4
 00006  0 70000 4 00144        CPY 144,4
 00007  1 77777 4 00006        TXI 6,4,77777
 00010  0 00000 0 00010        HTR 10
 00011  0 02000 0 00144        TRA 144
 00012  0 00000 0 00000
 ...
 00027  0 00000 0 00000
```
This time when the first record has been read, control transfers to a `RCD` (read card device) which starts the process for reading the second record. This is similar to the read of the first record, except that the destination starts at `00144` instead of `00000` and the `CPY` will copy all 24 words of the record rather than just 22.

# Loaders

The first programs would have been written as self-loading programs, but it would not have been long before someone realized a standard self-loading loader program could be used if the assembler output records contained information about where in memory each record was to go and what address the program should start at. The loader card(s) could be placed before the program binary, which would be followed by program input. Alternatively, an already running program could contain a loader that loaded additional programs/data.

The most basic loader needs to support two operations:
 1. Copy `count` words in a record to locations starting as `address`.
 2. Transfer control to `address`.

In addition, storage of records on cards, tape and drum was not all that reliable, so a checksum was added, which was the unsigned sum of all the words on the card other than the checksum.

The first word in the record (row 9, left side on a card, `9L`) holds `address` in its `address` field for copies and transfers. The `decrement` field holds the count for copies; if the count is 0, the record describes a transfer. The second word in the records (row 9, right side of the card, `9R`) holds the checksum.

The basic format was extended to:
 - Allow the checksum to be ignored,
 - Corrections to be applied to loaded data. This let bugs be patched without needing to take the time to reassemble the program. Not a good practice when assembling is cheap, but necessary in the days when it was costly.
 - Data to be loaded to load-time specified addresses with addresses swizzled by the loader to correspond to actual locations.

## NYBL1

An early example of a basic loader is `NYBL1`, here copied from a listing in [CODING for the MIT-IBM 704 COMPUTER, pages XII-15 to XII-16](https://bitsavers.org/pdf/mit/computer_center/Coding_for_the_MIT-IBM_704_Computer_Oct57.pdf) with annotations from the listing in the coding guide added as comments:
```
                 00000        ORG 0
                              FUL
00000  0 53400 4 00000        LXA 0,4         / These words are copied to 0 and 1 by
00001  0 70000 4 00002 A      CPY 2,4         \ the load card sequence.
00002  1 77777 4 00001 9R     TXI A,4,-1      / This copy loop brings the rest of the
                              REM             | card to core-memory and terminates on
                              REM             \ the end-of-record skip
00003  0 00000 0 00000 9L     PZE               Used to store 9 left row.
00004 -0 76000 0 00007        LTM             / We leave the trapping mode (just in case)
                              REM             | and enter the loader proper. The TXI in
                              REM             \ register 9R is right in any case.
00005  0 76200 0 00321 RCD    RCD               Read binary card
00006  0 70000 0 00003        CPY 9L            x and n to 9L and MQ
00007  0 76300 0 00021        LLS 17            n from MQ to AC
00010  0 73400 4 00000        PAX 0,4           n to IR4
00011  0 40000 0 00003        ADD 9L          / 
00012  0 62100 0 00015        STA TRA         | x + n to TRA, CPY and ACL
00013  0 62100 0 00020        STA CPY         |
00014  0 62100 0 00021        STA ACL         \
00015 -3 00000 4 00000 TRA    TXL **,4          exit to x if transfer card
00016  0 70000 0 00002        CPY 9R            Check sum to 9R
00017 -0 50000 0 00003        CAL 9L          / 
00020  0 70000 4 00000 CPY    CPY **,4        | Words to x, x+1, ..., x+n-1
00021  0 36100 4 00000 ACL    ACL **,4        | Form a new check sum
00022  2 00001 4 00020        TIX CPY,4,1     \
00023  0 60200 0 00003        SLW 9L            New check sum to 9L
00024  0 50000 0 00003        CLA 9L          /
00025  0 40200 0 00002        SUB 9R          |
00026  0 10000 0 00005        TZE RCD         | Stop if check sums disagree
00027  0 00000 0 00005        HTR RCD         \
```
The loop in `00000` through `00002` is similar to the previous self-loaders examples and will leave the 24 words in the record in addresses `00000` through `00027`, matching the addresses above. On the next iteration through the loop, `CPY 2,4` will be at the end of the record and return to `00004`. 

Location `00003` will never be reached since a `CPY` instruction has already suceeded after the `RCD` in `LOAD CARD` sequence so there is no chance of the card vanishing partly through the record. Location contains a `PZE` which is the psedo-op "plus zero" and is used to put a 0 in location `00003`. `HTR 0` would do the same, but `PZE` documents the intent to use the location as a variable, `9L`, rather than as an instruction. The notation `9L` names the left word in row 9 columns 1-36 on a card. The label for location `00002` is `9R`, which is the name of the word on the right side of row 9, columns 37-72. Why would an instruction have a label like a variable? Memory was scarce and once the card has been copied into memory that instruction will not be executed, so it can be used as a variable for later parts of the loader. This was common enough to be obvious; it wasn't noted in the annotations.

After the end of record, the `CPY 2,4` will continue with locatoin `00004`, `LTM`, "leave trap mode".  Trap mode is used for debugging and causes branches to halt the computer. 

In `00005` a read is started for the next card, the first one of the actual program. Recall that in `00006` `CPY` puts the read value in both the `MQ` register and the indicated address, `9L`.

In `00007`, `LLS` is a "long left shift." Shifts and rotates on the 704 can be a little complicated. For `LLS`, the sign of `MQ` replaces the sign of `AC` and then the `AC` bits `Q,P,1-35` are joined to the `MQ` bits `1-35` and shifted left by the value in the low 8 bits of the address. Thus position `i` in `MQ` ends up in position `35-shift+i` in `AC` for `i<=shift`. In other words the decrement of `MQ` ends up in the address of `AC`. In `00010` this is stored in `IR4`. This will be the count of the number of words to be copied from the cars. The address field in the `PAX` instruction is not used, so it is sometimes used as a place to store an integer.

In `00011`, `ADD 9L` adds the first word on the card to its decrement. The address field of `9L` contains the start address for the data to be copied from the card, so this produces the end address for the card. This is stored at locationss `TRA`,`CPY` and `ACL`. If the count is 0, this is a "transfer card" and control will be transferred to the address part of `9L`. Otherwise count words will be copied and checksummed against the word in `9R`.

In `00015` the `TXL **,4` has has `**` replaced by the address plus the count of `9L`. `TXL` transfers if the index register is less than or equal to the decrement. The unspecified decrement will be `0`, so if the count is `0` execution transfers to what was the address part of `9L` moving from loading to program execution.

In `00016` the next word of the card is read into `9R`. This is the checksum.

Location `00017` is the entry for the loop copying data into memory. It sets `AC` to `9L` which is to be included in the checksum. The loop then copies each word in the record to the next address (end of data minus `IR4`), performs an unsigned add to the running checksum in `AC`, and loops back for the next word, decrementing `IR4` until `IR4` is 1.

The unsigned computed checksum in `AC` is stored in `9L` and brought back as a signed word, from which the required checksum is subtracted. If the result is `0`, the process repeates with the next card, otherwise the computer halts.

## The FORTRAN II Loader

The boot loader for the final version of the FORTRAN II compiler is a tape-based loader that is used to produce a bootable FORTRAN II compiler tape for a particular memory size from a master tape . As with cards, the `RDS` read select operation is used to start reading a tape record. Tapes can be written in binary or decimal (character) format. The device number specified in the read select determines the format and tape unit. Unlike cards, tape records can have any number of words, so there is no need to indicate the record length in a control word. As described in [THE FORTRAN II AUTOMATIC CODING SYSTEM FOR RHW IBM 704, I.2.B](https://bitsavers.org/pdf/ibm/704/704_FORTRAN_II_OperMan.pdf), the loader expects a two word header in a record to be loaded. The first word is a checksum and the second is a control word whose address field is the address to start loading into, and whose decrement contains the transfer address.

```
                              REM 704 FORTRAN SELF LOADING RECORD 1 TO CS.
                              FUL
00000  0 53400 1 00000        LXA 0,1
00001  0 70000 1 00002        CPY 2,1
00002  1 00001 1 00001        TXI 1,1,1
00003  0 70000 1 00031        CPY 25,1
00004  0 00000 0 00003        HTR 3
00005  0 10000 0 00000        TZE 0
00006  0 76000 0 00006        COM
00007  0 36100 0 00002        ACL 2
00010  0 76000 0 00006        COM
00011  0 02000 0 00027        TRA 23
00012 -0 76000 0 00012        RTT
00013  0 76600 0 00333        IOD
00014  0 00000 0 00000        HTR 0
00015  1 77777 1 00015        TXI 13,1,-1
00016 -0 70000 1 00000        CAD 0,1
00017 -0 50000 0 00017        CAL 15
00020  0 62100 0 00026        STA 22
00021  0 77100 0 00022        ARS 18
00022  0 62100 0 00015        STA 13
00023 -0 50000 0 00017        CAL 15
00024  0 70000 0 00017        CPY 15
00025  0 70000 0 00002        CPY 2
00026  0 76200 0 00221        RTB 1
00027 -0 53400 1 00027        LXD 23,1
00030  0 70000 0 00003        CPY 3
00031 -0 76000 0 00007        LTM
00032  0 76400 0 00441        BST 145
                              END
```
This loader contains 26 words, so it would not fit on a card. It contains no labels, and, if you skim it to try to understand what it is doing, it won't make any sense.

The boot sequence selects the first tape unit for binary read, copies the first two words into locations `00000` and `00001`, and transfers to `00000`:
```
>00000  0 53400 1 00000        LXA 0,1
 00001  0 70000 1 00002        CPY 2,1
```
This time, index register 1 is used instead of index register 4, and it is initialized with `00000`. The next word is read into `00002`:
```
 00000  0 53400 1 00000        LXA 0,1
 00001  0 70000 1 00002        CPY 2,1
>00002  1 00001 1 00001        TXI 1,1,1
 IR1 = 00000
```
The previous loaders added `-1` to the index register, but `TXI 1,1,1` increments `IR1` and branches to `00001`:
```
 00000  0 53400 1 00000        LXA 0,1
>00001  0 70000 1 00002        CPY 2,1
 00002  1 00001 1 00001        TXI 1,1,1
 IR1 = 00001
```
Recall that the value in the index register is *subtracted* from the address field, so `CPY 2,1` will put the next word into location `00001`, not in `00003`, that is the instruction in `00003` in the listing replaces the instruction `00001` in memory:
```
 00000  0 53400 1 00000        LXA 0,1
 00001  0 70000 1 00031        CPY 25,1
>00002  1 00001 1 00001        TXI 1,1,1
 IR1 = 00001
```
`IR1` is again incremented, now to `00002`, and execution continues at the new `00001`:
```
 00000  0 53400 1 00000        LXA 0,1
>00001  0 70000 1 00031        CPY 25,1
 00002  1 00001 1 00001        TXI 1,1,1
 IR1 = 00002
```
The next word, `00004` in the listing, will be copied to `25-2`, i.e. location `00027`:
```
 00000  0 53400 1 00000        LXA 0,1
 00001  0 70000 1 00031        CPY 25,1
>00002  1 00001 1 00001        TXI 1,1,1
   .
   .
   .
 00027  0 00000 0 00003        HTR 3
 IR1 = 00002
```
Copying will continue backwards until all 24 words on the card have been read:
```
 00000  0 53400 1 00000        LXA 0,1
>00001  0 70000 1 00031        CPY 25,1
 00002 -0 53400 1 00027        LTM
 00003  0 76400 0 00441        BST 145
 00004 -0 53400 1 00027        LXD 23,1
 00005  0 76200 0 00221        RTB 1
 00006  0 70000 0 00002        CPY 2
 00007  0 70000 0 00017        CPY 15
 00010 -0 50000 0 00017        CAL 15
 00011  0 62100 0 00015        STA 13
 00012  0 77100 0 00022        ARS 18
 00013  0 62100 0 00026        STA 22
 00014 -0 50000 0 00017        CAL 15
 00015 -0 70000 1 00000        CAD 0,1
 00016  1 77777 1 00015        TXI 13,1,-1
 00017  0 00000 0 00000        HTR 0
 00020  0 76600 0 00333        IOD
 00021 -0 76000 0 00012        RTT
 00022  0 02000 0 00027        TRA 23
 00023  0 76000 0 00006        COM
 00024  0 36100 0 00002        ACL 2 
 00025  0 76000 0 00006        COM
 00026  0 10000 0 00000        TZE 0
 00027  0 00000 0 00003        HTR 3
 IR1 = 00027
```
The `CPY 25,1` in `00001` is now at the end of the record so execution transfers to `00004` with `LXD 23,1`. This sets `IR1` to the decrement of location `00027`, which is `00000`. The loader is now boot-loaded and ready to load.

The `RTB 1` in `00005` is a read select for the first tape drive, to start reading the next record in binary mode. Two words are read; the first is stored in `00002` and the second in `00017`. The first word in the record, the checksum, is stored in `00002`. The second word, the control word, is stored in `00017`. In `00011` the address in `00015 CAD 0,1` is changed to the address from the control record. `CAD` is an undocumented instruction that combines a `CPY` and a `CAL` so that words can be read and checksummed at the same time. In `00012` the `ARS 18` puts the decrement, the transfer address, in the address position.

Locations `00015-00016` are a tight loop copying words to memory and checksumming them. When the record ends, execution will continue at `00020`, `IOD`. This is an undocumented instrution that waits for pending IO operations since `MQ` is unstable until the IO completes. In `00021` the `RTT` redundanct tape test checks for tape problems and resets their indicators. The `RTT` can return to `00022` or `00023`. Since `00022` transfers to `00023`, the status from `RTT` is ignored.

In `00023` the running checksum in `AC` is complemented by `COM` (not including the `S` bit, which will have been `0` all along) and added to the desired checksum. If the checksums were the same, this would result in all ones. The `COM` in `00025` will turn all the ones to all zeros. If `AC` is `0`, the `TZE` in `00026` will branch to the transfer address that was stored in `00026` by the the `STA` in `00013`. Otherwise there is a halt. Continuing the halt backspaces the tape one record.

## Sections

The assembler breaks the program up into one or more contiguous address sections. The pseudo-op `ORG addr` begins a new section starting at the address `addr`.

## Output Formats

The pseudo-ops `FUL`, `ABS` and `REL` are used to change the format of the output records for a section. The `FUL` output format has already been described.

The `ABS` and `REL` and correction/transfer record formats all start with a control word followed by a checkword for the record. The control word specifies whether the record is absolute or relocatble and whether the card is data to be copied to memory, correction/transfer, or some other kind of data.

The description here corresponds to the SAP assembler. Over time, adjustments were made to the control word. To further confuse matters, some documentation refers to bits by card column while other documentation uses bit positions. Column `1` corresponds to bit position `S`, column `2` to bit position `1`, column `3` to bit position `2`, etc. Here fields will be designated by bit positions, `S`, `1-35`.

### Control Word
9S : 0
9L1 : Enable relocate
9L2 : Ignore CKS, disable transfer
9L12 : Origin table
9L13-17 : Word Count V
9L18-20 : 0
9L21-35 : Initial location R

9LS1 == 0 : Absolute data




The assembled program could be in several formats on tape or cards.

Several assembler output formats were used.
[IBM Modle-704 Guidebook page II-11, 17](https://escholarship.org/content/qt4cn1c702/qt4cn1c702.pdf) described pseudo-ops `ABS` and `FUL` for controlling output format.

[IBM 7090 FAP](http://www.bitsavers.org/pdf/ibm/7090/C28-6235-2_7090_FAP.pdf#page=67) described relocatable binaries.
[Retrocomputing](https://retrocomputing.stackexchange.com/questions/19574/why-have-both-the-org-and-loc-pseudo-operations-in-the-share-symbolic-fortran-as) describes `ORG` versus `LOC`
[IBM 7090/7094 Fortran II under monitor, page 16](https://bitsavers.org/pdf/ibm/7090/C28-6066-6_FORTRANII_oper.pdf) describes card formats for BSS loader.

[CODING for the MIT-IBM 704 COMPUTER](https://bitsavers.org/pdf/mit/computer_center/Coding_for_the_MIT-IBM_704_Computer_Oct57.pdf)

[Reference Manual 709/7090 FORTRAN Programming System](https://archive.computerhistory.org/resources/text/Fortran/102663112.05.01.acc.pdf)

[Reference Manual 7090/7094 FORTRAN Operations pg. 15-18](https://bitsavers.org/pdf/ibm/7090/C28-6066-6_FORTRANII_oper.pdf)


Coupled with these formats are the tape/card formats used by the emulators.

From [IBM Modle-704 Guidebook page II-2](https://escholarship.org/content/qt4cn1c702/qt4cn1c702.pdf)

[SHARE REFERENCE MANUAL for the IBM 704, August 1956, pages 3.10 - 02 to 3.10 - 05](https://www.piercefuller.com/scan/share59.pdf)

The `NY AP 1` assembler code samples shown in [704 electronic data-processing machine manual of operation, pages 73-79](https://bitsavers.org/pdf/ibm/704/24-6661-2_704_Manual_1955.pdf) do not indicate how they were loaded.


# Full cards (boot)

Full binary cards containt 24 words. The device boot switch will cause a device read is started and then the first two words, 9L and 9R (or first six binary tape characters), will be loaded into addresses 0 and 1 followed by a transfer to 0.

# Binary Symbolic Subroutine Loader (BSS Loader)

Row 9 1-72 define the format of remainder of the card:

1-3 prefix
4-18 dec
19-21 tag
22-36 addr
37-72 checksum

If c==1 or checksum==0 ignore checksum

00x,0,0,addr,x: Absolute transfer to x
00c,count,x,addr,checksum: Absolute inst, addr to addr + count
01c,count,x,addr,checksum: Relocatable inst
01x,0,0,addr,checksum: Relocatable transfer
10c,0,0,0,checksum : Transfer card
10c,count,x,0,checksum: transfer list, ignore checksum


[00x]

Card bits are designated as {0-9,10-12}{L|R}{S,1-35|D}
9L is the "control word" and 9R is 36-bit ACL checksum
9L13-17: Word count V (max is 22)
9L21-35: Initial location R
9L2: Ignore checksum

A transfer card has 9L.addr=0, 9L.dec= branch location (Coding for MIT-IBM 704 computer, XII-16)

Absolute data:
9L1: 0

Relocatable data
9L1: 1
V is nominal initial location (subject to relocation)
Row 8 has "indicator bits" starting on left (note Huffman encoding)
0: Absolute field
10: relocatable direct (uncomplemented) field
11: relocatable complemented field
7L,7R,6L,6R, ... are the relocatable data

From [IBM Modle-704 Guidebook page II-11](https://escholarship.org/content/qt4cn1c702/qt4cn1c702.pdf):
Pseudo OPs
ABS: Absolute binary format
Emits binary information in current format, then switches format to absolute binary format.

BOS T: Binary output select
Write binary in current format
If T=0 or blank, switch to card punch for future output
If T=1-10, switch to logical tape T

END: Write binary information

FUL: Full binary format
Write binary in current format
Switch to 24 words/card

REL: Relocatable binary formast
Write binary data, switch to relocatable format