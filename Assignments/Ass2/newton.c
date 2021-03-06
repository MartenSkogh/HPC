#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdatomic.h>

#define WHITE_ITERATION_COUNT 50
#define RANGE 2.f
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define DEBUG 0
#define PROGRESS 1

// Complex number
struct Complex {
  double re, im;
};

// RGB color
struct Colour {
  int asciiLen;
  char *ascii;
};

// Define global variables  
int nbrOfThreads, dimensions, degree, blockSize;
int **rootMatrix, **iterMatrix;
int *rowDone; // 0 untouched, 1 inprogress, 2 done
struct Complex *exactRoots;
struct Colour *rootColours;
struct Colour greyColours[WHITE_ITERATION_COUNT]; // 50 shades of grey 
atomic_int nbrRowsCompleted = 0;
atomic_int nbrRowsWritten = 0;

// Complex multiplicaion of two complex numbers
void mul_cpx(struct Complex * a, struct Complex * b, struct Complex * c)
{
  struct Complex res;
  res.re = (*b).re*(*a).re - (*b).im*(*a).im;
  res.im = (*b).im*(*a).re + (*b).re*(*a).im;
  (*c).re = res.re;
  (*c).im = res.im;
}

// Newton-Rhapson method
void findRoot(int rowIndex, int colIndex) {
  struct Complex x;
  int i;

  if(DEBUG)
    printf("Finding roots...\n");
  
  x.re = -RANGE + (2.f*RANGE)/((float)(dimensions-1))*colIndex;
  x.im =  RANGE - (2.f*RANGE)/((float)(dimensions-1))*rowIndex;
  
  struct Complex fValue = {1.f, 0.f};
  int iterations = 0;
  // Square for positive
  while ( (fValue.re*fValue.re + fValue.im*fValue.im) > 1e-6 ) { 
    // Newton-Rhapson
    struct Complex dfTemp = {1.f, 0.f};
    
    for (i = 0; i < degree - 1 ; ++i) {
      mul_cpx(&dfTemp, &x, &dfTemp);
    }
    
    mul_cpx(&dfTemp, &x, &fValue);
    
    fValue.re -= 1;
    
    dfTemp.re *= degree;
    dfTemp.im *= -degree;
    
    struct Complex denominator;
    
    mul_cpx(&fValue, &dfTemp, &denominator);
    
    x.re = x.re - denominator.re/(dfTemp.re*dfTemp.re + dfTemp.im*dfTemp.im);
    x.im = x.im - denominator.im/(dfTemp.re*dfTemp.re + dfTemp.im*dfTemp.im);
    
    iterations = min(WHITE_ITERATION_COUNT, ++iterations);
    // one iteration more than necessary
  }
  
  iterMatrix[rowIndex][colIndex] = iterations;
  
  int bestInd = -1;
  float bestDist = 1e10;
  for (i=0; i < degree; ++i) {
    float newDist = (x.re-exactRoots[i].re)*(x.re-exactRoots[i].re) + (x.im-exactRoots[i].im)*(x.im-exactRoots[i].im);
    if (newDist < bestDist) {
      bestInd = i;
      bestDist = newDist;
    }
  }
  //printf("%d", bestInd);
  rootMatrix[rowIndex][colIndex] = bestInd;
}


void * computeRows(void *args) {
  int startRow = ((int*)args)[0];
  if(DEBUG)
    printf("New thread started on row %d!\n", startRow);
  for(int row = startRow; row < dimensions; ++row) {
    if(DEBUG)
      printf("Checking row %d", row);
    if (rowDone[row] != 0)
      continue;

    if(DEBUG)
      printf("Started work on row %d...\n", row);

    rowDone[row] = 1;
    for(int column = 0; column < dimensions; ++column) {
      //printf("Calling solver...\n");
      findRoot(row, column);
    }
    rowDone[row] = 2;
    if(PROGRESS)
      ++nbrRowsCompleted;
    if(DEBUG)
      printf("setting row %d to %d\n", row, rowDone[row]);

  }
//  puts("worker done");
  return NULL;
} 

void runWorkerThreads() {
  unsigned int ret = 0;
  pthread_t threads[nbrOfThreads];
  //printf("blockSize = %d\n", blockSize);
  for (size_t tx=0; tx < nbrOfThreads; ++tx) {
    int* arg = (int*)malloc(sizeof(int));
    arg[0] = tx;
    ret = pthread_create(threads+tx, NULL, computeRows, (void*)arg);
    int errorval = errno;
    if (ret) {
      switch (errno) {
        case EAGAIN:
          printf("Error EAGAIN\n");
        case EPERM:
          printf("Error EPERM\n");
        case EINVAL:
          printf("Error EINVAL\n");
        default:
          printf("Error kiss\n");
      }
            
      printf("Error creating thread: %\n", ret);
      exit(1);
    }
  }
}
  
