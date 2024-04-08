
/*double func(double x)
{
    return exp(-x * x);
}
int main(int argc, char **argv)
{
    double t = omp_get_wtime();
    const double a = -4.0;
    const double b = 4.0;
    const int n = 10000000;
    printf("Numerical integration: [%f, %f], n = %d\n", a, b, n);
    double h = (b - a) / n;
    double s = 0.0;
    double sumloc[omp_get_max_threads()];
#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int points_per_thread = n / nthreads;
        int lo = tid * points_per_thread;
        int hi = (tid == nthreads - 1) ? n - 1 : lo + points_per_thread;
        sumloc[tid] = 0.0;
        for (int i = lo; i <= hi; i++)
            sumloc[tid] += func(a + h * (i + 0.5));
#pragma omp atomic
        s += sumloc[tid];
    }
    s *= h;
    printf("Result Pi: %.12f\n", s * s);
    t = omp_get_wtime() - t;
    printf("Elapsed time (sec.): %.12f\n", t);
    return 0;
}*/

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
    const double eps = 1E-10;
    const int n0 = 100;
    const double a = 1;
    const double b = 2;
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
    return 0;
}


double parallel()
{
    double t = omp_get_wtime();
    const double eps = 1E-6;
    const double a = 1;
    const double b = 2;
    const int n0 = 100000000;
    printf("Numerical integration: [%f, %f], n0 = %d, EPS = %f\n", a, b, n0, eps);
    // продолжение main()
    double sq[2];
#pragma omp parallel
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
        printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    }
    t = omp_get_wtime() - t;
    printf("Elapsed time parallel(sec.): %.3f\n", t);

    return t;
}

int main()
{
    double t1 = sereal();
    double t2 = parallel();

    printf("speedup: %.2f\n", t1/t2);

    return 0;
}