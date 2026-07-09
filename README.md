# SnapGram – Rekomendasi Pengguna Berbasis Kemiripan Aktivitas

Proyek ini mensimulasikan sistem rekomendasi teman pada aplikasi SnapGram menggunakan **perkalian matriks kemiripan aktivitas pengguna**. Implementasi disediakan dalam tiga varian: sekuensial, paralel dengan OpenMP, dan paralel dengan MPI.

---

## Deskripsi Masalah

Setiap pengguna memiliki data aktivitas:
| Field | Keterangan |
|---|---|
| `jamAktif` | Jam aktif pengguna (0–23) |
| `durasiMenit` | Durasi penggunaan (0–180 menit) |
| `kategoriFavorit` | Kategori favorit (0=Gaming, 1=Musik, 2=Kuliner, 3=Olahraga, 4=Fashion) |

Skor kemiripan dua pengguna dihitung (maksimal 10 poin):

- +4 poin jika selisih jam aktif ≤ 2
- +3 poin jika selisih durasi ≤ 20 menit
- +3 poin jika kategori favorit sama

Matriks kemiripan **M** (N×N) kemudian dikuadratkan (**M × M**) untuk menemukan rekomendasi pengguna yang paling relevan (nilai maksimum per baris).

---

## Struktur File

```
pemrogramanparalel/
├── generate_data.c      # Generator data CSV
├── sequential.c         # Solusi sekuensial
├── openmp.c             # Solusi paralel OpenMP
├── mpi_version.c        # Solusi paralel MPI
├── Makefile             # Instruksi kompilasi
├── benchmark.sh         # Skrip benchmark otomatis
├── data_aktivitas.csv   # Data input (di-generate)
└── README.md
```

---

## Prasyarat

Pastikan paket berikut sudah terpasang di sistem Linux/WSL:

```bash
# GCC dan OpenMP (biasanya sudah termasuk di gcc)
sudo apt install gcc

# MPI (OpenMPI)
sudo apt install openmpi-bin libopenmpi-dev
```

---

## Kompilasi

### Kompilasi semua sekaligus

```bash
make all
```

### Kompilasi per program

```bash
# Generator data
make generate_data

# Versi sekuensial
make sequential

# Versi paralel OpenMP
make openmp

# Versi paralel MPI
make mpi_version
```

### Kompilasi manual (tanpa Makefile)

```bash
gcc -O2 generate_data.c -o generate_data
gcc -O2 sequential.c -o sequential
gcc -O2 -fopenmp openmp.c -o openmp
mpicc -O2 mpi_version.c -o mpi_version
```

---

## Menjalankan Program

### 1. Generate data input

Langkah pertama, buat file `data_aktivitas.csv`:

```bash
./generate_data
```

Atau otomatis via Make:

```bash
make data_aktivitas.csv
```

### 2. Versi Sekuensial

```bash
./sequential
```

Atau:

```bash
make run-sequential
```

### 3. Versi Paralel OpenMP

```bash
# Jumlah thread default (dari make)
make run-openmp

# Atur jumlah thread secara manual
OMP_NUM_THREADS=2 ./openmp
OMP_NUM_THREADS=4 ./openmp
OMP_NUM_THREADS=8 ./openmp
```

### 4. Versi Paralel MPI

```bash
# Jumlah proses default (4) dari make
make run-mpi

# Atur jumlah proses secara manual
mpirun -np 1 ./mpi_version
mpirun -np 2 ./mpi_version
mpirun -np 4 ./mpi_version
```

---

## Benchmark Lengkap

Jalankan benchmark otomatis untuk semua ukuran matriks (256, 512, 1024) dan berbagai konfigurasi thread/proses:

```bash
make benchmark
```

Atau langsung:

```bash
bash benchmark.sh
```

Hasil tersimpan di `hasil_benchmark.txt`.

---

## Mengubah Ukuran Matriks (N)

Ubah nilai `#define N` di bagian atas masing-masing file `.c`:

```c
#define N 512   // ganti dengan 256, 512, atau 1024
```

Lalu kompilasi ulang:

```bash
make clean
make all
./generate_data   # re-generate data jika N berubah
```

> Catatan: `data_aktivitas.csv` harus memiliki minimal sebanyak N baris data.

---

## Contoh Output

```
=== SnapGram - Versi Sekuensial ===
Ukuran matriks: 1024 x 1024

Contoh hasil (5 baris pertama):
Baris 0 -> Max: 9810 di kolom 42
Baris 1 -> Max: 9700 di kolom 7
Baris 2 -> Max: 9600 di kolom 301
Baris 3 -> Max: 9500 di kolom 88
Baris 4 -> Max: 9400 di kolom 512

=== Waktu Eksekusi ===
Perkalian matriks : 4.231842 detik
Cari max per baris: 0.003201 detik
Total             : 4.235043 detik
```

---

## Membersihkan File Build

```bash
make clean
```

---

## Anggota Tim

| Nama | NIM |
| ---- | --- |
| Adhis Mauliyahsa Ashafaat | 50422079 |
| Irfan Fadhila Aryantoro | 50422720 |
| Muhammad Rafid Kamil | 51422111 |
| Muhammad Rafli Hariandy | 51422703 |
| Muhammad Tio Safrian | 51422163 |
