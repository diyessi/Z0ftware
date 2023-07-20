# Loading programs

Although it is possible to put a program into memory by using switches on the control panel to enter the program in binary one address and word at a time, it would be extremely tedious. The 704 provides three keys, *Load Card*, *Load Tape* and *Load Drum* to start a program loading from a card, tape, or drum. The *Load card* key performs the following instructions:

```
       RDS 145 SELECT CARD READER
       CPY 0
       CPY 1
       TRA 0
```
The ```RDS 145``` selects the card reader, which will cause ```CPY``` instructions to read from the card reader. The two ```CPY``` instructions put columns 1-36 of row 9 into address 0 and columns 37-72 into address 1. This is followed by a branch to 0. The card reader remains selected until it times out, so near future ```CPY``` instructions will continue reading from the card. The only difference between loading from card and loading from tape or drum is the device used in the ```RDS```.

In normal operations, the load keys are not used. Instead, a loader program already in memory will read cards, perform checksums, store the instructions and data in the appropriate memory locations and start running the program at the appropriate location. Compilers and assemblers normally produce cards in the formats required by loaders, but the ```FUL``` assembler pseudo-op to produce binary cards used by a load key.

The file `artifacts/uasap.sap` contains the SAP assembler source, which begins with a loader:

```
       REM   DRUM CALL CARD                                             SAP30015
       ORG 0                                                            SAP30016
       FUL                                                              SAP30017
       LXA *,4                                                          SAP30018
       CPY *+1,4                                                        SAP30019
       TXI *-1,4,-1                                                     SAP30020
           DRML1,,DRUM1+DCLAS                                           SAP30021
       CLA *-1                                                          SAP30022
       ARS 18                                                           SAP30023
       ORS *+1                                                          SAP30024
       RDR **                                                           SAP30025
       PXD ,-                                                           SAP30026
       LDA *-6                                                          SAP30027
       LXD *+2,4                                                        SAP30028
       CAD 0                                                            SAP30029
       TXI ,,PRE2-PRE1-1                                                SAP30030
       REM   DRUM CONTROL CARD                                          SAP30031
       ORG 0                                                            SAP30032
```

The *Load card* key will place the first two instructions in location 0 and 1 and transfer to 0, so memory will look like:

```
00000 LXA 0,4
00001 CPY 2,4
``````

and the card reading will be positioned at

```
      TXI 1,4,-1
```

The ```LXA 0,4``` will copy the address part of word 0, which happens to be itself with address portion 0, to index register 4. When index registers are used, their content will be subtracted from the address part of the instruction.

Next, ```CPY 2,4``` will read the next word from the card, i.e. ```TXI 1,4,-1```, and store it in location `2-C(IDX[4]) = 2` just in time for it to be the next instruction executed. There are no instruction cache issues to worry about since there are no caches.

The ```CPY``` instruction has a conditional built into it. Normally it continues with the next instruction, but if the end of file is reached (card reader out of cards) it continues with the instruction after that, while on end of record (all 24 values have been read from the card) it continues with the instruction third after the ```CPY```. Since there are 24 words on the card, neither end of file nor end of record will occur at this time.

The ```TXI 1,4,-1``` will transfer back to 1, the ```CPY``` and decrement index register 4 at the same time; it will contain `77777` (Note: this is specific to an i704 with 32,768 words or memory) which is a twos complement -1.

This time the ```CPY 2,4``` will write address 3. This is the address for the end of file condition for the ```CPY```, but it is being used as a constant under the expectation that the ```CPY``` will never see an end of file. Since the end of card condition will break out of the card reading loop, this expectation is true.

When the card is finished the ```CPY``` will transfer control to 4 and memory will contain

```
00000 LXA 0,4
00001 CPY 2,4
00002 TXI 2,4,-1
00003 000 DRML1,,DRUM1+DCLAS
00004 CLA 3
00005 ARS 18
00006 ORS 7
00007 RDR 0
00010 PXD 0,0
00011 LDA 3
00013 LXD 15,4
00014 CAD 0
00015 TXI ,,PRE2-PRE1-1
00016 HTR 0

```

In 4, the constant in 3 is loaded into AC. This constant is actually two 15-bit constants packed into one word, ```DRML1``` in the address and ```DRUM1+DCLAS``` in the decrement part of the word. The symbol ```DCLAS``` is a base for the four drum device numbers. Adding ```DRUM1``` gives the number for drum 1.

 Continuing with instruction 6, the device number is OR'd into the word in address 7, effectively setting the address part of the instruction to drum 1. The operation ```RDR```, *Read Drum*, is an alias for ```RDS```, *Read Select*, so drum 1 is not selected for reading.

 