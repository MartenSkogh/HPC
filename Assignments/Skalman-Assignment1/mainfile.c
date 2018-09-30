#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 30000

// Compelx multiplicaion of two complex numbers
void mul_cpx_mainfile(double * a_re, double * a_im, 
             double * b_re, double * b_im,
             double * c_re, double * c_im)
{
    // b = b_re + i*b_im
    // c = c_re + i*c_im
    // a = b*c = (b_re + i*b_im)*(c_re + i*c_im)
    *a_re = (*b_re)*(*c_re) - (*b_im)*(*c_im);
    *a_im = (*b_im)*(*c_re) + (*b_re)*(*c_im);
}

int main() 
{
    size_t i, offset;
    double **as, **bs, **cs;
    double *a, *b, *c;
    struct timespec t_start, t_end;
    double time_taken;

    // Allocate memory
    as = (double**) malloc(sizeof(double*)*SIZE);
    bs = (double**) malloc(sizeof(double*)*SIZE);
    cs = (double**) malloc(sizeof(double*)*SIZE);

    a = (double*) malloc(sizeof(double)*2*SIZE);
    b = (double*) malloc(sizeof(double)*2*SIZE);
    c = (double*) malloc(sizeof(double)*2*SIZE);

    for (i = 0, offset = 0; i < SIZE; ++i, offset += 2) {
        as[i] = a + offset;
        bs[i] = b + offset;
        cs[i] = c + offset;
    }

    // Write values to memory
    for(i = 0; i < SIZE; ++i ) {
        bs[i][0] = (rand() % 20) - 10; bs[i][1] = (rand() % 20) - 10;
        cs[i][0] = (rand() % 20) - 10; cs[i][1] = (rand() % 20) - 10;
    }

    // Makemultiplication
    timespec_get(&t_start, TIME_UTC);
    for(i = 0; i < SIZE; ++i ) {
        mul_cpx_mainfile(&as[i][0], &as[i][1], &bs[i][0], &bs[i][1], &cs[i][0], &cs[i][1]);
    }
    timespec_get(&t_end, TIME_UTC);
    
    time_taken = (t_end.tv_sec - t_start.tv_sec) + 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    //mul_cpx_mainfile(&as[0][0], &as[0][1], &bs[0][0], &bs[0][1], &cs[0][0], &cs[0][1]);

    //printf("b = %g + %gi, c = %g + %gi\n", bs[0][0], bs[0][1], cs[0][0], cs[0][1]);
    //printf("a = b*c = %g + %gi\n", as[0][0], as[0][1]);

    printf("Loop finished in %g seconds, (%g nanoseconds)\n", time_taken, time_taken*1e9);

    return 0;
}
