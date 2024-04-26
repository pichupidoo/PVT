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

double serial()
{
    double t = omp_get_wtime();
    const int n = 100000000;
    double s = 0;
    int in = 0;


    for (int i = 0; i < n; i++)
    {
        double x = getrand_sereal(); /* x in [-1, 0] */
        double y = 3 * getrand_sereal() + 2;        /* y in [0, 1] */
        if (y <= (1 / pow(x,2))) {
            in++;
            s += func(x, y);
        }
    }
    /*double v = 0.63;
    double res = v * s / n;*/
    double v = PI * in / n;
    printf("v=%lf\n", v);
    double res = v * s / in;

    printf("Serial result: %.12f, n %d\n", res, n);
    t = omp_get_wtime() - t;
    printf("Elapsed time serial(sec.): %.3f\n", t);
    return t;
}

double parallel(int threads)
{
    double t = omp_get_wtime();
    const int n = 100000000;
    printf("Numerical integration by Monte Carlo method: n = %d\n", n);
    double s = 0;
    int in = 0;

     

#pragma omp parallel num_threads(threads)
    {
        double s_loc = 0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();
#pragma omp for nowait
        for (int i = 0; i < n; i++)
        {
            double x = getrand_parallel(&seed); /* x in [0, 1] */
            double y = 3 * getrand_parallel(&seed) + 2;        /* y in [2, 5] */
            if (y <= (1 / pow(x,2))) {
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
    printf("Parallel result with %d threads: %.12f, n %d\n", threads, res, n);
    t = omp_get_wtime() - t;
    printf("Elapsed time parallel(sec.): %.3f\n", t);
    return t;
}

int main()
{
    int num_threads_list[] = {2, 4, 6, 8}; 
    int num_tests = sizeof(num_threads_list) / sizeof(num_threads_list[0]);

    double t1 = serial(); 

    for (int i = 0; i < num_tests; i++)
    {
        double t2 = parallel(num_threads_list[i]);
        double speedup = t1 / t2; 
        printf("Speedup with %d threads: %.2f\n", num_threads_list[i], speedup);
    }

    return 0;
}
