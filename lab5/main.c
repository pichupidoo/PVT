#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 100000000
#define THRESHOLD 1000

void swap(int *x, int *y)
{
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

void partition(int *v, int *i, int *j, int low, int high)
{
    *i = low;
    *j = high;
    int pivot = v[(low + high) / 2];
    do
    {
        while (v[*i] < pivot)
            (*i)++;
        while (v[*j] > pivot)
            (*j)--;
        if (*i <= *j)
        {
            swap(&(v[*i]), &(v[*j]));
            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void fquicksort_tasks(int *v, int low, int high)
{
    int i, j;
    partition(v, &i, &j, low, high);
    if (high - low < THRESHOLD || (j - low < THRESHOLD || high - i < THRESHOLD))
    {
        if (low < j)
            fquicksort_tasks(v, low, j);
        if (i < high)
            fquicksort_tasks(v, i, high);
    }
    else
    {
#pragma omp task
        {
            fquicksort_tasks(v, low, j);
        }
#pragma omp task
        {
            fquicksort_tasks(v, i, high);
        }
#pragma omp taskwait
    }
}

double quicksort_tasks(int *v, int low, int high)
{
    double t = wtime();
#pragma omp parallel
    {
#pragma omp single
        {
            fquicksort_tasks(v, low, high);
        }
    }
    t = wtime() - t;
    return t;
}

void fquicksort(int *v, int low, int high)
{
    int i, j;
    partition(v, &i, &j, low, high);
    if (low < j)
        fquicksort(v, low, j);
    if (i < high)
        fquicksort(v, i, high);
}

double quicksort(int *v, int low, int high)
{
    double t = wtime();
    fquicksort(v, low, high);
    t = wtime() - t;
    return t;
}

void init(int **arr)
{
    for (int i = 0; i < N; i++)
        (*arr)[i] = rand() % 100;
}

void renew(int *arr, int *arr_loc)
{
    for (int i = 0; i < N; i++)
        arr_loc[i] = arr[i];
}

int main()
{
    int *arr_loc = malloc(sizeof(int) * N);
    int *arr = malloc(sizeof(int) * N);
    init(&arr);

    renew(arr, arr_loc);

    double t1 = quicksort(arr_loc, 0, N - 1);

    renew(arr, arr_loc);

    int num_threads_list[] = {2, 4, 6, 8};
    int num_tests = sizeof(num_threads_list) / sizeof(num_threads_list[0]);
    double t2;
    for (int i = 0; i < num_tests; i++)
    {
        omp_set_num_threads(num_threads_list[i]);
        t2 = quicksort_tasks(arr_loc, 0, N - 1);
        double speedup = t1 / t2;
        printf("Speedup with %d threads: %.2f\n", num_threads_list[i], speedup);
        renew(arr, arr_loc);
    }

    free(arr_loc);
    free(arr);

    return 0;
}