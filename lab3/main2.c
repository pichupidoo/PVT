#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define PI 3.14

double func(double x, double y)
{ return x / pow(y, 2); }

double getrand(unsigned int *seed)
{
    return (double)rand_r(seed) / RAND_MAX;
}

int main(/*int argc, char **argv*/)
{
    const int n = 10000000;
    printf("Numerical integration by Monte Carlo method: n = %d\n", n);
    int in = 0;
    double s = 0;

#pragma omp parallel
    {
        double s_loc = 0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();
#pragma omp for nowait
        for (int i = 0; i < n; i++)
        {
            double x = getrand(&seed) * 1; /* x in [0, pi] */
            double y = getrand(&seed);      /* y in [0, sin(x)] */
            if (y <= sin(x))
            {
                in_loc++;
                s_loc += func(x, y);
            }
        }
#pragma omp atomic
        s += s_loc;
#pragma omp atomic
        in += in_loc;
    }
    double v = PI * in / n;
    double res = v * s / in;
    printf("Result: %.12f, n %d\n", res, n);
    return 0;
}