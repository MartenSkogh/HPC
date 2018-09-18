#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

int main()
{
  // Allocate memory for pointers to pointers
  int ** as = (int**) malloc(sizeof(int*) * SIZE);
  // The inner allocation is done per "column". Because of this, each column is
  // allocated onto seperate memory blocks (small chance that they can be
  // allocated in connected memory).
  for ( size_t ix = 0; ix < SIZE; ++ix ) 
    as[ix] = (int*) malloc(sizeof(int) * SIZE);

  for ( size_t ix = 0; ix < SIZE; ++ix )
    for ( size_t jx = 0; jx < SIZE; ++jx )
      as[ix][jx] = 0;

  printf("%d\n", as[0][0]);

  for ( size_t ix = 0; ix < SIZE; ++ix )
    free(as[ix]);
  free(as);
}
