#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 10

// Some kind of atempt at a hashing function...
int elementValue(int x, int y)
{
  return (x + y*SIZE);
}

int main()
{
  // Instantiate stuff
  int x, y; 
  size_t im, ie;
  bool allEqual;

  int * entries = (int*) malloc(sizeof(int) * SIZE*SIZE);
  int ** matrix = (int**) malloc(sizeof(int*) * SIZE);

  int * fileEntries = (int*) malloc(sizeof(int) * SIZE*SIZE);
  int ** fileMatrix = (int**) malloc(sizeof(int*) * SIZE);

  for(im = 0, ie = 0; im < SIZE; ++im, ie+=SIZE)
  {
    matrix[im] = entries + ie;
    fileMatrix[im] = fileEntries + ie;
  }

  // File name
  FILE *mFile;
  const char * matrixFilename = "matrix.dat";

  puts("Creating matrix values");
  for(y = 0; y < SIZE; ++y)
  {
    for(x = 0; x < SIZE; ++x)
    {
      matrix[y][x] = y;
    }
  }

  puts("Writing matrix to file");
  mFile = fopen(matrixFilename, "wb+");
  fwrite((void*)entries, sizeof(entries[0]), (size_t)SIZE*SIZE, mFile);

  puts("Reading matrix from file");
  rewind(mFile);
  fread((void*)fileEntries, sizeof(fileEntries[0]), (size_t)SIZE*SIZE, mFile);

  puts("Compairing results");
  allEqual = true;
  fileMatrix[2][4] = 0;
  for(y = 0; y < SIZE; ++y)
  {
    for(x = 0; x < SIZE; ++x)
    {
      if(matrix[y][x] != fileMatrix[y][x])
        {allEqual = false; break;}
    }
  }

  if(allEqual)
    puts("MATRICES MATCH!");
  else
    puts("MATRICES DO NOT MATCH!");

  fclose(mFile);
  
  puts("All done!");

  return 0;
}
