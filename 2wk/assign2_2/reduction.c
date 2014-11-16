#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "timer.h"

int g_num_threads;

/* Code copied from PDF */
double sum(double* vec, int len)
{
    int i;
    double accu = 0.0;
    #pragma omp parallel for num_threads(g_num_threads) reduction(+:accu)
    for (i = 0; i < len; i++) {
        accu = accu + vec[i];
    }
    return accu;
}


double reduce (double (fun)(double, double),
        double* vec, int len, double neutral)
{
    int i;
    double accu = neutral;
    #pragma omp parallel for num_threads(g_num_threads) reduction(+:accu)
    for (i = 0; i < len; i++) {
        accu = fun(accu, vec[i]);
    }
    return accu;
}

int main(int argc, char *argv[])
{
    int num_threads;
    int vec_size;
    double accu;
    double time;
    double *vec;

    if (argc < 3) {
        printf("Usage: %s num_threads vec_size\n", argv[0]);
        printf(" - num_threads: number of threads to use for simulation, "
                "should be >=1\n");
        printf(" - vec_size: size of the vector to do reduction on"
                "should be >=10\n");

        return EXIT_FAILURE;
    }

    num_threads = atoi(argv[1]);
    vec_size = atoi(argv[2]);
    if (num_threads < 1) {
        printf("argument error: num_threads should be >=1.\n");
        return EXIT_FAILURE;
    }

    if (vec_size < 10) {
        printf("argument error: num_threads should be >=1.\n");
        return EXIT_FAILURE;
    }

    vec = (double*)malloc(vec_size * sizeof(double));

    /* Fill a vector with a sinus values */
    #pragma omp parallel for num_threads(g_num_threads)
    for (int i = 0; i < vec_size; i++) {
        vec[i] = sin(i);
    }

    g_num_threads = num_threads;

    /* Start timing the effeciency of openMP */
    timer_start();

    /* Calculate a reduced vector with openMP */
    accu = sum(vec, vec_size);
    printf("sum accu: %g\n", accu);

    /* Stop timing */
    time = timer_end();
    printf("Took %g seconds\n", time);

    return EXIT_SUCCESS;
}
