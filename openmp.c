#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 1024

void generateMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = rand() % 10;
}

int** allocateMatrix(int n) {
    int **matrix = (int**) malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
        matrix[i] = (int*) malloc(n * sizeof(int));
    return matrix;
}

void freeMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) free(matrix[i]);
    free(matrix);
}

// Perkalian matriks PARALEL pakai OpenMP
void multiplyMatrixParallel(int **A, int **B, int **C, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            long long sum = 0;
            for (int k = 0; k < n; k++) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = (int)sum;
        }
    }
}

void findMaxPerRow(int **C, int n, int *maxValues, int *maxCols) {
    for (int i = 0; i < n; i++) {
        int maxVal = C[i][0];
        int maxCol = 0;
        for (int j = 1; j < n; j++) {
            if (C[i][j] > maxVal) {
                maxVal = C[i][j];
                maxCol = j;
            }
        }
        maxValues[i] = maxVal;
        maxCols[i] = maxCol;
    }
}

int main() {
    printf("=== SnapGram - Versi OpenMP ===\n");
    printf("Ukuran matriks: %d x %d\n", N, N);
    printf("Jumlah thread tersedia: %d\n\n", omp_get_max_threads());

    srand(42); // seed tetap biar hasilnya bisa dibandingkan dengan versi lain

    int **M = allocateMatrix(N);
    int **Result = allocateMatrix(N);

    generateMatrix(M, N);

    double start = omp_get_wtime();
    multiplyMatrixParallel(M, M, Result, N);
    double end = omp_get_wtime();
    double timeMultiply = end - start;

    int *maxValues = (int*) malloc(N * sizeof(int));
    int *maxCols = (int*) malloc(N * sizeof(int));

    double start2 = omp_get_wtime();
    findMaxPerRow(Result, N, maxValues, maxCols);
    double end2 = omp_get_wtime();
    double timeMax = end2 - start2;

    printf("Contoh hasil (5 baris pertama):\n");
    for (int i = 0; i < 5 && i < N; i++) {
        printf("Baris %d -> Max: %d di kolom %d\n", i, maxValues[i], maxCols[i]);
    }

    printf("\n=== Waktu Eksekusi ===\n");
    printf("Perkalian matriks : %.6f detik\n", timeMultiply);
    printf("Cari max per baris: %.6f detik\n", timeMax);
    printf("Total             : %.6f detik\n", timeMultiply + timeMax);

    freeMatrix(M, N);
    freeMatrix(Result, N);
    free(maxValues);
    free(maxCols);

    return 0;
}