#!/bin/bash

SIZES=(256 512 1024)
MPI_PROCS=(1 2 4)
OMP_THREADS=(1 2 4 8) 

OUTPUT_FILE="hasil_benchmark.txt"
echo "=== HASIL BENCHMARK SNAPGRAM ===" > $OUTPUT_FILE
echo "Tanggal: $(date)" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

for N in "${SIZES[@]}"; do
    echo ""
    echo "###################################"
    echo "# Testing N = $N"
    echo "###################################"
    echo "" >> $OUTPUT_FILE
    echo "===== N = $N =====" >> $OUTPUT_FILE

    sed -i "s/#define N [0-9]*/#define N $N/" sequential.c
    sed -i "s/#define N [0-9]*/#define N $N/" openmp.c
    sed -i "s/#define N [0-9]*/#define N $N/" mpi_version.c

    echo "Compiling..."
    gcc sequential.c -o sequential
    gcc -fopenmp openmp.c -o openmp
    mpicc mpi_version.c -o mpi_version

    # --- Sequential ---
    echo ""
    echo ">> Sequential (N=$N)"
    echo "--- Sequential ---" >> $OUTPUT_FILE
    ./sequential | tee -a $OUTPUT_FILE

    # --- OpenMP dengan variasi thread ---
    for T in "${OMP_THREADS[@]}"; do
        echo ""
        echo ">> OpenMP dengan $T thread (N=$N)"
        echo "--- OpenMP ($T thread) ---" >> $OUTPUT_FILE
        OMP_NUM_THREADS=$T ./openmp | tee -a $OUTPUT_FILE
    done

    # --- MPI dengan variasi proses ---
    for P in "${MPI_PROCS[@]}"; do
        echo ""
        echo ">> MPI dengan $P proses (N=$N)"
        echo "--- MPI ($P proses) ---" >> $OUTPUT_FILE
        mpirun -np $P ./mpi_version | tee -a $OUTPUT_FILE
    done

done

echo ""
echo "===================================="
echo "Semua testing selesai!"
echo "Hasil lengkap tersimpan di: $OUTPUT_FILE"
echo "===================================="