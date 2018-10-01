#!/usr/bin/env bash


ALLOPTFLAGS="-O0 -O1 -O2 -O3 -Os -Og -Ofast"

for OPT in $ALLOPTFLAGS
do
    echo "=== Using flag $OPT ==="

    EXE="main$OPT"
    OBJ="main$OPT.o"
    ASS="main$OPT.s"
    SRC="time.c"

    make $EXE OPTFLAG=$OPT OBJS=$OBJ TARGET=$EXE SOURCE=$SRC
    make $ASS OPTFLAG=$OPT OBJS=$OBJ ASSTARG=$ASS SOURCE=$SRC

done
