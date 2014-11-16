#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

int g_matrix_n, g_matrix_m, g_num_threads;

typedef struct matrix_t {
    int** values;
    int* row_size;
} matrix_t;

void print_matrix(matrix_t*);

/* Sum each row of the provided matrix, using the row_size to determine how many items are in a row */
int* sum_rows(matrix_t* matrix)
{
    int *sum_vector = malloc(sizeof(int) * g_matrix_n);
    if (!sum_vector) {
        fprintf(stderr, "Failed to malloc sum_vector\n");
        return NULL;
    }

    #pragma omp parallel for schedule(static) num_threads(g_num_threads)
    for (int i = 0; i < g_matrix_n; i++) {
        sum_vector[i] = 0;
    }

    #pragma omp parallel for schedule(static) num_threads(g_num_threads)
    for (int n = 0; n < g_matrix_n; n++) {
        for (int m = 0; m < matrix->row_size[n]; m++) {
            sum_vector[n] += matrix->values[n][m];
        }
    }
    return sum_vector;
}

/* Create a matrix, either triangular or non triangular and fill it with the appropriate values */
matrix_t* matrix(int is_triangular)
{
    /* Malloc matrix struct */
    matrix_t* matrix = malloc(sizeof(matrix_t*));
    if (!matrix) {
        fprintf(stderr, "Failed to malloc struct matrix\n");
        return NULL;
    }

    /* Malloc matrix values */
    matrix->values = malloc(sizeof(int*) * g_matrix_m);
    if (!matrix->values) {
        fprintf(stderr, "Failed to malloc matrix\n");
        return NULL;
    }

    /* Malloc matrix row sizes */
    matrix->row_size = malloc(sizeof(int) * g_matrix_n);
    if (!matrix->row_size) {
        fprintf(stderr, "Failed to malloc row size\n");
        return NULL;
    }

    /* Malloc matrix columns */
    #pragma omp parallel for schedule(static) num_threads(g_num_threads)
    for (int i = 0; i < g_matrix_n; i++) {
        matrix->row_size[i] = g_matrix_n - (i * is_triangular);
        matrix->values[i] = malloc(sizeof(int) * matrix->row_size[i]);
        if (!matrix->values[i]) {
            fprintf(stderr, "Failed to malloc matrix[%d]\n", i);
        }
    }

    /* Matrix[n][m] n = vertical, m = horizontal. eg. Matrix[2][3] is 2nd row (from top) 3rd value. */
    /* n is vert size m = hori size */
    #pragma omp parallel for schedule(static) num_threads(g_num_threads)
    for (int n = 0; n < g_matrix_n; n++) {
        //#pragma omp parallel for schedule(static) num_threads(g_num_threads)
        for (int m = 0; m < matrix->row_size[n]; m++) {
            matrix->values[n][m] = n + (m + (g_matrix_m - matrix->row_size[n]));
        }
    }
    return matrix;

}

matrix_t* init_matrix()
{
    /* Make a normal, non-triangular matrix */
    return matrix(0);
}


matrix_t* init_matrix_triangular()
{
    /* Make a triangular matrix */
    return matrix(1);
}

/* Print a matrix */
void print_matrix(matrix_t* matrix)
{
    for (int n = 0; n < g_matrix_n; n++) {
        for (int m = 0; m < matrix->row_size[n]; m++) {
            printf("%d ", matrix->values[n][m]);
            if (matrix->values[n][m] < 10) {
                printf("    ");
            }
            else if (matrix->values[n][m] < 100) {
                printf("   ");
            }
            else if (matrix->values[n][m] < 1000) {
                printf("  ");
            }
            else if (matrix->values[n][m] < 10000) {
                printf(" ");
            }

        }
        printf("\n");
    }
    return;

}


int main(int argc, char* argv[])
{

    double time;
    int* sum_vector;

    /* We allow only square matrices */
    g_matrix_n = g_matrix_m = atoi(argv[1]);
    g_num_threads = atoi(argv[2]);

    matrix_t* matrix;
    matrix = init_matrix_triangular();

    if (!matrix) {
        return EXIT_FAILURE;
    }

    timer_start();

    sum_vector = sum_rows(matrix);
    if (!sum_vector) {
        return EXIT_FAILURE;
    }

    time = timer_end();
    printf("%d, %d, %lf\n", g_matrix_n, g_num_threads, time);

    /* print_matrix(matrix); */

    /* Free this stupid shit */
    for (int i = 0; i < g_matrix_n; i++) {
        free(matrix->values[i]);
    }
    free(matrix->values);
    free(matrix->row_size);
    free(matrix);
    free(sum_vector);


    return EXIT_SUCCESS;
}
