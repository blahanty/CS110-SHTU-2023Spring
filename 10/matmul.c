#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MATRIX_SIZE 1024

void matmul_naive(double *a, double *b, double *c) {
    for (int i = 0; i < MATRIX_SIZE; i++)
        for (int j = 0; j < MATRIX_SIZE; j++)
            for (int k = 0; k < MATRIX_SIZE; k++)
                c[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
}

void matmul_optimized_slices(double *a, double *b, double *c) {
#pragma omp parallel
    {
        for (int i = omp_get_thread_num(); i < MATRIX_SIZE; i += omp_get_num_threads())
            for (int j = 0; j < MATRIX_SIZE; ++j)
                for (int k = 0; k < MATRIX_SIZE; ++k)
                    c[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
    }
}

void matmul_optimized_chunks(double *a, double *b, double *c) {
    int p = MATRIX_SIZE / omp_get_num_threads();
#pragma omp parallel
    {
        int start = p * omp_get_thread_num(), end = MATRIX_SIZE;
        if (start + p < end) { end = start + p; }
        for (int i = start; i < end; ++i)
            for (int j = 0; j < MATRIX_SIZE; ++j)
                for (int k = 0; k < MATRIX_SIZE; ++k)
                    c[i * MATRIX_SIZE + j] += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
    }
}

int main() {
    double *a = (double *) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    double *b = (double *) malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    double *c = (double *) calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));

    if (!a || !b || !c) {
        printf("Error: could not allocate memory.\n");
        return 1;
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            a[i * MATRIX_SIZE + j] = (double) rand() / RAND_MAX;
            b[i * MATRIX_SIZE + j] = (double) rand() / RAND_MAX;
        }
    }

    double start_time = omp_get_wtime();
    matmul_naive(a, b, c);
    double end_time = omp_get_wtime();
    printf("Naive: %.5f seconds\n", end_time - start_time);

    start_time = omp_get_wtime();
    matmul_optimized_slices(a, b, c);
    end_time = omp_get_wtime();
    printf("Optimized (slices): %.5f seconds\n", end_time - start_time);

    start_time = omp_get_wtime();
    matmul_optimized_chunks(a, b, c);
    end_time = omp_get_wtime();
    printf("Optimized (chunks): %.5f seconds\n", end_time - start_time);

    free(a);
    free(b);
    free(c);

    return 0;
}
