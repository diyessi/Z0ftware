# Output formats

The assembled program could be in several formats on tape or cards.

Several assembler output formats were used.
[IBM Modle-704 Guidebook page II-11, 17](https://escholarship.org/content/qt4cn1c702/qt4cn1c702.pdf) described pseudo-ops `ABS` and `FUL` for controlling output format.

[IBM 7090 FAP](http://www.bitsavers.org/pdf/ibm/7090/C28-6235-2_7090_FAP.pdf#page=67) described relocatable binaries.
[Retrocomputing](https://retrocomputing.stackexchange.com/questions/19574/why-have-both-the-org-and-loc-pseudo-operations-in-the-share-symbolic-fortran-as) describes `ORG` versus `LOC`
[IBM 7090/7094 Fortran II under monitor, pasge 16](https://bitsavers.org/pdf/ibm/7090/C28-6066-6_FORTRANII_oper.pdf) describes card formats for BSS loader.

Coupled with these formats are the tape/card formats used by the emulators.