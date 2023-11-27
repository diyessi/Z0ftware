# Running a job on the IBM 704

Modern computers start executing instructions about as soon as they have power and continue executing instructions for about as long as they have power. The first instructions are in some kind of non-volatile read-only memory and start the process of making the CPU and boot devices usable, eventually getting to the point where they can capture user input that indicates an application program is to be run, create a process for the program an let the process initialize memory based on information in the application program file and arguments from the user and then transfer control to the application program.

When an IBM 704 is turned on, no instructions start executing. Core memory is non-volatile and will retain the contents it had when powered off, but it is ordinary read/write memory. An operator can use the front panel to put a value in the `MQ` register or specify an instruction (not an address for an instruction) to execute, or the operator can press one of the three `LOAD` keys, one for card unit 0, one for tape unit 0, and one for drum unit 0. This will execute the following four instructions:

```
       RDS dev
       CPY 0
       CPY 1
       TRA 0
```

The value of `dev` depends on which `LOAD` key is pressed. The `RDS` begins the reading of a record from the device. The first two words of the record are copied into addresses 0 and 1 and then execution is transferred to address 0. It is not known if the four instructions are copied into the first four words of memory and then executed normally, or if they are instead executed directly. On simh the are executed directly and not copied into memory.

## A simple self-loading program

A program that can be started this with the `LOAD` key is called a "self-loading program." A self-loading program must be able to complete its work with words 0 and 1 of the first record copied to addresses 0 and 1, the input device positioned at word 2 of the first record.

A simple self-loading program uses the two-word program to load the rest of a program that can copy the real program into memory and then execute it. For example, a self-loading program that loads 7 into `AC` and halts would be:

```
                              ORG 0
                              FUL
00000  0 53400 4 00000        LXA *,4
00001  0 70000 4 00002        CPY *+1,4
00002  1 77777 4 00001        TXI *-1,4,-1
00003  0 00000 0 00003        HTR *
00004  0 50000 0 00006 START  CLA SEVEN
00005  0 00000 0 00005        HTR *
00006  0 00000 0 00007 SEVEN  DEC 7
```
The format is the address (in octal) is in the left column. This is followed by the contents of the address, written in octal as prefix, decrement, tag, address. The prefix is written in a "sign magnitude" format, where the sign is `-` if the high bit is 1. Thus, a prefix whose binary is `101` would be written as `-1` rather than `5`.

The assembler source follows the instruction. In the source, there is a three character opcode followed by up to three comma-separated expressions for address, tag and decrement. For most instructions, the opcode extends into the decrement.

A non-infix `*` is a pseudo-symbol for the current address. Although not used here, `**` is a pseudo-symbol for `0` and is used to document parts of an instruction that will be modified during program execution. Negative address and decrement values are stored using twos complement.

The `ORG 0` is an assembler psedo-op that starts a new section at address 0. The `FUL` is another pseudo-op that tells the assembler to produce output in in as many 24 word records as are needed. The first record will have the first 24 words of the section, the second record the next 24 words, etc.

When the `LOAD` sequence begins, `RDS dev` is executed, which starts a read of the first drum record, or the next tape record or card, depending on the device. This means all `CPY` instructions will read from that record. There is no buffering, so `CPY` instructions must be executed before the device has moved on to the next word. However, a `CPY` instruction will block if the next word is not yet available on the input device.

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
This process will continue until 20 more words have been read. This makes 24 total words, two from the `LOAD` key plus the two iterations of the loop already performed.
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


The loop continues copying the remaining words in the record to locations 4, 5, etc. On the 25th `CPY` the record has ended so `CPY` returns to the `CLA 6` in address 4, the beginning of the three word application program that begins at `START` and ends at `SEVEN`. The `END START` is a required pseudo-op for the assembler.

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
                              END START
```

In this self-loading program, adress 4 contains `RCD` which is a pseudo-op for an `RDS` for the card reader. When the first record (card) has finished being read, rather than going into the application program, we start reading the next record/card. The contents of this card will be copied to addresses starting with 100 (octal 144), but the copy loop is otherwise the same as the one for the initial loop. When the record has finished being read, control transfers to the application program.

# Loaders

Arbitrary jobs can be run by forming a "deck" of a self-loading loader followed by the application program in a loader-specific format, followed by program input. Booting loads the self-loading loader, with the next record being the first record of the application program. The loader reads all the the application program records leaving the input device positioned at the beginning of the application input when control is transferred to the application.

Input devices were not always reliable so records would often include a checksum. If the record checksum did not match what was loaded, the loader would halt.

Although a modern 704 assembler can compile a program more or less instantly, an assembler running on the 704 took noticeable time. Machine costs were high, so assembling or compiling was also expensive. The last record of a program is a "transfer card" which contains the address to transfer control. The transfer card also supports corrections, which are address/value pairs to be updated in memory after the original program is loaded. The transfer of control does not happen until after the corrections have been applied.

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