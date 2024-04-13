#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>

double func(double x, double y)
{
    return x / pow(y, 2);
}

double getrand_sereal()
{
    return (double)rand() / RAND_MAX;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    const int n = 10000000;
    double s = 0;
    
    for (int i = 0; i < n; i++)
    {
        double x = getrand_sereal() ; /* x in [0, 1] */
        double y = 3 * getrand_sereal() + 2;      /* y in [2, 5] */
        s += func(x, y);
    }
    double v = 0.5;
    double res = v * s / n;
    printf("Result: %.12f, n %d\n", res, n);
    return 0;
}
