# HPC Assignemt 4 - OpenCL

Authors:   
_Philip Edenborg_   
_Adam Tonderski_  
_Mårten Skogh_  

## Introduction
The goal of this assignment was to implement a simple model of 2D heat diffusion. The calculations were to be performed on the GPU of the system using OpenCL. 

## Making and Running the Program

Make the program by running 
`$ make`  
The program is compiled with gcc and optimization flag -O3.  
To run the program enter  
`$ ./heat\_diffusion {width of matrix} {height of matrix} -i{initial central value} -d{diffusion constant} -n{number of iterations}`  

## Program Overview

All code is written in the file heat\_diffusion.c. The program flow is as follows:

1. Parse input arguments. 
2. Find the GPU device ID, create context and command queue.
3. Allocate matrix and create buffers for the diffusion. 
4. Build CL-program and create kernel for diffusion.
5. For every iteration, set the read and write buffer arguements of the diffusion kernel and enqueue the kernel.
6. Create and enqueue the sumation kernel.
7. Calculate the average temperature. 
8. Create and enqueue the kernel for average absoute difference.
9. Calculate the average absolute difference.

### Output

The program outputs the average temperature and the average absolute temperature difference to `stdout`.

## Program description

### Required files
The only files that are required to compile the program are:
 
 1. `stdlib.h` - Standard definitions
 2. `stdio.h` - for printing to stdout and to write to files.
 3. `string.h` - to use the string data type.
 4. `math.h` - for mathimatical functions.
 5. `cl/CL.h` - to use OpenCL. 
 
### Functions

### Parallelization 

## Results
Here, the output results and timing results are presented for different input arguments. The benchmarking is done by running several times and taking the average runtime. For the size 10000*10000, the program was run 5 times and for the others it was run 100 times. 

`$ ./heat_diffusion w h -i{V} -d{D} -n{N}  

| w*h  | V   | D      | N | Time  | Target time     | Average temp. | Average abs. diff.  |
|-----:|:---:|:------:|:-:|:-----:|:---------------:|:-------------:|:-------------------:|
|100*1000   | 1e20 | 0.01 | 100000 | 0.8 s  | 1.7 s | 9062970323610388 | 7078330452703392 | 
|10000*10000| 1e10 | 0.02 | 1000   | 24.9 s | 98 s  | 100              | 200              |
|100000*100 | 1e10 | 0.60 | 200    | 0.63 s | 1.4 s | 1000             | 1999             |

All runtimes are below the maximum runtimes defined in the assignment.

