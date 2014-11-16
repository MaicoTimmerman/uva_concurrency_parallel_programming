#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int matrix_n, matrix_m = 10;
    int **matrix = malloc(sizeof(int*) * matrix_n);
    for (int i = 0; i < matrix_n; i++) {
        matrix[i] = malloc(sizeof(int) * matrix_m);
    }

    matrix[4][1] = 6;
    /* printf("%d", matrix[4][1]); */


    return EXIT_SUCCESS;
}
