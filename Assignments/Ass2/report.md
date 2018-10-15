# Assignment 2 - High Performance Computing

## Introduction 
An important aspect of high performance computing is parallelization.  This assignment was to write a program that performs a task using POSIX threads so that it is fast enough to pass a number of requirements. 
The task is to solve equations with Newton’s method with different starting positions in the complex plane and then generate two images of type .ppm. The first being related to what root one converges to for each point, the second being related to how many iterations needed for convergence for each point. 

## Making and Running the Program

Make the program by running 
`$ make`  
The program is compiled with gcc and optimization flag -O3.  
To run the program enter  
`$ ./newton -t{number of threads} -l{side length} {polynomial degree}`  

## Program Overview
All code is written in the file newton.c. The program flow is as follows:

1. Parse input arguments. 
2. Allocate memory.
3. Pre-calculate root values and assign colors to each root
4. Start single writer thread that writes colors of roots to PPM file as they are calculated.
5. Start worker threads that finds the roots of each value for a row at a time. The number of threads started is given by the input argument `-t`.
6. Wait for all threads to finish, free memory and exit.


### Detailed Execution
In the `main` function the first thing happening is the parsing of the command line input arguments. From these the number of threads, image size, and degree of polynomial is assigned.

Next memory is allocated for the two matrices used for building the images. These are `rootMatrix` and `iterMatrix`. Every element in `rootMatrix` stores a number between 1 and d which corresponds to one of the exact roots to the equation and every element in `iterMatrix` stores the number of iterations needed to find the root. 

The exact roots are calculated and stored and are given an associated color. For this purpose we make use of a `Colour` data structure which stores three integers `r`, `g` and `b` aswell as an ascii char array, named `ascii`, with those values. This lets us easily color the root s found during the calculations.

The program handles the data, both writing and calculating, row by row. A list of the statuses of all rows, `rowDone`, is used to keep track of which rows are left to be calculated and which are left to be written to the PPM-image file. The entry value 0 corresponds to a un-handled row, 1 is a row that is being calculated, and 2 is a row that is ready for writing to image file. All values of `rowDone` are initialized to 0.

The writer thread is created and assigned the function `writeRows`. This thread then waits for the worker threads to finish with rows in the order `0,1,2,...,n`. That is, if row 25 is finished before the previous rows, then the thread cannot write row 25 until all rows before have been completed. 

Then the function `runWorkerThreads` is called. This function creates the threads used for the computations. Each thread runs the Newton-Rhapson method on a single row until roots have been calculated for all values in that row. When a thread is finished with the row, the thread sets the corresponding value in `rowDone` to 2 and looks for another available row.

When the threads are done they are joined, the allocated memory is freed and the main function is done. 

### Required files
The only files that are required to compile the program are:
 
 1. `stdlib.h` - Standard definitions
 2. `stdio.h` - for printing to stdout and to write to files.
 3. `string.h` - to use the string data type.
 4. `math.h` - for mathimatical functions.
 5. `pthread.h` - for POSIX threads.
 6. `errno.h` - translating error numbers.
 7. `time.h` - for timing and thread sleeping.
 8. `stdatomic.h` - for atomic variables, protected against race conditions.

### Functions

In addition to our `main` function, several other functions are used. These are listed below:

1. `mul_cpx`: given two `Complex` number structs calculates and returns the product of the two as a `Complex` number struct.
2. `findRoot`: findRoot takes two indices and constructs a complex number in accordance with a grid mapping a part of the complex plane. It then finds the root to the equation by using Newton-Rhapson’s method with the complex number as starting point. The root is identified as one of the exact roots and the corresponding position in `rootMatrix` is set to the found root number and the corresponding position in `iterMatrix` is set to the number of iterations it took to converge. 
3. `computeRows`: finds the next available row that has not yet beem computed. When a row which is marked as available, with a zero in the `rowDone` array, is found the thread marks that the row as being computed by changing `rowDone[i]` from 0 to 1. It then finds the root for every point within that row by looping  through the column indices and calling findRoot. When the row is calculated it is marked as done with a two in the rowDone array and the thread moves on to the next row until every row is done or in progress.
4. `runWorkerThreads`: creates the number of threads specified by the user and assign the all to the function computeRows.
5. `writeRows`: the function performed by the writer thread. The writer thread writes the headers of the two files and then waits for the first row to be computed before it can write the results to the files. It then waits for the second row and so on by checking the rowDone array. This is done until all rows are written. In the while lopp, a the nanosleep function is called if the current row is not done yet. The function then closes the files. 

## Results
The following results were obtained by running the program on gantenbein on the ssd multiple times and taking the average. The reason for using the ssd was that writing to the home directory was highly unreliable with the time varying by at least a factor of 10 between runs. This could have been due to a high number of concurrent users. 

`$ ./newton -l{L} -t10 7`
| L     |  Time     |
|------:|:---------:|
|1000   | 0.077s  |
|50000  | 158.516s |

`$ ./newton -l1000 -t{N} 5`
| N     |  Time     |
|------:|:---------:|
|1      | 0.239s  |
|2      | 0.127s  |
|3      | 0.089s  |
|4      | 0.075s  |

`./newton -l1000 -t1 {D}`
| D     |  Time     |
|------:|:---------:|
|1      | 0.072s  | 
|2      | 0.099s  |
|5      | 0.241s  |
|7      | 0.401s  |

## Performance discussion
We were satisfied with the overall performance since it fit well within the provided range. Some optimisations were obvious given the course material, such as multiplying the complex numbers in a loop instead of using the build in pow function. However, there were some other areas of interest that will be discussed below.

### Bottlenecks
In all of our runs the bottleneck turned out to be writing to the disk. Initially we used fwrite where the values for each pixel were substituted for each write. Then we optimized that to storing the strings and and writing with fprintf without substitution. Finally we moved to frwite with precomputed strings and string lengths. Even with the final version the writing still took significantly longer than the computation. Often the computation was done when the writing was only half-ways. A further improvement would have been to create entire rows in memory as a single string and writing it all at once.

### Synchronisation 
The synchronisation was made using the array rowsDone. Initially a lock was used to prevent threads from accessing this array simultaneously but later that restriction was removed. We realized that the the absolute worst thing that could happen was that a row could be calculated more than once and that was deemed highly unlikely. The writing thread did all the file managing and was forced to write in the correct order. All other computations by the threads were completely independent.

