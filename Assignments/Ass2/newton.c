#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define WHITE_ITERATION_COUNT 20
#define RANGE 2.f
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

// Complex number
struct Complex {
  double re, im;
};

// RGB color
struct Colour {
  int r, g, b;
};
  
int nbrOfThreads, dimensions, degree, blockSize;
int **rootMatrix, **iterMatrix;
struct Complex *exactRoots;
struct Colour *rootColours;


// OPTIMIZE IN CASE OF GERMAN
// Compelx multiplicaion of two complex numbers
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
          printf("new dist! %f", newDist);
    if (newDist < bestDist) {
      printf("best dist! %f", newDist);
      bestInd = i;
      bestDist = newDist;
    }
  }
  printf("%d", bestInd);
  rootMatrix[rowIndex][colIndex] = bestInd;
}


void newton(void * restrict arg) {
  int startPoint = ((int*)arg)[0];
  int nbrOfElements = ((int*)arg)[1];
  int rowIndex = ((int*)arg)[2];
  for(int i = 0; i < nbrOfElements; ++i) {
    findRoot(rowIndex, startPoint+i);
  }
} 

void computeRow(int **rootMatrix, int **iterMatrix, 
                int rowIndex, struct Complex *roots) {
  int ret = 0;
  pthread_t threads[nbrOfThreads];
  for (int tx=0, ix=0; tx < nbrOfThreads; ++tx, ix+=blockSize) {
    int *arg = malloc(3*sizeof(int));
    arg[0] = ix; 
    arg[1] = min(blockSize,dimensions-ix); 
    arg[2] = rowIndex;
    if (ret = pthread_create(threads+tx, NULL, newton, (void*)arg)) {
      printf("Error creating thread: %\n", ret);
      exit(1);
    }
  }
  for (int tx=0; tx < nbrOfThreads; ++tx) {
    if (ret = pthread_join(threads[tx], NULL)) {
      printf("Error joining thread: %\n", ret);
      exit(1);
    }
  }
}
  
void writeRow(FILE *rootFile, FILE *iterFile, int *rootRow, int *iterRow, struct Colour *rootColours) {
  for (int i = 0; i < dimensions; ++i){
    struct Colour colour = rootColours[rootRow[i]];
    //printf("%d %d %d ", colour.r, colour.g, colour.b);
    //printf("%d ", iterRow[i]);
    fprintf(rootFile, "%d %d %d ", colour.r, colour.g, colour.b);
    fprintf(iterFile, "%d ", iterRow[i]);
  }
  fprintf(rootFile, "\n");
  fprintf(iterFile, "\n");
}

 

int main(int argc, char *argv[]) {
  size_t i, j;
 
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
  printf("l: %d\n",dimensions);
  printf("t: %d\n",nbrOfThreads);
  

  rootMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *rootMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  iterMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *iterMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  for (i = 0, j = 0; i < dimensions; ++i, j+=dimensions) {
    rootMatrix[i] = rootMatrixValues + j;
    iterMatrix[i] = iterMatrixValues + j;      
  }
  
  exactRoots = (struct Complex*)malloc(sizeof(struct Complex)*degree);
  rootColours = (struct Colour*)malloc(sizeof(struct Colour)*degree);
  for (i = 0; i < degree; ++i) {
    float k = ((float)i)/((float)degree); 
    exactRoots[i].re = cosf(2*M_PI*k);
    exactRoots[i].im = sinf(2*M_PI*k);
    printf("ROOTS. BLOODY ROOOOOOOTS: (%f,%fi)\n",exactRoots[i].re,exactRoots[i].im);
  }
  for (i = 0; i < degree; ++i) {
    int val = 255*i / degree;
    rootColours[i].r = val;
    rootColours[i].g = val;
    rootColours[i].b = val;
  }
     
  printf("Starting...\n");
  FILE *rootFile;
  FILE *iterFile;
  
  rootFile = fopen("rootImage.ppm","w");
  iterFile = fopen("iterImage.ppm","w");  
  fprintf(rootFile,"P3\n%d %d\n255\n", dimensions, dimensions);
  fprintf(iterFile,"P2\n%d %d\n%d\n", dimensions, dimensions, WHITE_ITERATION_COUNT);  

  for (i = 0; i < dimensions; i++) {
    computeRow(rootMatrix, iterMatrix, i, exactRoots);
    writeRow(rootFile, iterFile, rootMatrix[i], iterMatrix[i], rootColours);    
  }
   

  free(rootMatrix);
  free(rootMatrixValues);
  free(iterMatrix);
  free(iterMatrixValues);
  
  fclose(rootFile);
  fclose(iterFile);
  
  printf("Finished!\n");
  return 0;
}
