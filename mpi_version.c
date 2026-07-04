#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1024

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Alokasi matriks sebagai array 1D (lebih gampang buat MPI daripada 2D)
    int *M = (int*) malloc(N * N * sizeof(int));
    int *Result = NULL;

    if (rank == 0) {
        Result = (int*) malloc(N * N * sizeof(int));
        srand(42);
        for (int i = 0; i < N * N; i++) {
            M[i] = rand() % 10;
        }
    }

    double start, end;

    // Broadcast matriks M ke semua proses (semua butuh matriks B = M lengkap)
    if (rank == 0) start = MPI_Wtime();
    MPI_Bcast(M, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Hitung pembagian baris per proses
    int rowsPerProcess = N / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? N : startRow + rowsPerProcess; 
    // proses terakhir handle sisa baris kalau N tidak habis dibagi size

    int localRows = endRow - startRow;
    int *localResult = (int*) malloc(localRows * N * sizeof(int));

    // Tiap proses hitung baris yang jadi tanggung jawabnya
    for (int i = 0; i < localRows; i++) {
        int globalRow = startRow + i;
        for (int j = 0; j < N; j++) {
            long long sum = 0;
            for (int k = 0; k < N; k++) {
                sum += (long long)M[globalRow * N + k] * M[k * N + j];
            }
            localResult[i * N + j] = (int)sum;
        }
    }

    // Kumpulkan semua hasil ke proses 0
    // Karena rowsPerProcess bisa gak rata, pakai MPI_Gatherv
    int *recvCounts = NULL;
    int *displs = NULL;
    if (rank == 0) {
        recvCounts = (int*) malloc(size * sizeof(int));
        displs = (int*) malloc(size * sizeof(int));
        for (int p = 0; p < size; p++) {
            int pStart = p * rowsPerProcess;
            int pEnd = (p == size - 1) ? N : pStart + rowsPerProcess;
            recvCounts[p] = (pEnd - pStart) * N;
            displs[p] = pStart * N;
        }
    }

    MPI_Gatherv(localResult, localRows * N, MPI_INT,
                Result, recvCounts, displs, MPI_INT,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        end = MPI_Wtime();
        double timeMultiply = end - start;

        // Cari max per baris (dilakukan di proses 0 saja, karena sudah cepat)
        double start2 = MPI_Wtime();
        int *maxValues = (int*) malloc(N * sizeof(int));
        int *maxCols = (int*) malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            int maxVal = Result[i * N];
            int maxCol = 0;
            for (int j = 1; j < N; j++) {
                if (Result[i * N + j] > maxVal) {
                    maxVal = Result[i * N + j];
                    maxCol = j;
                }
            }
            maxValues[i] = maxVal;
            maxCols[i] = maxCol;
        }
        double end2 = MPI_Wtime();
        double timeMax = end2 - start2;

        printf("=== SnapGram - Versi MPI ===\n");
        printf("Ukuran matriks: %d x %d\n", N, N);
        printf("Jumlah proses: %d\n\n", size);

        printf("Contoh hasil (5 baris pertama):\n");
        for (int i = 0; i < 5 && i < N; i++) {
            printf("Baris %d -> Max: %d di kolom %d\n", i, maxValues[i], maxCols[i]);
        }

        printf("\n=== Waktu Eksekusi ===\n");
        printf("Perkalian matriks : %.6f detik\n", timeMultiply);
        printf("Cari max per baris: %.6f detik\n", timeMax);
        printf("Total             : %.6f detik\n", timeMultiply + timeMax);

        free(Result);
        free(maxValues);
        free(maxCols);
        free(recvCounts);
        free(displs);
    }

    free(M);
    free(localResult);

    MPI_Finalize();
    return 0;
}