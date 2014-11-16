#include <stdio.h>
#include <stdlib.h>

int g_matrix_n, g_matrix_m;

int* sum_rows(int** matrix)
{
    int *sum_vector = malloc(sizeof(int) * g_matrix_n);
    if (!sum_vector) {
        fprintf(stderr, "Failed to malloc sum_vector");
        return NULL;
    }

    for (int i = 0; i < g_matrix_n; i++) {
        sum_vector[i] = 0;
    }

    for (int row = 0; row < g_matrix_n; row++) {
        for (int col = 0; col < g_matrix_m; col++) {
            fprintf(stderr, "i%d, j%d\n", row, col);
            fprintf(stderr, "matr: %d\n", matrix[col][row]); //SEGFAULT
            sum_vector[row] += matrix[col][row];
        }
    }
}

int** init_matrix(int matrix_n, int matrix_m)
{
    /* Amount of rows */
    g_matrix_n = matrix_n;
    /* Amount of columns */
    g_matrix_m = matrix_n;

    int **matrix = malloc(sizeof(int*) * matrix_m);
    if (!matrix) {
        fprintf(stderr, "Failed to malloc matrix");
        return NULL;
    }
    for (int i = 0; i < matrix_n; i++) {
        matrix[i] = malloc(sizeof(int) * matrix_n);
        if (!matrix[i]) {
            fprintf(stderr, "Failed to malloc matrix[%d]", i);
            return NULL;
        }
    }
    for (int i = 0; i < g_matrix_n; i++) {
        for (int j = 0; j < g_matrix_m; j++) {
            matrix[i][j] = ((2 * j + ((i + 1) * i % 8)) / (i + 1 + (j % 3))) % 10;
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{

    int** matrix;
    matrix = init_matrix(10, 10);
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
