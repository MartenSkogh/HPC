#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WHITE_ITERATION_COUNT 20

struct CpxNbr{
  double re, im;
};

struct Colour{
  int r, g, b;
};
  
int nbrOfThreads, dimensions, degree, blockSize;  

void computeRow(int **rootMatrix, int **iterMatrix, int rowIndex, struct CpxNbr *roots) {
,
}
  
void writeRow(FILE *rootFile, FILE *iterFile, int *rootRow, int *iterRow, struct Colour *rootColours) {
  for (int i = 0; i < dimensions; i++){
    struct Colour colour = rootColours[rootRow[i]]
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
  
  struct CpxNbr exactRoots[degree];
  struct Colour rootColours[degree];
  int **rootMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *rootMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  int **iterMatrix = (int**)malloc(sizeof(int*)*dimensions);
  int *iterMatrixValues = (int*)malloc(sizeof(int)*dimensions*dimensions); 
  for (i = 0, j = 0; i < dimensions; ++i, j+=dimensions) {
    rootMatrix[i] = rootMatrixValues + j;
    iterMatrix[i] = iterMatrixValues + j;      
  }
  
  for (i = 0; i < degree; ++i) {
    float k = ((float)i)/((float)degree); 
    exactRoots[i].re = cosf(2*M_PI*k);
    exactRoots[i].im = sinf(2*M_PI*k);
    printf("ROOTS. BLOODY ROOOOOOOTS: (%f,%fi)\n",exactRoots[i].re,exactRoots[i].im);
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
