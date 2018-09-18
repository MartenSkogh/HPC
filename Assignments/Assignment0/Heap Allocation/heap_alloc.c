#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 10000000

int main()
{
    int * as = (int*) malloc(sizeof(int) * SIZE);
    for ( size_t ix = 0; ix < SIZE; ++ix )
      as[ix] = 0;

    printf("%d\n", as[0]);

    free(as);
}
