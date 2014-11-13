#include <stdio.h>
#include <stdlib.h>

/* Code copied from PDF */
double sum (double* vec, int len)
{
    int i;
    double accu = 0.0;
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
    for (i = 0; i < len; i++) {
        accu = fun(accu, vec[i]);
    }
    return accu;
}

int main(int argc, char *argv[])
{
    return EXIT_SUCCESS;
}