void * writeRows(void *args) {
  puts("Initializing files...");
  FILE *rootFile;
  FILE *iterFile;
  char rootFileName[40];
  char iterFileName[40];
  sprintf(rootFileName, "newton_attractors_x%d.ppm", degree);
  sprintf(iterFileName, "newton_convergence_x%d.ppm",degree);
  puts("opening files");
  rootFile = fopen(rootFileName,"w");
  iterFile = fopen(iterFileName,"w");
  puts("writing header");  
  fprintf(rootFile,"P3\n%d %d\n255\n", dimensions, dimensions);
  fprintf(iterFile,"P3\n%d %d\n%d\n", dimensions, dimensions, WHITE_ITERATION_COUNT);  
 
  int row = 0;
  struct timespec sleepTime = {0, 1000};
  while (row < dimensions) {
    //printf("checking row %d", row);
    if (rowDone[row] != 2) {
      nanosleep(&sleepTime, NULL);
      continue;
    }
    
    // Write the file pixel content
    for (int column = 0; column < dimensions; ++column) {
      struct Colour colour = rootColours[rootMatrix[row][column]];
      struct Colour greyColour = greyColours[iterMatrix[row][column]-1];
      fwrite(colour.ascii,     1, colour.asciiLen,     rootFile);
      fwrite(greyColour.ascii, 1, greyColour.asciiLen, iterFile);
      //printf("String: \"%s\", length: %d\n", greyColours[iterMatrix[row][column]-1].ascii, greyColours[iterMatrix[row][column]-1].asciiLen);
    }
    fprintf(rootFile, "\n");
    fprintf(iterFile, "\n");
    ++row;
    if(PROGRESS)
      ++nbrRowsWritten;
  }
  
  puts("Closing files...");
  fclose(rootFile);
  fclose(iterFile);
  return NULL;
}

int main(int argc, char *argv[]) {
  size_t i, j;
 
  // Parse command line arguments
  for ( i = 1; i < argc; ++i) {
    if (strncmp(argv[i],"-t",2) == 0) {
      nbrOfThreads = atoi(argv[i]+2);
    }
    else if (strncmp(argv[i],"-l",2) == 0) {
      dimensions = atoi(argv[i]+2);
    }
    else {
      degree = atoi(argv[i]);
    }
  }

  if(DEBUG)
    printf("l: %d\n t: %d\n", dimensions, nbrOfThreads);
  
  // Allocate memory
  puts("Initializing stuffs...");
  rootMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *rootMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  iterMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *iterMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  for (i = 0, j = 0; i < dimensions; ++i, j+=dimensions) {
    rootMatrix[i] = rootMatrixValues + j;
    iterMatrix[i] = iterMatrixValues + j;      
  }
  
  // Calculate the possible roots
  exactRoots = (struct Complex*)malloc(sizeof(struct Complex)*degree);
  for (i = 0; i < degree; ++i) {
    float k = ((float)i)/((float)degree); 
    exactRoots[i].re = cosf(2*M_PI*k);
    exactRoots[i].im = sinf(2*M_PI*k);
    if(DEBUG)
      printf("ROOTS. BLOODY ROOOOOOOTS: (%f,%fi)\n",exactRoots[i].re,exactRoots[i].im);
  }
  
  // Assign colors to each root
  rootColours = (struct Colour*)malloc(sizeof(struct Colour)*degree);
  for (i = 0; i < degree; ++i) {
    int val = 255*i / degree;
    int red = (val)%256;
    int green = (val+85)%256;
    int blue = (val+145)%256;
    asprintf(&rootColours[i].ascii, "%d %d %d ", red, green, blue);
    rootColours[i].asciiLen = strlen(rootColours[i].ascii);
  }
  
  puts("Initializing greyscale..");
  //struct Colour greyColours[WHITE_ITERATION_COUNT]; // 50 shades of grey 
  for (int iteration = 0; iteration < WHITE_ITERATION_COUNT; ++iteration) {
    asprintf(&greyColours[iteration].ascii, "%d %d %d ",iteration, iteration, iteration);// Includes null terminator
    greyColours[iteration].asciiLen = strlen(greyColours[iteration].ascii); // Misses terminating null terminator
  }
 
  // Keeps track finished/unfinshed lines
  rowDone = (int*) malloc(sizeof(int)*dimensions);
  for (i = 0; i < degree; ++i) {
    rowDone[i] = 0;
  }

  printf("Starting...\n");
  
  // create writer thread
  pthread_t writerThread;
  int ret = 0;
  if (ret = pthread_create(&writerThread, NULL, writeRows, NULL)) {
    printf("Error creating thread: %\n", ret);
    exit(1);
  }
  
  runWorkerThreads();

  // Risk for race condition, mitigated with useof atomic variables
  if (PROGRESS) {
    puts("");
    struct timespec mainSleep = {0, 100000};
    while((nbrRowsCompleted < dimensions) || (nbrRowsWritten < dimensions)) {
      printf("Calculation Progress: %d %% ", (nbrRowsCompleted*100)/dimensions);
      printf("Writing Progress: %d %%\r", (nbrRowsWritten*100)/dimensions);
      nanosleep(&mainSleep, NULL);
    }
  }

  if (ret = pthread_join(writerThread, NULL)) {
    printf("Error joining thread: %\n", ret);
    exit(1);
  }

  puts("Freeing memory...");
  free(rootMatrix);
  free(rootMatrixValues);
  free(iterMatrix);
  free(iterMatrixValues);
  free(exactRoots);
  free(rootColours);
  free(rowDone);
  
  printf("Finished!\n");
  return 0;
}
