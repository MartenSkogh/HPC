#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// 2^20 = 0xFFFFF
#define NBR_VALUES 0xFFFFF 

int main() 
{
    FILE *hdd_file, *sdd_file;
    int *data;
    struct timespec t_start, t_end;
    double hdd_read_time, hdd_write_time, sdd_read_time, sdd_write_time;

    const char *hdd_file_name = "HDD_data.dat";
    const char *sdd_file_name = "/run/mount/scratch/hpcuser095/SDD_data.dat";

    data = (int*) malloc(sizeof(int)*NBR_VALUES);

    // Test write times
    hdd_file = fopen(hdd_file_name, "w");
    sdd_file = fopen(sdd_file_name, "w");
    
    // HDD
    timespec_get(&t_start, TIME_UTC);
    fwrite(data, sizeof(int), NBR_VALUES, hdd_file);
    timespec_get(&t_end, TIME_UTC);
    hdd_write_time = (t_end.tv_sec - t_start.tv_sec); 
    hdd_write_time += 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    // SDD
    timespec_get(&t_start, TIME_UTC);
    fwrite(data, sizeof(int), NBR_VALUES, sdd_file);
    timespec_get(&t_end, TIME_UTC);
    sdd_write_time = (t_end.tv_sec - t_start.tv_sec); 
    sdd_write_time += 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    fclose(hdd_file);
    fclose(sdd_file);
    
    // Test read times
    hdd_file = fopen(hdd_file_name, "r");
    sdd_file = fopen(sdd_file_name, "r");

    // HDD
    timespec_get(&t_start, TIME_UTC);
    fread(data, sizeof(int), NBR_VALUES, sdd_file);
    timespec_get(&t_end, TIME_UTC);
    hdd_read_time = (t_end.tv_sec - t_start.tv_sec); 
    hdd_read_time += 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    // SDD
    timespec_get(&t_start, TIME_UTC);
    fread(data, sizeof(int), NBR_VALUES, sdd_file);
    timespec_get(&t_end, TIME_UTC);
    sdd_read_time = (t_end.tv_sec - t_start.tv_sec); 
    sdd_read_time += 1e-9*(t_end.tv_nsec - t_start.tv_nsec);

    fclose(hdd_file);
    fclose(sdd_file);

    // Print result
    printf("HDD write time: %f s\n", hdd_write_time);
    printf("HDD read time: %f s\n",  hdd_read_time);
    printf("SDD write time: %f s\n", sdd_write_time);
    printf("SDD read time: %f s\n",  sdd_read_time);

    free(data);
    return 0;
}
