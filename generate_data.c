#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1024  
typedef struct {
    int jamAktif;
    int durasiMenit;
    int kategoriFavorit;
} Aktivitas;

int main() {
    srand(42);

    FILE *fp = fopen("data_aktivitas.csv", "w");
    if (fp == NULL) {
        printf("Gagal membuka file!\n");
        return 1;
    }

    fprintf(fp, "jamAktif,durasiMenit,kategoriFavorit\n"); // header

    for (int i = 0; i < MAX_N; i++) {
        int jamAktif = rand() % 24;
        int durasiMenit = rand() % 181;
        int kategoriFavorit = rand() % 5;
        fprintf(fp, "%d,%d,%d\n", jamAktif, durasiMenit, kategoriFavorit);
    }

    fclose(fp);
    printf("Data berhasil digenerate ke data_aktivitas.csv (%d baris)\n", MAX_N);
    return 0;
}