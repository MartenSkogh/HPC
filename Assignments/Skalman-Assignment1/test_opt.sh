#!/usr/bin/env bash

# Loop for the time measurements
ALLOPTFLAGS="-O0 -O1 -O2 -O3 -Os -Og -Ofast"

for OPT in $ALLOPTFLAGS
do
    echo "=== Using flag $OPT: ==="

    EXE="main$OPT"
    ASS="main$OPT.s"

    # Run the files
    ./$EXE

    # Count lines of the assembly files
    nlines=$(wc -l < $ASS)
    echo "$nlines lines in $ASS"
done
