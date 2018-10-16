#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <stdatomic.h>

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define DEBUG 0
#define PROGRESS 1

#define MAX_DISTANCE 3465 // maximum distance given that all points are within [-10, 10]. Calculated by ceil(sqrt(20^2 + 20^2 + 20^2) * 100)

int *distances;
int nbrOfThreads;

int dist(int *point1, int *point2)
{
  float x = (float) (point1[0] - point2[0]);
  float y = (float) (point1[1] - point2[1]);
  float z = (float) (point1[2] - point2[2]);
  return (int) (sqrt(x*x + y*y + z*z)/10);
}

int readBlock(int** block, int blockSize)
{
  // Read until the number of lines is equal to blockSize or we reach the end of the file. Return number of lines that were read
  return 0;
}

void compute_inner_distances(int** block, int numElements)
{
  //Parallelize
  for (int i = 0; i < numElements - 1; ++i)
    for(int j = i + 1; j < numElements; ++j)
      ++distances[dist(block[i], block[j])];
}

void compute_cross_distances(int **block1, int **block2, int numElements1, int numElements2)
{
  for (int i = 0; i < numElements1; ++i)
    for(int j = 0; j < numElements2; ++j)
      ++distances[dist(block1[i], block2[j])];
}

void write_distances()
{
  FILE *file = fopen("output.txt", "w+");
  for (int i = 0; i < MAX_DISTANCE; ++i)
  {
    if (distances[i] == 0)
      continue;
    fprintf(file, "%.2f, %d", ((float)(i))/100, distances[i]);
  }
  fclose(file);
}

int main(int argc, char *argv[]) {
  size_t i, j;

  // Parse command line arguments
  for ( i = 1; i < argc; ++i) {
    if (strncmp(argv[i],"-t",2) == 0) {
      nbrOfThreads = atoi(argv[i]+2);
    }
  }

  if(DEBUG)
    printf("t: %d\n", nbrOfThreads);
  
  // Figure out block size
  int blockSize = 10; // do something smart here

  // Allocate memory
  puts("Initializing stuffs...");
  distances = (int*)malloc(sizeof(int)*MAX_DISTANCE);
  for (i = 0; i < MAX_DISTANCE; ++i)
    distances[i] = 0;

  int **block1 = (int**) malloc(sizeof(int*) * blockSize);
  int *block1Values = (int*) malloc(sizeof(int) * blockSize * 3); 
  int **block2 = (int**) malloc(sizeof(int*) * blockSize);
  int *block2Values = (int*) malloc(sizeof(int) * blockSize * 3); 
  for (i = 0, j = 0; i < blockSize; ++i, j+=3) {
    block1[i] = block1Values + j;
    block2[i] = block2Values + j;      
  }
  
  printf("Starting...\n");
  // figure out numBlocks somehow 
  int numLines1 = blockSize;
  int numLines2 = blockSize;
  while (numLines1 == blockSize)
  {
    // TODO: need some way to tell readBlock where to start reading
    numLines1 = readBlock(block1, blockSize);

    // compute distances within one block
    compute_inner_distances(block1, numLines1);

    // End the loop if we read the last block
    if (numLines1 < blockSize)
      break;

    // compute distances between blocks
    while (numLines2 == blockSize)
    {
      // TODO: need some way to tell readBlock where to start reading
      numLines2 = readBlock(block2, blockSize);
      if (numLines2 == 0)
        break;
      compute_cross_distances(block1, block2, numLines1, numLines2);
    }
  }

  // Risk for race condition, mitigated with useof atomic variables
  // if (PROGRESS) {
  //   puts("");
  //   struct timespec mainSleep = {0, 100000};
  //   while((nbrRowsCompleted < dimensions) || (nbrRowsWritten < dimensions)) {
  //     printf("Calculation Progress: %d %% ", (nbrRowsCompleted*100)/dimensions);
  //     printf("Writing Progress: %d %%\r", (nbrRowsWritten*100)/dimensions);
  //     nanosleep(&mainSleep, NULL);
  //   }
  // }

  printf("Starting write to file");
  write_distances();

  puts("Freeing memory...");
  free(block1);
  free(block1Values);
  free(block2);
  free(block2Values);
  
  printf("Finished!\n");
  return 0;
}
