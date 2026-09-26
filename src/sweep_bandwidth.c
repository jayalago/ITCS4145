#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

double branchless_max(double *arr, size_t n) {
    double max = arr[0];
    for (size_t i = 1; i < n; i++) {
        double diff = arr[i] - max;
        max += (diff > 0) * diff;   // branchless
    }
    return max;
}

double seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    size_t start_bytes = 2 * 1024;          // 2 KB
    size_t end_bytes   = 256 * 1024 * 1024; // 256 MB

    printf("bytes,n,time,bandwidth_GBps\n");

    for (size_t bytes = start_bytes; bytes <= end_bytes; bytes *= 2) {

        size_t n = bytes / sizeof(double);

        double *arr = malloc(n * sizeof(double));
        if (!arr) {
            fprintf(stderr, "malloc failed at %zu bytes\n", bytes);
            return 1;
        }

        // fill with random doubles
        for (size_t i = 0; i < n; i++) {
            arr[i] = (double)rand() / RAND_MAX;
        }

        double t0 = seconds();
        double max = branchless_max(arr, n);
        double t1 = seconds();

        double time = t1 - t0;
        double bandwidth = (bytes / time) / 1e9;  // GB/s

        printf("%zu,%zu,%f,%f\n", bytes, n, time, bandwidth);

        free(arr);
    }

    return 0;
}
