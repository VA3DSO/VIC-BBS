#!/bin/bash
set -e
clear

cl65 -t vic20 --config bbs.cfg -Cl -O -o bbs.prg -DEXP8K bbs.c common.c uutils.c futils.c
cl65 -t vic20 --config bbs.cfg -Cl -O -o games.prg -DEXP8K games.c common.c
cl65 -t vic20 --config bbs.cfg -Cl -O -o editor.prg -DEXP8K editor.c common.c futils.c
cl65 -t vic20 --config bbs.cfg -Cl -O -o files.prg -DEXP8K files.c common.c futils.c
cl65 -t vic20 --config bbs.cfg -Cl -O -o sysop.prg -DEXP8K sysop.c common.c uutils.c futils.c
#cl65 -t vic20 --config rsa.cfg -Cl -O -o rsa.prg -DEXP8K rsa.c sm_common.c
#cl65 -t vic20 --config rsab.cfg -Cl -O -o rsab.prg -DEXP8K rsab.c
c1541 bbs.dhd < inst.txt

