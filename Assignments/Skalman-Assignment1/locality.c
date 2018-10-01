#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NBR_ROWS 1000
#define NBR_COLS 1000

void row_sums(double * sums, double ** matrix, size_t nrs, size_t ncs)
{
    size_t ix, jx;
    double sum;

    for (ix = 0; ix < nrs; ++ix) {
        sum = 0;
        for (jx = 0; jx < ncs; ++jx) {
            sum += matrix[ix][jx];
        }
        sums[ix] = sum;
    }
}

void col_sums(double * sums, double ** matrix, size_t nrs, size_t ncs)
{
    size_t ix, jx;
    double sum;

    for (jx = 0; jx < ncs; ++jx) {
        sum = 0;
        for (ix = 0; ix < nrs; ++ix) {
            sum += matrix[ix][jx];
        }
        sums[jx] = sum;
    }
}


int main()
{
    double **matrix;
    double *m_entries;
    struct timespec t_start, t_end;
    double time_rows, time_cols;
    size_t ix, jx, offset;
    double *sums;

    matrix = (double**) malloc(sizeof(double*)*NBR_COLS);
    m_entries = (double*) malloc(sizeof(double)*NBR_ROWS*NBR_COLS);

    for (ix = 0, offset = 0; ix < NBR_ROWS; ++ix, offset+=NBR_COLS ) {
        matrix[ix] = m_entries + offset;
        for (jx = 0;  jx < NBR_COLS; ++jx)
            matrix[ix][jx] = 1.0;
    }

    sums = (double*) malloc(sizeof(double)*NBR_ROWS);
    timespec_get(&t_start, TIME_UTC);
    row_sums(sums, matrix, NBR_ROWS, NBR_COLS);
    timespec_get(&t_end, TIME_UTC);
    time_rows = (t_end.tv_sec - t_start.tv_sec) + 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    sums = (double*) malloc(sizeof(double)*NBR_COLS);
    timespec_get(&t_start, TIME_UTC);
    col_sums(sums, matrix, NBR_ROWS, NBR_COLS);
    timespec_get(&t_end, TIME_UTC);
    time_cols = (t_end.tv_sec - t_start.tv_sec) + 1e-9*(t_end.tv_nsec - t_start.tv_nsec);
    

    printf("Time to sum rows: %g s\n", time_rows);
    printf("Time to sum cols: %g s\n", time_cols);

    free(m_entries);
    free(matrix);

    return 0;
}
