#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#include "timer.h"

/* Code copied from PDF */
double sum(double* vec, int len)
{
    int i;
    double accu = 0.0;
    #pragma omp parallel for reduction(+:accu)
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
    #pragma omp parallel for
    for (i = 0; i < len; i++) {
        #pragma omp critical
        accu = fun(accu, vec[i]);
    }
    return accu;
}

double reduce2(double (fun)(double, double),
        double* vec, int len, double neutral)
{
    /* We need a copy of the original head of the vector, as well as a vector to work in */
    double* start_vec = vec;
    double* calc_vec = NULL;

    /* We calculate the largest power of two (n) that will fit in len */
    int calc_vec_len_copy;
    int vec_len = len;
    int calc_vec_len = (int)pow(2, floor(log2(vec_len)));

    double accu = neutral;


    /* Continue until the whole original vector has been reduced */
    while (vec_len != 0) {

        /* Remove the current working vector from the original vector. */
        vec_len -= calc_vec_len;

        /* Save the length of the the current working vector to continue
         * with the leftovers of the original vector. */
        calc_vec_len_copy = calc_vec_len;

        /* While the current working vector has not yet been reduced to zero,
         * keep merging sets of 2 with the function given. */
        while (calc_vec_len != 1) {

            /* New vector for the results, automatically remove the old. */
            calc_vec = malloc((calc_vec_len / 2) * sizeof(double));

            /* The merging of sets using the function can be done parallel
             * Because there are no race conditions in this loop. */
            #pragma omp parallel for
            for (int i = 0; i < calc_vec_len; i+=2)
                calc_vec[i/2] = fun(vec[i], vec[i+1]);

            /* Prepare for next step */
            vec = calc_vec;
            calc_vec_len = calc_vec_len / 2;
        }

        /* Working vector has been reduced to a single term. Reduce with
         * the previously done parts. */
        accu = fun(accu, vec[0]);


        /* Shift the original vector beyond all the done indices */
        start_vec += calc_vec_len_copy;
        vec = start_vec;

        /* Calculate the length of the new working vector */
        calc_vec_len = (int)pow(2, floor(log2(vec_len)));

    }

    return accu;

}

double fun(double a, double b) {
    return (a+a+b+b);
}

int main(int argc, char *argv[])
{
    int num_threads;
    int vec_size;
    int custom_reduce_method;
    double accu;
    double time;
    double *vec;

    if (argc < 4) {
        printf("Usage: %s num_threads vec_size [reduce method]\n", argv[0]);
        printf(" - num_threads: number of threads to use for simulation, "
                "should be >=1\n");
        printf(" - vec_size: size of the vector to do reduction on 10^n"
                "should be >=10\n");
        printf(" - [reduce_method]: custom | sequential.");

        return EXIT_FAILURE;
    }

    num_threads = atoi(argv[1]);
    vec_size = pow(10, atoi(argv[2]));
    if (num_threads < 1) {
        printf("argument error: num_threads should be >=1.\n");
        return EXIT_FAILURE;
    }

    if (vec_size < 4) {
        printf("argument error: vec_size should be >=4.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[3], "sequential") == 0) {
        custom_reduce_method = 0;
    } else {
        custom_reduce_method = 1;
    }

    vec = (double*)malloc(vec_size * sizeof(double));

    /* Fill a vector with a sinus values */
    #pragma omp parallel for
    for (int i = 0; i < vec_size; i++) {
        vec[i] = sin(i);
    }


    omp_set_num_threads(num_threads);

    /* Start timing the effeciency of openMP */
    timer_start();

    /* Calculate a reduced vector with openMP */
    if (custom_reduce_method) {
        accu = reduce2(fun, vec, vec_size, 0);
        /* accu = sum(vec, vec_size); */
    } else {
        accu = reduce(fun, vec, vec_size, 0);
    }
    /* Stop timing */

    time = timer_end();
    printf("%d, %d, %g\n",num_threads, vec_size, time);

    free(vec);
    vec = NULL;

    return EXIT_SUCCESS;
}
