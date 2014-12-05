#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "timer.h"
#include <iostream>
#include <iostream>
#include <fstream>

using namespace std;
typedef double (*func_t)(double x);

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

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}

/* Fill an double array with the function given */
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
    }
}

__global__ void vectorAddKernel(unsigned int i_max, double* old_d, double* cur_d, double* next_d) {
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    if ((i > 0) || (i < i_max)) {
        next_d[i] = (2*cur_d[i]) - old_d[i] + (0.15*(cur_d[i-1] - (2*cur_d[i] - cur_d[i+1])));
    } else {
        next_d[i] = 0;
    }
}


void vectorAddCuda(int i_max, int t_max, double* old_h, double* cur_h, double* next_h) {
    int threadBlockSize = 1024;
    double *tmp;

    // allocate the vectors on the GPU
    double* old_d = NULL;
    checkCudaCall(cudaMalloc((void **) &old_d, i_max * sizeof(double)));
    if (old_d == NULL) {
        cout << "could not allocate memory!" << endl;
        return;
    }

    double* cur_d = NULL;
    checkCudaCall(cudaMalloc((void **) &cur_d, i_max * sizeof(double)));
    if (cur_d == NULL) {
        checkCudaCall(cudaFree(old_d));
        cout << "could not allocate memory!" << endl;
        return;
    }

    double* next_d = NULL;
    checkCudaCall(cudaMalloc((void **) &next_d, i_max * sizeof(double)));
    if (next_d == NULL) {
        checkCudaCall(cudaFree(old_d));
        checkCudaCall(cudaFree(cur_d));
        cout << "could not allocate memory!" << endl;
        return;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // copy the original vectors to the GPU
    checkCudaCall(cudaMemcpy(old_d, old_h, i_max*sizeof(double), cudaMemcpyHostToDevice));
    checkCudaCall(cudaMemcpy(cur_d, cur_h, i_max*sizeof(double), cudaMemcpyHostToDevice));

    // execute kernel
    cudaEventRecord(start, 0);
    for (int i = 0; i < t_max; i++) {
        vectorAddKernel<<<i_max/threadBlockSize, threadBlockSize>>>(i_max, old_d, cur_d, next_d);
        tmp = old_d;
        old_d = cur_d;
        cur_d = next_d;
        next_d = tmp;
    }
    cudaEventRecord(stop, 0);

    // check whether the kernel invocation was successful
    checkCudaCall(cudaGetLastError());

    // copy result back
    checkCudaCall(cudaMemcpy(cur_h, cur_d, i_max * sizeof(double), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(old_d));
    checkCudaCall(cudaFree(cur_d));
    checkCudaCall(cudaFree(next_d));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cout << "kernel invocation took " << elapsedTime << " milliseconds" << endl;
}

int main(int argc, char *argv[])
{
    int t_max, i_max;

    /* Parse commandline args: i_max t_max num_threads */
    if (argc < 3) {
        printf("Usage: %s i_max t_max\n", argv[0]);
        printf(" - i_max: number of discrete amplitude points, should be >2\n");
        printf(" - t_max: number of discrete timesteps, should be >=1\n");
        return EXIT_FAILURE;
    }

    /* Only accept powers of two */
    i_max = atoi(argv[1]);
    if (!(!(i_max == 0) && !(i_max & (i_max - 1)))) {
        cout << "Not a power of two" << endl;
        exit(0);
    }

    t_max = atoi(argv[2]);

    if (i_max < 3) {
        printf("argument error: i_max should be >2.\n");
        return EXIT_FAILURE;
    }
    if (t_max < 1) {
        printf("argument error: t_max should be >=1.\n");
        return EXIT_FAILURE;
    }

    /* Allocate and initialize buffers. */
    double *old = new double[i_max];
    double *cur = new double[i_max];
    double *next = new double[i_max];

    memset(old, 0, i_max * sizeof(double));
    memset(cur, 0, i_max * sizeof(double));
    memset(next, 0, i_max * sizeof(double));

    /* Fill the first two generations */
    fill(old, 1, i_max/4, 0, 2*3.14, sin);
    fill(cur, 2, i_max/4, 0, 2*3.14, sin);

    /* Start measuring the time */
    timer vectorAddTimer("vector add timer");
    vectorAddTimer.start();

    /* Call the actual simulation and measure the time */
    /* ret = simulate(i_max, t_max, num_threads, old, current, next); */

    /* Print the time it took */
    vectorAddTimer.stop();
    cout << vectorAddTimer << endl;

    /* Write the output */
    ofstream returnfile;
    returnfile.open("results.txt", ios::in);
    for (int i = 0; i < i_max; i++) {
        returnfile << cur[i] << endl;
    }
    returnfile.close();

    delete[] cur;
    delete[] old;
    delete[] next;

    return EXIT_SUCCESS;
}
