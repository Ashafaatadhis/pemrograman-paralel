#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Ukuran matriks (bisa diubah: 1024, 2048, 4096)
#define N 1024

// Fungsi untuk mengisi matriks dengan angka acak
void generateMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10; // angka 0-9 biar hasil kali gak overflow
        }
    }
}

// Alokasi matriks N x N secara dinamis
int** allocateMatrix(int n) {
    int **matrix = (int**) malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (int*) malloc(n * sizeof(int));
    }
    return matrix;
}

// Bebaskan memori matriks
void freeMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Perkalian matriks: C = A x B (sekuensial)
void multiplyMatrix(int **A, int **B, int **C, int n) {
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

// Cari nilai maksimum per baris + posisi kolomnya
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
    printf("=== SnapGram - Versi Sekuensial ===\n");
    printf("Ukuran matriks: %d x %d\n\n", N, N);

    srand(42);

    // Alokasi matriks
    int **M = allocateMatrix(N);
    int **Result = allocateMatrix(N);

    // Generate matriks M dengan angka acak
    generateMatrix(M, N);

    // Ukur waktu perkalian matriks
    clock_t start = clock();
    multiplyMatrix(M, M, Result, N);
    clock_t end = clock();

    double timeMultiply = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Cari nilai max per baris
    int *maxValues = (int*) malloc(N * sizeof(int));
    int *maxCols = (int*) malloc(N * sizeof(int));

    clock_t start2 = clock();
    findMaxPerRow(Result, N, maxValues, maxCols);
    clock_t end2 = clock();

    double timeMax = ((double)(end2 - start2)) / CLOCKS_PER_SEC;

    // Tampilkan beberapa hasil (jangan print semua kalau N besar)
    printf("Contoh hasil (5 baris pertama):\n");
    for (int i = 0; i < 5 && i < N; i++) {
        printf("Baris %d -> Max: %d di kolom %d\n", i, maxValues[i], maxCols[i]);
    }

    printf("\n=== Waktu Eksekusi ===\n");
    printf("Perkalian matriks : %.6f detik\n", timeMultiply);
    printf("Cari max per baris: %.6f detik\n", timeMax);
    printf("Total             : %.6f detik\n", timeMultiply + timeMax);

    // Bebaskan memori
    freeMatrix(M, N);
    freeMatrix(Result, N);
    free(maxValues);
    free(maxCols);

    return 0;
}