# Output formats

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

From [IBM Modle-704 Guidebook page II-2](https://escholarship.org/content/qt4cn1c702/qt4cn1c702.pdf):

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