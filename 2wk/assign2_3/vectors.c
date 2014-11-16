#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int g_matrix_n, g_matrix_m, g_num_threads;
char* g_schedule;

typedef struct matrix_t {
    int** values;
    int* row_size;
} matrix_t;

int* sum_rows(matrix_t* matrix)
{
    int *sum_vector = malloc(sizeof(int) * g_matrix_n);
    if (!sum_vector) {
        fprintf(stderr, "Failed to malloc sum_vector\n");
        return NULL;
    }

    for (int i = 0; i < g_matrix_n; i++) {
        sum_vector[i] = 0;
    }

    #pragma omp parallel for schedule(g_schedule) num_threads(g_num_threads)
    for (int n = 0; n < g_matrix_n; n++) {
        for (int m = 0; m < matrix->row_size[n]; m++) {
            sum_vector[n] += matrix->values[n][m];
        }
    }
}

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
    #pragma omp parallel for schedule(g_schedule) num_threads(g_num_threads)
    for (int i = 0; i < g_matrix_n; i++) {
        matrix->row_size[i] = g_matrix_n - (i * is_triangular);
        matrix->values[i] = malloc(sizeof(int) * matrix->row_size[i]);
        if (!matrix->values[i]) {
            fprintf(stderr, "Failed to malloc matrix[%d]\n", i);
            for (int x = 0; x < i; x++) {
                free(matrix->values[x]);
            }
            free(matrix->values);
            free(matrix);
            return NULL;
        }
    }

    /* Matrix[n][m] n = vertical, m = horizontal. eg. Matrix[2][3] is 2nd row (from top) 3rd value. */
    /* n is vert size m = hori size */
    int max_val = 2;
    #pragma omp parallel for schedule(g_schedule) num_threads(g_num_threads)
    for (int n = 0; n < g_matrix_n; n++) {
        #pragma omp parallel for schedule(g_schedule) num_threads(g_num_threads)
        for (int m = 0; m < matrix->row_size[n]; m++) {
            matrix->values[n][m] = n + (m + (g_matrix_m - matrix->row_size[n]));
        }
    }
    return matrix;

}

matrix_t* init_matrix()
{
    return matrix(0);
}


matrix_t* init_matrix_triangular()
{
    return matrix(1);
}

int main(int argc, char* argv[])
{

    /* We allow only square matrices */
    g_matrix_n = g_matrix_m = atoi(argv[1]);
    g_schedule = argv[3];
    g_num_threads = atoi(argv[2]);

    matrix_t* matrix;
    matrix = init_matrix_triangular();

    if (!matrix) {
        return EXIT_FAILURE;
    }

    int* sum_vector;
    sum_vector = sum_rows(matrix);
    if (!sum_vector) {
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
