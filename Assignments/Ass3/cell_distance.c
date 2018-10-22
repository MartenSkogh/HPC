#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <stdatomic.h>

#define min(a,b) \
     ({ __typeof__ (a) _a = (a); \
             __typeof__ (b) _b = (b); \
         _a < _b ? _a : _b; })

#define DEBUG 0
#define PROGRESS 1

#define MAX_DISTANCE 3465 // maximum distance given that all points are within [-10, 10]. Calculated by ceil(sqrt(20^2 + 20^2 + 20^2) * 100)
#define CHARACTERS_IN_LINE 24 // example "+01.330 -09.035 +03.489\n"

int *distances;


void parseLine(float* destination, char* line)
{
    for (size_t i = 0, offset = 0; i < 3; ++i, offset += 8)
    {
        destination[i] = (float)(line[offset + 1] - '0') * 10 + (float)(line[offset+2]-'0') + (float)(line[offset+4]-'0')/10 + (float)(line[offset+5]-'0')/100 + (float)(line[offset+6]-'0')/1000;
        if (line[offset] == '-')
            destination[i] *= -1;
    }
}


int readBlock(float** block, int numPoints, FILE *fp, long int startLine)
{
    // Read until the number of lines is equal to blockSize or we reach the end of the file. Return number of lines that were read
    if (DEBUG)
	printf("starting to read block at position %d\n", startLine);
    int fileSeek = fseek(fp,startLine*CHARACTERS_IN_LINE*sizeof(char),SEEK_SET);
    char line[CHARACTERS_IN_LINE];
    int lineNumber;
    for (lineNumber = 0; lineNumber < numPoints; ++lineNumber)
    {
        size_t charRead = fread(line, sizeof(char), CHARACTERS_IN_LINE, fp);
        if (charRead < CHARACTERS_IN_LINE)
            break;
        #pragma omp task
        parseLine(block[lineNumber], line);
    }
    return lineNumber;
}


int dist(float *point1, float *point2)
{
    float x = (point1[0] - point2[0]);
    float y = (point1[1] - point2[1]);
    float z = (point1[2] - point2[2]);
    return (int) (sqrt(x*x + y*y + z*z)*100);
}


void compute_inner_distances(float** block, int numElements)
{
    // Parallelize this shit to hell (maybe use some reduce thingy)
    #pragma omp parallel for reduction(+:distances[0:MAX_DISTANCE])
    for (int i = 0; i < numElements - 1; ++i)
        for(int j = i + 1; j < numElements; ++j)
            ++distances[dist(block[i], block[j])];
}


void compute_cross_distances(float **block1, float **block2, int numElements1, int numElements2)
{
    // Parallelize this shit to hell (maybe use some reduce thingy)
    #pragma omp parallel for reduction(+:distances[0:MAX_DISTANCE])
    for (int i = 0; i < numElements1; ++i)
        for(int j = 0; j < numElements2; ++j)
            ++distances[dist(block1[i], block2[j])];
}


void write_distances()
{  
    if (DEBUG) {	
    FILE *file = fopen("output.txt", "w+");
    for (int i = 0; i < MAX_DISTANCE; ++i)
    {
        if (distances[i] == 0)
            continue;
        fprintf(file, "%.2f %d\n", ((float)(i))/100, distances[i]);
    }
    fclose(file);}

    else {	
    for (int i = 0; i < MAX_DISTANCE; ++i)
    {
        if (distances[i] == 0)
            continue;
        printf("%.2f %d\n", ((float)(i))/100, distances[i]);
    }
   }
}


int main(int argc, char *argv[]) {
    size_t i, j;
    char * fileName = NULL; 
    // Parse command line arguments
    int numThreads = 1;
    int blockSize = 10000;
    for ( i = 1; i < argc; ++i) {
        if (strncmp(argv[i],"-t",2) == 0) {
            numThreads = atoi(argv[i]+2);
        }
	      else if (strncmp(argv[i],"-b",2) == 0) {
            blockSize = atoi(argv[i]+2);
        }
        // Take data file name as input
        else if (strncmp(argv[i],"-f",2) == 0) {
             fileName = argv[i]+2;
             printf("Input file: '%s'\n", fileName);
        }
    }

    if(DEBUG)
        printf("t: %d\n", numThreads);
    
    omp_set_num_threads(numThreads);

    // Allocate memory
    if (DEBUG)
    	puts("Initializing stuffs...");
    distances = (int*)malloc(sizeof(int)*MAX_DISTANCE);
    for (i = 0; i < MAX_DISTANCE; ++i)
        distances[i] = 0;

    float **block1 = (float**) malloc(sizeof(float*) * blockSize);
    float *block1Values = (float*) malloc(sizeof(float) * blockSize * 3); 
    float **block2 = (float**) malloc(sizeof(float*) * blockSize);
    float *block2Values = (float*) malloc(sizeof(float) * blockSize * 3); 
    for (i = 0, j = 0; i < blockSize; ++i, j+=3) {
        block1[i] = block1Values + j;
        block2[i] = block2Values + j;            
    }

    if (DEBUG) { puts("opening file"); }
    if (fileName == NULL)
      fileName = "cells"; 
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("ERROR: Cannot open file \"%s\"\n", fileName);
        return -1;
    }

    if (DEBUG)
    	printf("Starting...\n");
    // figure out numBlocks somehow 
    long int block1Position = 0;
    long int block2Position = 0;
    while (block1Position % blockSize == 0)
    {
        // TODO: need some way to tell readBlock where to start reading
        int numLines1 = readBlock(block1, blockSize, fp, block1Position);
        block1Position += numLines1;

        // compute distances within one block
        compute_inner_distances(block1, numLines1);

        // End the loop if we read the last block
        if (numLines1 < blockSize)
            break;

        // compute distances between blocks
        block2Position = block1Position;
        while (block2Position % blockSize == 0)
        {
            // TODO: need some way to tell readBlock where to start reading
            int  numLines2 = readBlock(block2, blockSize, fp, block2Position);
            if (numLines2 == 0)
                break;
            compute_cross_distances(block1, block2, numLines1, numLines2);
            block2Position += numLines2;
        }
    }

    // Risk for race condition, mitigated with useof atomic variables
    // if (PROGRESS) {
    //     puts("");
    //     struct timespec mainSleep = {0, 100000};
    //     while((nbrRowsCompleted < dimensions) || (nbrRowsWritten < dimensions)) {
    //         printf("Calculation Progress: %d %% ", (nbrRowsCompleted*100)/dimensions);
    //         printf("Writing Progress: %d %%\r", (nbrRowsWritten*100)/dimensions);
    //         nanosleep(&mainSleep, NULL);
    //     }
    // }
    if (DEBUG)	
    	printf("Starting write to file...\n");
    write_distances();
    if (DEBUG)	
    	puts("Freeing memory...");
    free(block1);
    free(block1Values);
    free(block2);
    free(block2Values);
    if (DEBUG)
    	printf("Finished!\n");
    return 0;
}
