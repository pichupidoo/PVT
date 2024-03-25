#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <omp.h>
#include <stdlib.h>

extern double res = 0;

double wtime ( )
{
    clock_t start, end;
    double cpu_time_used;
    double value;

  value = ( double ) clock ( ) 
        / ( double ) CLOCKS_PER_SEC;

  return value;
}

void matrix_vector_product_omp(double *a, double *b, double *c, int m, int n)
{
#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
        for (int i = lb; i <= ub; i++)
        {
            c[i] = 0.0;
            for (int j = 0; j < n; j++)
                c[i] += a[i * n + j] * b[j];
        }
    }
}
double run_parallel(int m, int n)
{
    double *a, *b, *c;
    // Allocate memory for 2-d array a[m, n]
    a = (double *)malloc(sizeof(*a) * m * n);
    b = (double *)malloc(sizeof(*b) * n);
    c = (double *)malloc(sizeof(*c) * m);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }
    for (int j = 0; j < n; j++)
        b[j] = j;
    //double t = wtime();
    double t = omp_get_wtime();
    matrix_vector_product_omp(a, b, c, m, n);
    //t = wtime() - t;
    //t = t - res;
    t = omp_get_wtime() - t;
    printf("Elapsed time (parallel): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
    return t;
}

void matrix_vector_product(double *a, double *b, double *c, int m, int n)
{
    for (int i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

double run_serial(int m, int n)
{
    double *a, *b, *c;
    
    a = (double *)malloc(sizeof(*a) * m * n);
    b = (double *)malloc(sizeof(*b) * n);
    c = (double *)malloc(sizeof(*c) * m);
    
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }
    for (int j = 0; j < n; j++)
        b[j] = j;
    //double t = wtime();
    double t = omp_get_wtime();
    matrix_vector_product(a, b, c, m, n);
    t = omp_get_wtime() - t;
    /*t = wtime() - t;
    res = t;*/
    printf("Elapsed time (serial): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
    return t;
}

int main(int argc, char **argv)
{
    int m = 15000, n = 15000;
    double t, res;
    printf("Matrix-vector product (c[m] = a[m, n] * b[n]; m = %d, n = %d)\n", m, n);
    printf("Memory used: %" PRIu64 " MiB\n", ((m * n + m + n) * sizeof(double)) >> 20);
    res = run_serial(m ,n);
    t = run_parallel(m, n);
    t = res / t;
    printf("speedup: %f\n", t);
    return 0;
}