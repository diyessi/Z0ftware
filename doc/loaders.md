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

The general form of an assembler instruction is

```
LABEL  OPR ADDRESS,INDEX,DECREMENT COMMENT
```

The ```LABEL``` is up to 6 characters. Spaces will be removed. The ```OPR``` is the three character operation code. The ```ADDRESS,INDEX,DECREMENT``` are up to three expressions. The ```INDEX``` and ```DECREMENT``` default to 0.

The first ```ORG 0``` begins the card, setting the location to 0. The ```FUL``` tells the assembler to produce binary cards suitable for the load key. The first two real instructions on the card will be ```LXA *,4```  and ```CPY *+1,4``` which the load key will have placed into storage locations 0 and 1. The load key sequence then transfers to address 0, which will actually be ```LXA 0,4``` so index 4 will be set to the address field, 0. The ```CPY 2,4``` will copy the next value from the card to the address `2 - C(IDX4)`.