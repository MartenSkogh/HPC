# HPC Assignemt 3 - OpenMP

Authors:   
_Philip Edenborg_  
_Adam Tonderski_  
_Mårten Skogh_  

## Introduction
The goal of this assignment was to implement a program that reads a arbitrary number of three dimensional cooridnates (positions) in space from a file and that calculates all distances between all points (with precission of two decimal values), counts how many times each distance occurs and write out the distance and the number of occurrences to stdout. The program shall also be parallelized using OpenMP and be fast enough to pass some requirements. 

## Making and Running the Program

Make the program by running 
`$ make`  
The program is compiled with gcc and optimization flag -O3.  
To run the program enter  
`$ ./cell\_distance -t{number of threads} -b{size of reading block} [-f{/path/to/positions}]`  
 where the -f flag is optional, the default behavior is to search for a file called cells in the current working directory.

## Program Overview

All code is written in the file cell_distance.c. The program flow is as follows:

1. Parse input arguments. 
2. Allocate memory and open the file.
3. Combine reading of positions and calculation of distances block-wise. 
4. Write distances and number of times each distance occurrs. 
5. Free memory and exit.

### Input

The input to the program is a ASCII text file with three values on each line. Each value is to be seperated with a space character. Each value `x` has to be in the range `-10 > x > 10`, and the text format of each value has to be on the form `snn.nnn` where `s` is either a plus (+) or minus (-) sign, `n` is a decimal digit 0-9. No character can be left out, each value has to be six (6) characters long. 

Example input file containing 10 positions:
~~~
+01.330 -09.035 +03.489
-03.718 +02.517 -05.995
+09.568 -03.464 +02.645
-09.620 +09.279 +08.828
+07.630 -02.290 +00.679
+04.113 -03.399 +05.299
-00.994 +07.313 -06.523
+03.376 -03.614 -06.657
+01.304 +09.381 -01.559
-04.238 -07.514 +08.942
~~~

### Output

The program outputs to `stdout` a string of both distance and number of occurences where the distance and number of occurences are seperated by a single space character and the different distances are seperated by a newline. The ouptut is formated as `(n)n.nn` where the total length of the string is either four or five characters, there is no padding for distances below 10.

The output values are floored to closest value with two (2) decimal points.

Example of the 15 first lines of the output given the input from the above section:
~~~
2.99 1
5.54 1
5.84 1
5.91 1
6.06 1
6.54 1
7.94 1
8.58 1
9.39 1
9.59 1
9.64 1
9.98 1
9.99 1
11.17 1
11.68 1
...
~~~

## Limiting the memory usage

The program was not allowed to consume more than 1 GiBi byte of memory at a given time. In order to fulfill this criterion, the positions in the file are read in blocks. The program has a variable determining the size of each block (as a number of rows). The size has a default value but can be set by the user using the flag `-b`. If the number of rows in the file is less or equal to the block size, all data is read at once. If the number of rows in the file is larger than the block size, the following happens:

1. A first block, block1, is read from the file.
2. All distances in block1 are calculated and counted.
3. A second block, block2, is read from the file and the cross-distances between every position in block1 and block2 are calculated and counted.
4. New blocks are read into block2 and processed until the end of the file is reached.
5. A new block1 is read and (2.-4.) is repeated until every distance has been calculated. 

The size in bytes of every row is known since it has a defined format. With the default block size of 1e4, the memory requirements for both blocks are roughly \$$ 4 * 10^4 * 3 * 2 $$ bytes which is 240 kB. This is obviously much smaller than 1 GiB and thus the memory requirement is fulfilled.

## Program description

The distances were to be calculated with two decimals precision (with some rounding error tolerated). Another simplification was the range of the coordinated that was assumed to be [-10,10]. This made the number of possible distances manageble and to keep count over the distances, an integer array called `distances`, with every index corresponding to a distance, is allocated and set to zero. The number of possible distances are calculated as the maximum distance times 100 (for two decimals)
rounded upward, ceil(sqrt(20^2 + 20^2 + 20^2)\*100).

The blocks, `block1` and `block2`, are allocated as 2d float arrays.  

