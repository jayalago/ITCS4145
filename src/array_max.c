#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* Returns the current time in seconds as a double. */
double now_sec(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9; //seconds + nanoseconds to a single double
}

/* Fillers */
void fill_int_sorted(int *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = i;
}

void fill_int_reverse(int *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = n - i - 1;
}

void fill_int_random(int *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = rand();
}

void fill_double_sorted(double *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = (double)i;
}

void fill_double_reverse(double *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = (double)(n - i - 1);
}

void fill_double_random(double *a, size_t n){
    for (size_t i = 0; i < n; i++) a[i] = (double)rand() / RAND_MAX;
}   

/* Kernels */
int max_int_branch(const int *a, size_t n){
    int m = a[0];
    for (size_t i = 1; i < n; i++){
        if (a[i] > m) m = a[i];
    }
    return m;
}

int max_int_branchless(const int *a, size_t n){
    int m = a[0];
    for (size_t i = 1; i < n; i++){
        m = (a[i] > m) ? a[i] : m;
    }
    return m;
}

double max_double_branch(const double *a, size_t n){
    double m = a[0];
    for (size_t i = 1; i < n; i++){
        if (a[i] > m) m = a[i];
    }
    return m;
}

double max_double_branchless(const double *a, size_t n){
    double m = a[0];
    for (size_t i = 1; i < n; i++){
        m = (a[i] > m) ? a[i] : m;
    }
    return m;
}

int main(){
    srand(12345); // Seed the random number generator for reproducibility
    size_t sizes[] = {1000000, 10000000, 100000000}; // Array sizes to test
    const char *orders[] = {"sorted", "reverse", "random"}; // Different orderings of the array
    printf("type,variant,order,n,avg,median,min,max,bandwith\n"); // Print CSV header

    /*Measurements*/
    for (int si = 0; si < 3; si++){
        size_t n = sizes[si];
        int *ai = malloc(n * sizeof(int)); // Allocate memory for integer array
        double *ad = malloc(n * sizeof(double)); // Allocate memory for double array

        for (int oi = 0; oi < 3; oi++){
            const char *ord = orders[oi];
            /* Fill integer array */
            if (oi == 0) fill_int_sorted(ai, n);
            else if (oi == 1) fill_int_reverse(ai, n);
            else fill_int_random(ai, n);
            /* Fill double */
            if (oi == 0) fill_double_sorted(ad, n);
            else if (oi == 1) fill_double_reverse(ad, n);
            else fill_double_random(ad, n);
            /* Measure int branch + branchless */
            for (int v = 0; v < 2; v++){
                double times[10];
                volatile int sink; // Prevent compiler optimization
                /* Warm up */
                sink = (v == 0) ? max_int_branch(ai, n) : max_int_branchless(ai, n);
                /* 10 runs */
                for (int r = 0; r < 10; r++){
                    double t0 = now_sec();
                    int m = (v == 0) ? max_int_branch(ai, n) : max_int_branchless(ai, n);
                    double t1 = now_sec();
                    sink ^=m; // Prevent compiler optimization
                    times[r] = t1 - t0;
                }
                /* stats */
                double min = times[0], max = times[0], sum = 0;
                for (int r = 0; r < 10; r++){
                    sum += times[r];
                    if (times[r] < min) min = times[r]; // Find minimum time
                    if (times[r] > max) max = times[r]; // Find maximum time
                }
                double avg = sum / 10; // Calculate average time
                /* Median */
                for (int i = 0; i < 10; i++){
                    for (int j = i + 1; j < 10; j++){
                        if (times[j] < times[i]){
                            double tmp = times[i];
                            times[i] = times[j];
                            times[j] = tmp;
                        }
                    }
                }
                double median = (times[4] + times[5]) / 2; // Calculate median time
                double bw = (double)n * sizeof(int) / avg; // bytes per second
                printf("int,%s,%s,%zu,%g,%g,%g,%g,%g\n", (v == 0 ? "branch" : "branchless"), ord, n, avg, median, min, max, bw);
            }
            /* Measure double branch + branchless */
            for (int v = 0; v < 2; v++){
                double times[10];
                volatile double sink; // Prevent compiler optimization
                /* Warm up */
                sink = (v == 0) ? max_double_branch(ad, n) : max_double_branchless(ad, n);
                /* 10 runs */
                for (int r = 0; r < 10; r++){
                    double t0 = now_sec();
                    double m = (v == 0) ? max_double_branch(ad, n) : max_double_branchless(ad, n);
                    double t1 = now_sec();
                    sink +=m;
                    times[r] = t1 - t0;
                }
                /* stats */
                double min = times[0], max = times[0], sum = 0;
                for (int r = 0; r < 10; r++){
                    sum += times[r];
                    if (times[r] < min) min = times[r]; // Find minimum time
                    if (times[r] > max) max = times[r]; // Find maximum time
                }
                double avg = sum / 10; // Calculate average time
                /* Median */
                for (int i = 0; i < 10; i++){
                    for (int j = i + 1; j < 10; j++){
                        if (times[j] < times[i]){
                            double tmp = times[i];
                            times[i] = times[j];
                            times[j] = tmp;
                        }
                    }
                }
                double median = (times[4] + times[5]) / 2.0; // Calculate median time
                double bw = (double)n * sizeof(double) / avg; // bytes per second
                printf("double,%s,%s,%zu,%g,%g,%g,%g,%g\n", (v == 0 ? "branch" : "branchless"), ord, n, avg, median, min, max, bw);
            }
        }
        free(ai); // Free allocated memory for integer array
        free(ad); // Free allocated memory for double array
    }

    /* Sweep */
    size_t bytes = 2048; // Start with 2048 bytes
    while (bytes <= 256UL * 1024UL * 1024UL){
        size_t n = bytes / sizeof(double); // Calculate number of elements based on bytes
        double *a = malloc(bytes); 
        fill_double_random(a, n); // Fill the array with random doubles

        double times[10];
        volatile double sink = max_double_branchless(a, n);
         for (int r = 0; r < 10; r++){
            double t0 = now_sec();
            double m = max_double_branchless(a, n);
            double t1 = now_sec();
            sink += m; // Prevent compiler optimization
            times[r] = t1 - t0;
        }
        
        double min = times[0], max = times[0], sum = 0;
        for (int r = 0; r < 10; r++){
            sum += times[r];
            if (times[r] < min) min = times[r]; // Find minimum time
            if (times[r] > max) max = times[r]; // Find maximum time
        }
        double avg = sum / 10; // Calculate average time

        for (int i = 0; i < 10; i++){
            for (int j = i + 1; j < 10; j++){
                if (times[j] < times[i]){
                    double tmp = times[i];
                    times[i] = times[j];
                    times[j] = tmp;
                }
            }
        }
        double median = (times[4] + times[5]) / 2.0; // Calculate median time
        double bw = (double)bytes / avg; // bytes per second
        printf("sweep,double,%zu,%g,%g,%g,%g,%g\n", bytes, avg, median, min, max, bw);
        free(a); // Free allocated memory for the array
        bytes *= 2; // Double the size for the next iteration
    }
    return 0; // Exit the program successfully

}

