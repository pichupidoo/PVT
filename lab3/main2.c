#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define PI 3.14

double func(double x, double y)
{
    return x / pow(y, 2);
}

double getrand_sereal()
{
    return (double)rand() / RAND_MAX;
}

double getrand_parallel(unsigned int *seed)
{
    return (double)rand_r(seed) / RAND_MAX;
}

double sereal()
{
    double t = omp_get_wtime();
    const int n = 10000000;
    double s = 0;

    for (int i = 0; i < n; i++)
    {
        double x = getrand_sereal();         /* x in [0, 1] */
        double y = 3 * getrand_sereal() + 2; /* y in [2, 5] */
        s += func(x, y);
    }
    double v = 0.5;
    double res = v * s / n;
    printf("Result: %.12f, n %d\n", res, n);
    t = omp_get_wtime() - t;
    printf("Elapsed time sereal(sec.): %.3f\n", t);
    return t;
}

double parallel()
{
    double t = omp_get_wtime();
    const int n = 10000000;
    printf("Numerical integration by Monte Carlo method: n = %d\n", n);
    // int in = 0;
    double s = 0;
    int in = 0;

#pragma omp parallel
    {
        double s_loc = 0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();
#pragma omp for nowait
        for (int i = 0; i < n; i++)
        {
            double x = getrand_parallel(&seed);         /* x in [0, 1] */
            double y = 3 * getrand_parallel(&seed) + 2; /* y in [2, 5] */
            if (x < y)
            {
                s_loc += func(x, y);
                in_loc++;
            }
        }
#pragma omp atomic
        in += in_loc;

#pragma omp atomic
        s += s_loc;
    }
    double v = 0.5;
    double res = v * s / in;
    printf("Result: %.12f, n %d\n", res, n);
    t = omp_get_wtime() - t;
    printf("Elapsed time parallel(sec.): %.3f\n", t);
    return t;
}

int main()
{
    double t1 = sereal();
    double t2 = parallel();
    double t3 = t1 / t2;

    printf("speedup: %.2f\n", t3);

    return 0;
}
