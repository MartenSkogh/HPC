# HPC Assignemt 3 - OpenMP

## Introduction
The goal of this assignment was to implement a program that reads a number of positions in space from a file, calculates all the distances between points, counts how many times each distance occurs and write out the distance and the number of occurrences to stdout. The program shall also be parallelized using OpenMP and be fast enough to pass some requirements. 

## Making and Running the Program

Make the program by running 
`$ make`  
The program is compiled with gcc and optimization flag -O3.  
To run the program enter  
`$ ./cell_distance -t{number of threads} -b{size of reading block} -f{position file name}`  

## Program Overview
All code is written in the file cell_distance.c. The program flow is as follows:

1. Parse input arguments. 
2. Allocate memory and open the file.
3. Combine reading of positions and calculation of distances block-wise. 
4. Write distances and number of times each distance occurrs. 
5. Free memory and exit.

## Limiting the memory usage

The program was not allowed to consume more than 1 GiBi byte of memory at a given time. In order to fulfill this criterion, the positions in the file are read in blocks. The program has a variable determining the size of each block (as a number of rows). The size has a default value but can be set by the user using the flag `-b`. If the number of rows in the file is less or equal to the block size, all data is read at once. If the number of rows in the file is larger than the block size, the following happens:

1. A first block, block1, is read from the file.
2. All distances in block1 are calculated and counted.
3. A second block, block2, is read from the file and the cross-distances between every position in block1 and block2 are calculated and counted.
4. New blocks are read into block2 and processed until the end of the file is reached.
5. A new block1 is read and (2.-4.) is repeated until every distance has been calculated. 

The size in bytes of every row is known since it has a defined format. And the default block size times two (one for each block) times the size of a row will be smaller than 1 Gibi byte. Thus the memory requirement is fulfilled. 

## Detailed Execution

The distances were to calculated with one decimal precision (with some rounding error tolerated). Another simplification was the range of the coordinated that were assumed to be [-10,10]. This made the number of possible distances manageble and to keep count over the distances, an integer array called `distances`, with every index corresponding to a distance, was allocated and set to zero. HOW IS `NBR_POSSIBLE_DISTANCES´ DEFINED MATH STUFF.

The blocks, `block1` and `block2`, were allocated as double float arrays.  

To keep track over where in the file the blocks "are", the integers `blockPosition1` and `blockPosition2` are used. The block-reading loop is a w double while-loop. A new `block1` is read for as long as `blockPosition1` is not a multiple of `blockSize`, i.e. when the latest read block is at the end of the file. A similiar condion is used for the inner while-loop thats loops over different `block2`. FUNCTIONS CALLED; NUMLINES; STORED IN DISTANCE[calculated distance * 100] OR SOMETHING.

