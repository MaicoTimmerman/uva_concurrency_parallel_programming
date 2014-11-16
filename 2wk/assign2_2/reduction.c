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


double reduce(double (fun)(double, double),
        double* vec, int len, double neutral)
{
    int i;
    double accu = neutral;
    /* #pragma omp parallel for num_threads(g_num_threads) */
    for (i = 0; i < len; i++) {
        accu = fun(accu, vec[i]);
    }
    return accu;
}

double reduce2(double (fun)(double, double),
        double* vec, int len, double neutral)
{
    int vec_len;
    int calc_vec_len;
    double accu;

    accu = neutral;
    vec_len = len;
    double *calc_vec = (double*)malloc(sizeof(double)*vec_len);
    double *free_calc_vec = calc_vec;

    /* While the vector has more then one value, keep calculating */
    while (vec_len > 1) {

        calc_vec_len = pow(2, floor(log2(vec_len)));
        fprintf(stderr, "calc_vec_len: %d", calc_vec_len);
        calc_vec = vec;

        /* Current vector with the length == 2^n */
        while (calc_vec_len > 1) {

            /* halveer telkens de lengte van huidige vector door functie
             * toe te passen op alle tweetallen */
            for (int i = 0; i < calc_vec_len; i+=2) {
                calc_vec[i] = fun(calc_vec[(i*2)], calc_vec[(i*2)]);
            }
            calc_vec_len = calc_vec_len / 2;
        }

        accu = fun(accu, calc_vec[0]);

        /* The calc_vec is converted into a single number */
        vec_len = vec_len - calc_vec_len;
        vec = vec+calc_vec_len;
    }

    accu = fun(accu, vec[0]);

    free(free_calc_vec);

    return accu;
}

double fun(double a, double b) {
    return a+b;
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
    /* accu = sum(vec, vec_size); */
    accu = reduce(fun, vec, vec_size, 0);
    printf("sum accu: %g\n", accu);
    accu = reduce2(fun, vec, vec_size, 0);
    printf("sum accu: %g\n", accu);

    /* Stop timing */
    time = timer_end();
    printf("Took %g seconds\n", time);

    free(vec);

    return EXIT_SUCCESS;
}
