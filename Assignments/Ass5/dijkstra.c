#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(a,b) \
     ({ __typeof__ (a) _a = (a); \
             __typeof__ (b) _b = (b); \
         _a < _b ? _a : _b; })

#define DEBUG 0
#define PROGRESS 1

void parse_node_file()
{
    puts("Parsing file...");
}


int main(int argc, char** argv)
{
    int i, j;
    int mpi_error;
    int from_node, to_node;
    char* node_file = NULL;

    puts("Starting...");

    // Parse command line arguments
    from_node = argv[1];
    to_node   = argv[2];
    node_file = argv[3];
    
    
    
    
    mpi_error = MPI_Init(&argc, &argv);



    mpi_error = MPI_Finalize();
    puts("Finished!");
    return 0;
}

