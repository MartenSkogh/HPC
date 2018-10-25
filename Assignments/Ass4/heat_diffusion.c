#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <CL/cl.h>

#define DEBUG 0


char *program_source = "__kernel void heat_step(__global restrict double * read, __global restrict double * write, double c)"
    "{"
        "int ix = get_global_id(0); "
        "int jx = get_global_id(1); "
        "int height = get_global_size(0); "
        "int width = get_global_size(1); "
        "double top = (ix == 0 ? 0.0 : read[(ix-1) * width + jx]; "
        "double bot = (ix == height - 1) ? 0.0 : read[(ix+1) * width + jx]; "
        "double left = (jx == 0) ? 0.0 : read[ix * width + jx - 1]; "
        "double right = (jx == width - 1) ? 0.0 : read[ix * width + jx + 1]; "
        "write[ix * width + jx] = (1.0 - c) * read[ix * width + jx] + c * (top + left + bot + right) / 4.0; "
    "} \n"
    "__kernel void abs_diff(__global restrict double * matrix, double subtractor)"
    "{"
        "int ix = get_global_id(0); "
        "int jx = get_global_id(1); "
        "int pos = ix * width + jx; "
        "matrix[pos] = matrix[pos] - subtractor;"
        "if (matrix[pos] < 0)"
            "matrix[pos] *= -1; "
    "} \n"
    "__kernel void reduce(  __global restrict float* matrix, __local restrict float* scratch, __global int len, __global restrict float* result)"
    "{"
        "int gsz = get_global_size(0); "
        "int gix = get_global_id(0); "
        "int lsz = get_local_size(0); "
        "int lix = get_local_id(0); "
        "float acc = 0; "
        "for (int cix = gix; cix < len; cix+=gsz) "
            "acc += c[cix]; "

        "scratch[lix] = acc;"
        "barrier(CLK_LOCAL_MEM_FENCE);"

        "for(int offset = lsz/2; offset>0; offset/=2) { "
            "if (lix < offset) "
                "scratch[lix] += scratch[lix+offset]; "
            "barrier(CLK_LOCAL_MEM_FENCE); "
        "}"

        "if (lix == 0) "
            "result[get_group_id(0)] = scratch[0]; "
    "}";


