#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main()
{
  // Allocate memory
  int * asEntries = (int*) malloc(sizeof(int*) * SIZE*SIZE);
  int ** as = (int**) malloc(sizeof(int**) * SIZE);

  // Write matrix elements
  for(size_t ix = 0, iy = 0; iy < SIZE; ++ix, iy)
  
}