To keep track over where in the file the blocks "are", the integers `blockPosition1` and `blockPosition2` are used. The block-reading loop is a w double while-loop. A new `block1` is read for as long as `blockPosition1` is not a multiple of `blockSize`, i.e. when the latest read block is at the end of the file. A similiar condion is used for the inner while-loop thats loops over different data in `block2`. 

The function `readBlock`, which in`block1` and `block2` are read, returns the number of lines read in the variables `numLines1` and `numLines2` respectively. These are used to increment the block-position variables and to determine whether to break the while-loops. If `numLines1` at some time is smaller than `blockSize`, the last `block1` has been read. Similary, if `numLines2` is zero, the last `block2` has been read. 

The distances within a block are computed using the function `compute_inner_distances` with `block1` and the number of lines in the block, `numLines`, as input arguments. The distances between blocks are computed using `compute_cross_distances` for every instance of `block2`.

When all blocks have been read and evaluated, the function `ẁrite_distances` is called to print the results. When a distance is calculated it is multiplied by 100, converted to an integer and stored in the corresponding position in `distances`. For example, a distance 3.213 will be stored in `distances[321]`. When the distances and occurences then are printed, the indices are printed after being converted to floats and divided by 100. Distances that that does not occur in the data set are ignored. 

### Required files
The only files that are required to compile the program are:
 
 1. `stdlib.h` - Standard definitions
 2. `stdio.h` - for printing to stdout and to write to files.
 3. `string.h` - to use the string data type.
 4. `math.h` - for mathimatical functions.
 5. `omp.h` - to use OpenMP. 
 
### Functions
In addition to our `main` function, several other functions are used. These are listed below:

1. `int readBlock(float** block, int numPoints, FILE *fp, long int startLine)`: Finds the right position in the file pointer `fp` using `fseek` with an offset `startLine*CHARACTERS_IN_LINE*sizeof(char)`. It then reads one line at a time using `fread` and calls the function `parseLine` to put the data in `block`.
2. `void parseLine(float* destination, char* line)`: Parses the chars in `line` to floats and stores them in `destination` (a position in block)
4. `int dist(float *point1, float *point2)`: Calculates the cartesian distance between `point1` and `point2` and multiplies is with 100.
5. `void compute_inner_distances(float** block, int numElements)`: Loops through `block` using `numElements` and updates the global array `distances` using the function `dist`.
6. `void compute_cross_distances(float **block1, float **block2, int numElements1, int numElements2)`: Calculates the distances between every point in `block1` and every point in `block2` using the number of elements in the blocks,`numElements1`and `numElements2`. The global array `distances` is updated using the function `dist`.
7. `void write_distances()`: Prints ´((float)i)/100´ and `distances[i]` for every index `i` of `distances`.

### Parallelization 
The parallelization is done using OpenMP. In the the two functions for calculating distances, the double for-loops are parallelized using reduction 

~~~C
#pragma omp parallel for reduction(+:distances[0:NBR_POSSIBLE_DISTANCES])
for (int i = 0; i < numElements - 1; ++i)
    for (int j = i + 1; j < numElements; ++j)
        ++distances[dist(block[i], block[j])];
~~~
 Reduction is used because it is fast. An alternative which was tested was
~~~C
#pragma omp parallel for shared(distances)
for (int i = 0; i < numElements - 1; ++i)
    for (int j = i + 1; j < numElements; ++j)
        #pragma omp atomic
        ++distances[dist(block[i], block[j])];
~~~
This was however more than ten times slower than reduction.

## Results
Here, the timing results are presented for different number of positions and number of threads. The benchmarking is done by running 100 times and taking the average runtime. 

`$ ./cell_distance -t{T} -fcell_{N}`  

| T    | N   |  Time  | Target Time |
|-----:|:---:|:------:|:------------|
|1     | 1e4 | 0.27 s | 0.41        |
|5     | 1e5 | 4.40 s | 8.2         |
|10    | 1e5 | 2.90 s | 4.1         |
|20    | 1e5 | 1.23 s | 2.6         |

As can be seen in the table, all runtimes are below the maximum runtimes defined in the assignment and are actually close to half of the target times.

