#include <stdio.h>
#include <math.h>
#include <omp.h>

double func(double x)
{
    return (1 - exp((0.7) / (x))) / (2 + x);
}

double sereal()
{
    double t = omp_get_wtime();
    const double eps = 1E-5;
    const int n0 = 100000000;
    const double a = 0.4;
    const double b = 1.5;
    int n = n0, k;
    double sq[2], delta = 1;
    for (k = 0; delta > eps; n *= 2, k ^= 1)
    {
        double h = (b - a) / n;
        double s = 0.0;
        for (int i = 0; i < n; i++)
            s += func(a + h * (i + 0.5));
        sq[k] = s * h;
        if (n > n0)
            delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
    }
    printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    t = omp_get_wtime() - t;
    printf("Elapsed time sereal(sec.): %.3f\n", t);
    return t;
}

double parallel(int threads)
{
    double t = omp_get_wtime();
    const double eps = 1E-5;
    const double a = 0.4;
    const double b = 1.5;
    const int n0 = 100000000;
    printf("Numerical integration: [%f, %f], n0 = %d, EPS = %f\n", a, b, n0, eps);
     
    double sq[2];
#pragma omp parallel num_threads(threads)
    {
        int n = n0, k;
        double delta = 1;
        for (k = 0; delta > eps; n *= 2, k ^= 1)
        {
            double h = (b - a) / n;
            double s = 0.0;
            sq[k] = 0;
// Ждем пока все потоки закончат обнуление sq[k], s
#pragma omp barrier
#pragma omp for nowait
            for (int i = 0; i < n; i++)
                s += func(a + h * (i + 0.5));
#pragma omp atomic
            sq[k] += s * h;
// Ждем пока все потоки обновят sq[k]
#pragma omp barrier
            if (n > n0)
                delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
#if 0
printf("n=%d i=%d sq=%.12f delta=%.12f\n", n, k, sq[k], delta);
#endif
        }
#pragma omp master
        printf("Result with %d threads: %.12f; Runge rule: EPS %e, n %d\n", threads, sq[k] * sq[k], eps, n / 2);
    }
    t = omp_get_wtime() - t;
    printf("Elapsed time parallel(sec.): %.3f\n", t);

    return t;
}

int main()
{
    int num_threads_list[] = {2, 4, 6, 8}; 
    int num_tests = sizeof(num_threads_list) / sizeof(num_threads_list[0]);

    double t1 = sereal(); 

    for (int i = 0; i < num_tests; i++)
    {
        double t2 = parallel(num_threads_list[i]); 
        double speedup = t1 / t2; 
        printf("Speedup with %d threads: %.2f\n", num_threads_list[i], speedup);
    }

    return 0;
}
