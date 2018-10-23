#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <CL/cl.h>

#define DEBUG 0

int main(int argc, char *argv[]) {
    size_t i, j;     
    cl_int error;
    
    cl_platform_id platform_id;
    cl_uint nmb_platforms;
    if (clGetPlatformIDs(1,&platform_id, &nmb_platforms) != CL_SUCCESS) {
        printf( "cannot get platform\n" );
        return 1;           
    }
    printf("Number of platforms: %d \n", (int)nmb_platforms);	
    int box_height;
    int box_width;
    float central_value;
    float diff_const;
    int nbr_iterations;
    // Parse command line arguments
    box_width = atoi(argv[1]);
    box_height = atoi(argv[2]);
    for ( i = 3; i < argc; ++i) {
        if (strncmp(argv[i],"-i",2) == 0) {
            central_value = atoi(argv[i]+2);
        }
        else if (strncmp(argv[i],"-d",2) == 0) {
            diff_const = atoi(argv[i]+2);
        }
        // Take data file name as input
        else if (strncmp(argv[i],"-n",2) == 0) {
             nbr_iterations = atoi(argv[i]+2);            
        }
    }
    
    cl_device_id device_id;
    cl_uint nmb_devices;
    if (clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
            &device_id, &nmb_devices) != CL_SUCCESS) {
        printf( "cannot get device\n" );
        return 1;
    }

    cl_context context;
    cl_context_properties properties[] =
    {
    CL_CONTEXT_PLATFORM,
    (cl_context_properties) platform_id,
    0
    };
    context = clCreateContext(properties, 1, &device_id, NULL, NULL, &error);    
    
    cl_command_queue command_queue;
    command_queue = clCreateCommandQueue(context, device_id, 0, &error);
    if (error != CL_SUCCESS) {
    printf("cannot create context\n");
    return 1;
  }
    // Allocate memory
    

    if (DEBUG)
    	printf("Starting...\n");


    if (DEBUG)	
    	printf("Starting write to file...\n");

    if (DEBUG)	
    	puts("Freeing memory...");

    if (DEBUG)
    	printf("Finished!\n");
    return 0;
}

