#!/usr/bin/env bash


ALLOPTFLAGS="-O0 -O1 -O2 -O3 -Os -Og -Ofast"

for OPT in $ALLOPTFLAGS
do
    echo "Using flag $OPT"

    EXE="main$OPT"
    ASS="main$OPT.s"
    OBJ="main$OPT.o"

    make clean OPTFLAG=$OPT TARGET=$EXE OBJS=$OBJ
    make clean_ass OPTFLAG=$OPT ASSTARG=$ASS

done
