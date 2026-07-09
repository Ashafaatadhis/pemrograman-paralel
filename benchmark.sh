#!/bin/bash

SIZES=(256 512 1024)
MPI_PROCS=(1 2 4)
OMP_THREADS=(1 2 4 8)

OUTPUT_FILE="hasil_benchmark.txt"
echo "=== HASIL BENCHMARK SNAPGRAM ===" > $OUTPUT_FILE
echo "Tanggal: $(date)" >> $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

# --- Cari nilai N terbesar dari SIZES secara otomatis ---
MAX_N=0
for n in "${SIZES[@]}"; do
    if (( n > MAX_N )); then
        MAX_N=$n
    fi
done

# --- Set MAX_N di generate_data.c pakai sed, lalu generate CSV ---
echo "Generating data_aktivitas.csv dengan $MAX_N baris..."
sed -i "s/#define MAX_N [0-9]*/#define MAX_N $MAX_N/" generate_data.c
gcc generate_data.c -o generate_data
./generate_data

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

    echo ""
    echo ">> Sequential (N=$N)"
    echo "--- Sequential ---" >> $OUTPUT_FILE
    ./sequential | tee -a $OUTPUT_FILE

    for T in "${OMP_THREADS[@]}"; do
        echo ""
        echo ">> OpenMP dengan $T thread (N=$N)"
        echo "--- OpenMP ($T thread) ---" >> $OUTPUT_FILE
        OMP_NUM_THREADS=$T ./openmp | tee -a $OUTPUT_FILE
    done

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