#include <stddef.h>
#include <stdio.h>

#define SIZE 10000000


int main() {
  int as[SIZE];

  for ( size_t ix = 0; ix < SIZE; ++ix )
    as[ix] = 0;

  printf("%d\n", as[0]);

}
