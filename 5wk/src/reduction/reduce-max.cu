#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <cstdlib>
#include <iostream>

#include "timer.h"

using namespace std;

/* Utility function, use to do error checking.

   Use this function like this:
   checkCudaCall(cudaMalloc((void **) &deviceRGB, imgS * sizeof(color_t)));

   And to check the result of a kernel invocation:
   checkCudaCall(cudaGetLastError());
 */
static void checkCudaCall(cudaError_t result) {
    if (result != cudaSuccess) {
        cerr << "cuda error: " << cudaGetErrorString(result) << endl;
        exit(1);
    }
}

float max_array(float a[], int num_elements)
{
    float max=FLT_MIN;
    for (int i = 0; i < num_elements; i++) {
        if (a[i] > max) {
            max = a[i];
        }
    }
    return max;
}

__global__ void reduce_max_kernel(float* input_d, float* partial_result_d, int i_max)
{
    extern __shared__ float shared_input_d[];

    unsigned global_tid = blockIdx.x * blockDim.x + threadIdx.x;

    /* load input into __shared__ memory */
    if (global_tid < i_max) {
        shared_input_d[threadIdx.x] = input_d[global_tid];
    } else {
        shared_input_d[threadIdx.x] = 0;
    }
    __syncthreads();

    /* Let every thread compare a number of the first half of the block to
       a number in the second half of the block. */
    for (int offset = blockDim.x / 2; offset > 0; offset >>= 1) {

        /* If the offset is smaller as the threadId, then the thread is not
           needed during computation */
        if (threadIdx.x < offset) {

            /* Get the maximum value of both cells */
            shared_input_d[global_tid] =
                ((shared_input_d[global_tid] < shared_input_d[global_tid + offset]) ?
                 shared_input_d[global_tid + offset] : shared_input_d[global_tid]);
        }

        /* Wait for all threads to update their data, so the next iteration
           can be started */
        __syncthreads();
    }

    if (threadIdx.x == 0) {
        partial_result_d[blockIdx.x] = shared_input_d[global_tid];
    }
}


void reduce_max_cuda(int i_max, float *list_h, const int block_size, float *result_h)
{
    float *list_d = NULL;
    float *partial_result_d = NULL;

    /* number of blocks is equal to:
     * Integer division + extra block for remainder */
    const int max_blocks = (i_max/block_size) + ((i_max % block_size) ? 1 : 0);

    /* Copy the original vectors to the GPU */
    checkCudaCall(cudaMalloc((void **) &list_d, i_max * sizeof(float)));
    checkCudaCall(cudaMemcpy(list_d, list_h, i_max * sizeof(float), cudaMemcpyHostToDevice));
    checkCudaCall(cudaMalloc((void**)&partial_result_d, (max_blocks + 1) * sizeof(float)));

    /* Setup timing  */
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    /* Measure the start time  */
    cudaEventRecord(start, 0);

    /* launch one kernel to compute, per-block, a partial maximum */
    reduce_max_kernel<<<max_blocks, block_size, sizeof(float) * block_size>>>
        (list_d, partial_result_d, i_max);

    // check whether the kernel invocation was successful
    checkCudaCall(cudaGetLastError());

    /* launch a single block to compute the maximum of the partial maximums */
    reduce_max_kernel<<<1, max_blocks, max_blocks * sizeof(float)>>>
        (partial_result_d, partial_result_d, i_max);

    /* check whether the kernel invocation was successful */
    checkCudaCall(cudaGetLastError());

    /* Stop the timing */
    cudaEventRecord(stop, 0);

    // copy result back
    checkCudaCall(cudaMemcpy(result_h, partial_result_d, sizeof(float), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(list_d));
    checkCudaCall(cudaFree(partial_result_d));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    cout << elapsedTime << endl;
}


int main(int argc, char* argv[])
{
    int i_max = 0;
    int block_size;
    float *result = new float[1]();
    timer maxTimer("Max Timer:");

    srand(time(NULL));

    if (argc < 3) {
        printf("Usage: %s i_max b_s\n", argv[0]);
        printf(" - i_max: number of discrete points in the list, should be >2\n");
        printf(" - b_s: block size, power of 2\n");
        return EXIT_FAILURE;
    }

    /* Only accept powers of two */
    i_max = atoi(argv[1]);
    if (!(!(i_max == 0) && !(i_max & (i_max - 1)))) {
        cout << "Not a power of two" << endl;
        exit(0);
    }
    if (i_max < 2) {
        printf("argument error: i_max should be >2.\n");
        return EXIT_FAILURE;
    }

    block_size = atoi(argv[2]);


    // make a list of floats
    float list[i_max];
    for (int i = 0; i < i_max; i++){
        list[i] = (float)rand()/((float)RAND_MAX/FLT_MAX);
    }

    maxTimer.start();
    reduce_max_cuda(i_max, list, block_size, result);
    *result = max_array(list, i_max);
    maxTimer.stop();
    /* cout << "max seq:" << max_array(list, i_max) << endl; */
    /* cout << "max cuda:" << *result << endl; */
    cout << maxTimer << endl;

    return EXIT_SUCCESS;
}
