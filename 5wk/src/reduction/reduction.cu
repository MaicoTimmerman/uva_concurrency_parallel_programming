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

__global__ void vectorAddKernel(float* deviceA, float* deviceB, float* deviceResult) {
    unsigned index = blockIdx.x * blockDim.x + threadIdx.x;
    deviceResult[index] = deviceA[index] + deviceB[index];
}


float vectorAddCuda(int i_max, float* list_h) {
    /* int threadBlockSize = 512; */
    float *list_d;

    checkCudaCall(cudaMalloc((void **) &list_d, i_max * sizeof(float)));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // copy the original vectors to the GPU
    checkCudaCall(cudaMemcpy(list_d, list_h, i_max * sizeof(float), cudaMemcpyHostToDevice));

    // execute kernel
    cudaEventRecord(start, 0);
    /* vectorAddKernel<<<n/threadBlockSize, threadBlockSize>>>(deviceA, deviceB, deviceResult); */
    cudaEventRecord(stop, 0);

    // check whether the kernel invocation was successful
    checkCudaCall(cudaGetLastError());

    // copy result back
    checkCudaCall(cudaMemcpy(list_h, list_d, sizeof(float), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(list_d));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cout << "kernel invocation took " << elapsedTime << " milliseconds" << endl;
    return *list_h;
}


int main(int argc, char* argv[])
{
    int i_max;

    if (argc < 3) {
        printf("Usage: %s i_max\n", argv[0]);
        printf(" - i_max: number of discrete points in the list, should be >2\n");
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


    // make a list of floats
    float list[i_max];
    for (int i = 0; i < i_max; i++){
        list[i] = (float)rand()/((float)RAND_MAX/FLT_MAX);
        printf("List[%d]: %f\n", i, list[i]);
    }

    cout << "max: " << max_array(list, i_max) << endl;

    return EXIT_SUCCESS;
}