void print_matrix(double *vector, int height, int width){
    for (int i = 1; i < height-1; ++i) { 
        for (int j = 1; j < width-1; ++j)
	    printf("%.2e ",vector[i*width+j]);
    	printf("\n");
    }
}


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
    double central_value;
    double diff_const;
    int nbr_iterations;
    // Parse command line arguments
    box_width = atoi(argv[1]) + 2; // Add boundaries
    box_height = atoi(argv[2]) + 2;
    for ( i = 3; i < argc; ++i) {
        if (strncmp(argv[i],"-i",2) == 0) {
            central_value = atof(argv[i]+2);
        }
        else if (strncmp(argv[i],"-d",2) == 0) {
            diff_const = atof(argv[i]+2);
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
    cl_mem matrix_buffer_read, matrix_buffer_write, tmp1;
    matrix_buffer_read  = clCreateBuffer(context, CL_MEM_READ_WRITE, box_height * box_width * sizeof(double), NULL, NULL);
    matrix_buffer_write = clCreateBuffer(context, CL_MEM_READ_WRITE, box_height * box_width * sizeof(double), NULL, NULL);
    
    // Allocate memory
    double * box_matrix = malloc(box_height*box_width*sizeof(double));
    for (size_t ix=0; ix < box_height*box_width; ++ix)
    {
        box_matrix[ix] = 0;
    }
    for (int ix = box_height / 2; ix >= box_height / 2 - 1 + (box_height % 2); --ix)
        for (int jx = box_width / 2; jx >= box_width / 2 - 1 + (box_width % 2); --jx)
            box_matrix[ix * box_width + jx] = central_value;
    
    printf("3\n");

    print_matrix(box_matrix,box_height,box_width);

    clEnqueueWriteBuffer(command_queue, matrix_buffer_read, CL_TRUE, 0, box_height*box_width*sizeof(double), box_matrix, 0, NULL, NULL);

    printf("4\n");
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) &program_source, NULL, &error);
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
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        printf( "%s\n", log );
        free(log);
        return 1;
    }

    printf("5\n");
    cl_kernel heat_step_kernel = clCreateKernel(program, "heat_step", &error);
    if (error != CL_SUCCESS) {
        printf("cannot create kernel 0\n");
        return 1;
    }

    const size_t global[] = {box_height, box_width};
    const size_t local[] = {20, 20};

    clSetKernelArg(heat_step_kernel, 2, sizeof(double), &diff_const);

    for (i = 0; i < nbr_iterations; ++i) {    
        clSetKernelArg(heat_step_kernel, 0, sizeof(cl_mem), &matrix_buffer_read);
        clSetKernelArg(heat_step_kernel, 1, sizeof(cl_mem), &matrix_buffer_write);

        clEnqueueNDRangeKernel(command_queue, heat_step_kernel, 2, NULL, (const size_t *)&global, (const size_t *)&local, 0, NULL, NULL);
        if (error != CL_SUCCESS) {
            printf("cannot enque kernel 0\n");
            return 1;
        }
        if (i != nbr_iterations - 1)
        {
            tmp1 = matrix_buffer_write;
            matrix_buffer_write = matrix_buffer_read;
            matrix_buffer_read = tmp1;
        }
    }

    cl_kernel average_kernel = clCreateKernel(program, "reduce", &error);
    if (error != CL_SUCCESS) {
        printf("cannot create kernel reduce\n");
        return 1;
    }

    cl_mem output_buffer_c, output_buffer_c_sum;
    output_buffer_c = clCreateBuffer(context, CL_MEM_READ_WRITE, box_height * box_width * sizeof(double), NULL, NULL);
    output_buffer_c_sum  = clCreateBuffer(context, CL_MEM_READ_WRITE, box_height * box_width * sizeof(double), NULL, NULL);
    const cl_int ix_m_int;// = (cl_int)ix_m;
    clSetKernelArg(average_kernel, 0, sizeof(cl_mem), &output_buffer_c);
    clSetKernelArg(average_kernel, 1, local_size*sizeof(cl_float), NULL);
    clSetKernelArg(average_kernel, 2, sizeof(cl_int), &ix_m_int);
    clSetKernelArg(average_kernel, 3, sizeof(cl_mem), &output_buffer_c_sum);

    clEnqueueNDRangeKernel(command_queue, average_kernel, 1,
        NULL, (const size_t *)&global_size, (const size_t *)&local, 0, NULL, NULL);

    double * sum = malloc(nmb_groups*sizeof(double));
    clEnqueueReadBuffer(command_queue, output_buffer_c_sum, CL_TRUE,
        0, nmb_groups*sizeof(double), c_sum, 0, NULL, NULL);

    clFinish(command_queue);

    double sum_total = 0;
    for (size_t ix=0; ix < nmb_groups; ++ix)
        c_sum_total += c_sum[ix];
    
    cl_kernel absdiff_kernel = clCreateKernel(program, "abs_diff", &error);
    if (error != CL_SUCCESS) {
        printf("cannot create kernel absdiff \n");
        return 1;
    }

    clSetKernelArg(absdiff_kernel, 0, sizeof(cl_mem), &matrix_buffer_write);
    clSetKernelArg(absdiff_kernel, 0, sizeof(double), &sum_total);

    clEnqueueReadBuffer(command_queue, matrix_buffer_write, CL_TRUE, 0, box_height*box_width*sizeof(double), box_matrix, 0, NULL, NULL);
    if (error != CL_SUCCESS) {
        printf("cannot read buffer 0\n");
        return 1;
    }
    print_matrix(box_matrix, box_height, box_width);

    return 0;
}
