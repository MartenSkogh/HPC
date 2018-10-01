#include <stdlib.h>
#include <stdio.h>


#define LEN 1000000

int main()
{
    int *y, *x, *p;
    int ix, jx, kx;
    int m, a;

    p = (int*) malloc(sizeof(int)*LEN);
    x = (int*) malloc(sizeof(int)*LEN);
    y = (int*) malloc(sizeof(int)*LEN);

    /*
    m = 1000;
    ix = 0;
    for (jx = 0; jx < m; ++jx) {
        for (kx = 0; kx < m; ++kx) {
            p[jx + m*kx] = ix++;
    }
    */

    for (ix = 0; ix < LEN; ++ix)
        p[ix] = ix;

    a = 2;
    for(kx = 0; kx < LEN; ++kx) {
        jx = p[kx];
        y[jx] = a * x[jx];
    }

    free(p);
    free(x);
    free(y);

    return 0;
}
