#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 1024

// Struct data perilaku pemakaian aplikasi tiap user
typedef struct {
    int jamAktif;         // 0-23
    int durasiMenit;      // 0-180
    int kategoriFavorit;  // 0-4 (0=Gaming, 1=Musik, 2=Kuliner, 3=Olahraga, 4=Fashion)
} Aktivitas;

// Hitung skor kemiripan aktivitas antara 2 user (maks 10)
int hitungSkor(Aktivitas a, Aktivitas b) {
    int skor = 0;
    if (abs(a.jamAktif - b.jamAktif) <= 2) skor += 4;
    if (abs(a.durasiMenit - b.durasiMenit) <= 20) skor += 3;
    if (a.kategoriFavorit == b.kategoriFavorit) skor += 3;
    return skor;
}

// Baca data aktivitas dari CSV, ambil sebanyak n baris
Aktivitas* bacaDataCSV(const char *filename, int n) {
    Aktivitas *aktivitas = (Aktivitas*) malloc(n * sizeof(Aktivitas));
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Gagal membuka file %s!\n", filename);
        exit(1);
    }

    char buffer[100];
    fgets(buffer, sizeof(buffer), fp); // lewati baris header

    for (int i = 0; i < n; i++) {
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            printf("Data CSV kurang dari N=%d baris!\n", n);
            exit(1);
        }
        sscanf(buffer, "%d,%d,%d",
               &aktivitas[i].jamAktif,
               &aktivitas[i].durasiMenit,
               &aktivitas[i].kategoriFavorit);
    }

    fclose(fp);
    return aktivitas;
}

// Isi matriks M berdasarkan kemiripan aktivitas antar user (dari CSV)
void generateMatrix(int **matrix, int n) {
    Aktivitas *aktivitas = bacaDataCSV("data_aktivitas.csv", n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = hitungSkor(aktivitas[i], aktivitas[j]);
        }
    }

    free(aktivitas);
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

    int **M = allocateMatrix(N);
    int **Result = allocateMatrix(N);

    // Generate matriks M dari data CSV (bukan random lagi)
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