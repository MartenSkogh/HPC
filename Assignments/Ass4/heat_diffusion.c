#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <CL/cl.h>

#define DEBUG 0

/*
char *kernel_program = "__kernel void heat_step(__global const float * read, __global const float * write, int width, int height)"
    "{"
    "int ix = get_global_id(0);"
    "int jx = get_global_id(1);"
    "if (ix == 0 || ix == width || jx == 0 || jx == height)"
    "return;"
    "write[ix * width + jx] = read[(ix+1) * width + jx] + read[(ix-1) * width + jx] + read[ix * width + jx + 1] + read[ix * width + jx - 1] - read[ix * width + jx];"
    "}";
*/

char *kernel_program = "__kernel void heat_step(__global const float * read, __global const float * write, int width, int height)"
    "{"
    "int ix = get_global_id(0);"
    "int jx = get_global_id(1);"
    "write[ix * width + jx] = 1;"
    "}";


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
    printf("1\n");
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
    if (error != CL_SUCCESS) {
        printf("cannot create context\n");
        return 1;
    }
    printf("2\n");
    cl_command_queue command_queue;
    command_queue = clCreateCommandQueue(context, device_id, 0, &error);
    if (error != CL_SUCCESS) {
        printf("cannot create queue\n");
        return 1;
    }
    cl_mem box_matrix_1, box_matrix_2;
    box_matrix_1  = clCreateBuffer(context, CL_MEM_READ_WRITE,
        box_height * box_width * sizeof(float), NULL, NULL);
    box_matrix_2  = clCreateBuffer(context, CL_MEM_READ_WRITE,
        box_height * box_width * sizeof(float), NULL, NULL);
    
    // Allocate memory
    float * a = malloc(box_height*box_width*sizeof(float));
    float * b = malloc(box_height*box_width*sizeof(float));
    for (size_t ix=0; ix < box_height*box_width; ++ix)
    {
        a[ix] = 0;
    }
    for (int ix = box_height / 2; ix <= box_height / 2 + 1 - (box_height % 2); ++ix)
        for (int jx = box_width / 2; jx <= box_width / 2 + 1 - (box_width % 2); ++jx)
        {
            a[ix * box_width + jx] = central_value;
        }
    
    printf("3\n");
    		
    for (i = 0; i < box_height*box_width; i++)
    	printf("%f ",a[i]);
    clEnqueueWriteBuffer(command_queue, box_matrix_1, CL_TRUE,
        0, box_height*box_width*sizeof(float), a, 0, NULL, NULL);
    // clEnqueueWriteBuffer(command_queue, input_buffer_b, CL_TRUE,
    //     0, width_b*height_b*sizeof(float), b, 0, NULL, NULL);
    
    printf("4\n");
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &kernel_program, NULL, &error);
    if (error != CL_SUCCESS) {
        printf("cannot create program\n");
        return 1;
    }

    printf("4.2\n");
    error = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (error != CL_SUCCESS) {
        printf("cannot build program. log:\n");

        size_t log_size = 0;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        char * log = calloc(log_size, sizeof(char));
        if (log == NULL) {
            printf("could not allocate memory\n");
            return 1;
        }
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
            log_size, log, NULL);
        printf( "%s\n", log );
        free(log);
        return 1;
    }

    printf("5\n");
    cl_kernel kernel = clCreateKernel(program, "heat_step", &error);
    if (error != CL_SUCCESS) {
        printf("cannot create kernel 0\n");
        return 1;
    }
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &box_matrix_1);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &box_matrix_2);
    clSetKernelArg(kernel, 2, sizeof(int), &box_width);
    clSetKernelArg(kernel, 3, sizeof(int), &box_height);

    printf("6\n");
    const size_t global[] = {box_height, box_width};
    clEnqueueNDRangeKernel(command_queue, kernel, 2,
        NULL, (const size_t *)&global, NULL, 0, NULL, NULL);
    if (error != CL_SUCCESS) {
        printf("cannot enque kernel 0\n");
        return 1;
    }

    clEnqueueReadBuffer(command_queue, box_matrix_2, CL_TRUE,
        0, box_height*box_width*sizeof(float), b, 0, NULL, NULL);
    if (error != CL_SUCCESS) {
        printf("cannot read buffer 0\n");
        return 1;
    }
    for (i = 0; i < box_height*box_width; i++)
    	printf("%f ",b[i]);
    return 0;
}
