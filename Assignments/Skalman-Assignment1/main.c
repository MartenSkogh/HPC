#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ITER 1e9

int main(int argc, char ** argv)
{
    int i;
    unsigned long sum = 0;
    struct timespec start_time, end_time;

    time_t run_time_sec;
    long run_time_nsec;

    timespec_get(&start_time, TIME_UTC);

    for(i = 0; i < MAX_ITER; ++i  ) {
        sum += 1;
    }

    timespec_get(&end_time, TIME_UTC);

    run_time_sec = end_time.tv_sec - start_time.tv_sec;
    run_time_nsec = end_time.tv_nsec - start_time.tv_nsec;

    printf("Result: %lu\n", sum);
    printf("Time taken: %li s, %li ns\n", run_time_sec, run_time_nsec);

    if (run_time_nsec < 0)
        printf("Warning: Nanosecond measurement overflow!\n");

    return 0;
}
