# HPC Assignemt 4 - OpenCL

Authors:   
_Philip Edenborg_   
_Adam Tonderski_  
_Mårten Skogh_  

## Introduction
The goal of this assignment was to implement a simple model for 2D heat diffusion. The calculations were to be performed on the GPU of the system using OpenCL. 

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
5. For every iteration, set the read and write buffer arguments of the diffusion kernel and enqueue the diffusion kernel.
6. Create and enqueue the summation kernel.
7. Calculate the average temperature by summing the result vector from the previous step and dividing with the number of elements. 
8. Create and enqueue the kernel for average absolute difference with the value computed in step 7.
9. Calculate the average absolute difference as in step 7 but on thematrix updated with absolute differences.

### Output

The program outputs the average temperature and the average absolute temperature difference to `stdout`.

## Program description

### Required files
The only files that are required to compile the program are:
 
 1. `stdlib.h` - Standard definitions
 2. `stdio.h` - for printing to stdout and to write to files.
 3. `string.h` - to use the string data type.
 4. `math.h` - for mathimatical functions.
 5. `CL/cl.h` - to use OpenCL. 
 
### Kernels
The kernels used by the program are described here.

1. `__kernel void heat_step(__global double * restrict read, __global double * restrict write, double c)`: Performes a diffusion step for an element in the 2D space. The previous values are read from `read` and written to `write`. The kernel gets indices via `get_global_id`. `c` is the diffusion constant.
2. `__kernel void sum(__global double* restrict matrix, __local double * restrict scratch, int len, __global double* restrict result)`: Reduces a vector of length 'len' to a shorter vector with partial sums. The length of this shorter vector is determined by the number of work groups (the quota between global size and local size). The complete sum can be calculated on the CPU by adding together all elements of 'result'. During the reduction, a memory barrier is used with the code `barrier(CLK_LOCAL_MEM_FENCE);` to handle synchronization within the local group. 
3. `__kernel void absdiff(__global double * restrict matrix, double subtractor)`: Calculates an element of the matrix of all absolute differences. The current position is calculated using `get_global_id`, the difference is calculated by substracting the average, `substractor`, and the absolute value of the difference is stored. 

## Results
Here, the output results and timing results are presented for different input arguments. The benchmarking is done by running several times and taking the average runtime. For the size 10000*10000, the program was run 5 times and for the others it was run 100 times. 

`$ ./heat_diffusion w h -i{V} -d{D} -n{N}  

| w*h  | V   | D      | N | Time  | Target time     | Average temp. | Average abs. diff.  |
|-----:|:---:|:------:|:-:|:-----:|:---------------:|:-------------:|:-------------------:|
|100*100    | 1e20 | 0.01 | 100000 | 0.8 s  | 1.7 s | 9062970323610388 | 7078330452703392 | 
|10000*10000| 1e10 | 0.02 | 1000   | 24.9 s | 98 s  | 100              | 200              |
|100000*100 | 1e10 | 0.60 | 200    | 0.63 s | 1.4 s | 1000             | 1999             |

All runtimes are below the maximum runtimes defined in the assignment.

## Performance comments 
Two changes were nescessary in order to make the program run fast enough. The first thing was to use `restrict`. The second thing was to set an appropriate local size. Setting the local size in a quadratic fashion to `{25, 25}` had a huge impact on performance. It was also noted that the program was not able to enqueue the kernel for some larger local sizes like for instance `{50, 50}` for the cases with w\*h = 10000\*10000 and w\*h = 100000\*100. We do not know the reason for this. 

