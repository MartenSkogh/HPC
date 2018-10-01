#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000

void naive_alloc(int ** data)
{
    *data = (int*) malloc(sizeof(int)*SIZE);
}

int main()
{
    int *data;
    int i, sum;

    naive_alloc(&data);
    
    sum = 0;
    for(i = 0; i < 1e9; ++i)
        sum += i;

    free(data);

    return 0;
}
