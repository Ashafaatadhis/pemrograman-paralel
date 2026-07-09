CC      = gcc
MPICC   = mpicc
CFLAGS  = -O2 -Wall
OMPFLAGS = -fopenmp

TARGETS = generate_data sequential openmp mpi_version

.PHONY: all clean run-sequential run-openmp run-mpi benchmark

all: $(TARGETS)

generate_data: generate_data.c
	$(CC) $(CFLAGS) -o $@ $<

sequential: sequential.c
	$(CC) $(CFLAGS) -o $@ $<

openmp: openmp.c
	$(CC) $(CFLAGS) $(OMPFLAGS) -o $@ $<

mpi_version: mpi_version.c
	$(MPICC) $(CFLAGS) -o $@ $<

data_aktivitas.csv: generate_data
	./generate_data

run-sequential: sequential data_aktivitas.csv
	./sequential

run-openmp: openmp data_aktivitas.csv
	OMP_NUM_THREADS=4 ./openmp

run-mpi: mpi_version data_aktivitas.csv
	mpirun -np 4 ./mpi_version

benchmark: all data_aktivitas.csv
	bash benchmark.sh

clean:
	rm -f $(TARGETS) data_aktivitas.csv hasil_benchmark.txt
