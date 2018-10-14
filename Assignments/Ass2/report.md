## Introduction 
An important aspect of high performance computing is parallelization.  This assignment was to write a program that performs a task using POSIX threads so that it is fast enough to pass a number of requirements. 
The task is to solve equations with Newton’s method with different starting positions in the complex plane and then generate two images of type .ppm. The first being related to what root one converges to for each point, the second being related to how many iterations needed for convergence for each point. 

## Program layout
All code is written in the file newton.c. 

TODO: WRITE ABOUT HEADERS AND STUFF

First the input arguments are parsed. 

Then memory is allocated for the two matrices used for building the images. These are rootMatrix and iterMatrix. Every element in rootMatrix stores a number between 1 and d which corresponds to one of the exact roots to the equation and every element in iterMatrix stores the number of iterations needed to find the root. 

The exact roots are calculated and stored and are given an associated colour. For this purpose we make use of a Colour struct which stores three integers r, g and b aswell as an ascii string with those values.  


The program writes the images row by row. In order to keep track of which rowes are ready to write or needs to be calculated, an array with an element for every row is created. It is filled with zeros at first which indicates that none of the rows are ready to be written. 

~~~
  rowDone = (int*) malloc(sizeof(int)*dimensions);
  for (i = 0; i < degree; ++i) {
    rowDone[i] = 0;
  }
~~~

The writer thread is created and assigned the function writeRows. 

~~~
 pthread_t writerThread;
  int ret = 0;
  if (ret = pthread_create(&writerThread, NULL, writeRows, NULL)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
~~~

The function runWorkerThreads is called. This function creates the threads used for the computations and makes them calculate rows until they are all done.

When the threads are done they are joined, the allocated memory is freed and the main function is done.



 which in addition to the main function has five more functions. 
1. mul_cpx which given two complex number structs returns the product of the two as a complex number struct.
2. findRoot: findRoot takes two indices and constructs a complex number in accordance with a grid mapping a part of the complex plane. It then finds the root to the equation by using Newton’s method with the complex number as starting point. The root is identified as one of the exact roots and the corresponding position is rootMatrix is set to the right root number and the corresponding position in iterationMatrix is set to the number of iterations it took to converge. 
3. computeRows: In computeRows, a thread finds the next row that is yet to be computed. When a row which is marked as available, with a zero in the rowDone array, the threads marks that the row is being computed by changing the zero to a one. It then finds the root for every point within that row by looping  through the column indices and calling findRoot. When the row is calculated it is marked as done with a two in the rowDone array and the thread moves on to the next row until every row is done or in progress.

4. runWorkerThreads: runWorkerThreads creates the number of threads specified by the user and assign the all to the function computeRows.

5. writeRows: writeRows is the function performed by the writer thread. The writer thread writes the headers of the two files and then waits for the first row to be computed before it can write the results to the files. It then waits for the second row and so on by checking the rowDone array. This is done until all rows are written. In the while lopp, a the nanosleep function is called if the current row is not done yet. The function then closes the files. 

##Results

##Performance discussion

###Synchronisation 
The synchronisation was made using the array rowsDone. With this, the absolute worst thing that could happen was that a row could be calculated more than once and that was deemed highly unlikely. The writing thread did all the file managing and was forced to write in the correct order. All other computations by the threads were completely independent.
