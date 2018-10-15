# Assignmnet 2 - High Performance Computing

## Making and Running the Program

Make the program by running  
`$ make`  
then run the program with  
`$ ./newton`  

## Program Layout
Our program is divided into several functions and we also utilize several of the standard C headers.

The program uses several threads to compute the roots of a polynomial on a square subset of values around the complex zero. There is also two additional threads, the main thread and a second one used for writing the results to a file.

### Required files
The only files that are required to compile the program are:
 
 1. stdlib.h - Standard definitions
 2. stdio.h - for printing to stdout and to write to files.
 3. string.h - to use the string data type.
 4. math.h - for mathimatical functions.
 5. pthread.h - for POSIX threads.
 6. errno.h - translating error numbers.
 7. time.h - for timing and thread sleeping.
 8. stdatomic.h - for atomic variables, protected against race conditions.

### Functions

`mul_cpx`  
Performs complex multiplication of two complex numbers.

`findRoot`  
Performs the Newton-Rhapson method to find the root to which a gicǘen starting point converges.

`computeRows`  
Function passed to a new thread that will compute the roots for values in a given row of the input matrix.

`runWorkerThreads`  
Starts threads and gives them the data they are supposed to start work on.

`writeRows`  
Function passed to the writer thread. 

`main`  
Main function, called on program start.

### Data Structures

**Complex**  
A complex number consisting of two ints _re_ and _im_, the real and imaginary part. 

**Colour**
A data structure representing a color value. Represents the color in RGB format as both three integers 0 < _r_, _g_, _b_ < 255, and as a character array with the same values saved as a single char array.

## Performance
