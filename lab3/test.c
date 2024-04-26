#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

const double PI = 3.14;


double getrand(){ 
    return (double)rand() / RAND_MAX; 
}

double getrandomp(unsigned int *seed){ 
    return (double)rand_r(seed) / RAND_MAX; 
}

double func(double x, double y){
    return x/pow(y,2); 
}

double S(double time_nomp,double time_omp){
    return time_nomp/time_omp;
}

double wtime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1E-9;
}

double run_serial(int n){
    double t = wtime();
    int in = 0;
    double s = 0;
    for (int i = 0; i < n; i++) {
        double x = getrand(); /* x in [0, 1] */
        double y = 2+(getrand()*3); /* y in [2, 5] 3->ширина диапазона*/
        if (y <= 5 && y>=2) {
            in++;
            s += func(x, y);
        }
    }
    double v = PI * in / n;
    double res = v * s / in;
    printf("Result: %.12f, n %d\n", res, n);
    t = wtime() - t;
    return t;
}

double run_parallel(int f, int n){
    double t = wtime();
    int in = 0;
    double s = 0;
    #pragma omp parallel num_threads(f)
    {
        double s_loc = 0;
        int in_loc = 0;
        unsigned int seed = omp_get_thread_num();
        #pragma omp for nowait
        for (int i = 0; i < n; i++) {
            double x = getrandomp(&seed); /* x in [0, 1] */
            double y = 2+(getrandomp(&seed)*3); /* y in [2, 5] 3->ширина диапазона*/
            if (y <= 5 && y>=2) {
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
    t = wtime() - t;
    return t;
}

void write8(double S, int n){ 
  FILE *f; 
  f = fopen("mntkrl10^8.txt", "a"); 
  fprintf(f, "%d %f\n", n, S); 
  fclose(f); 
} 
void write7(double S, int n){ 
  FILE *f; 
  f = fopen("mntkrl10^7.txt", "a"); 
  fprintf(f, "%d %f\n", n, S); 
  fclose(f); 
} 

int main(int argc, char **argv)
{
    int n = 100000000;
    double time_nomp = run_serial(n);
    printf("Time serial: %.6f\n\n", time_nomp);
    for(int i = 2; i<5; i++){
        double time_omp = run_parallel(i,n);
        printf("Time parallel: %.6f\n", time_omp);
        printf("S: %.6f\n\n",S(time_nomp,time_omp));
        write8(S(time_nomp,time_omp),i);
    }

    n = 10000000;
    for(int i = 2; i<5; i++){
        double time_omp = run_parallel(i,n);
        printf("Time parallel: %.6f\n", time_omp);
        printf("S: %.6f\n\n",S(time_nomp,time_omp));
        write7(S(time_nomp,time_omp),i);
    }
    return 0;
}