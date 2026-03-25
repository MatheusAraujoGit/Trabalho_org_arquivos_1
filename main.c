#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h" 

int main() {
    FILE* inputCSV = fopen("estacoes.csv", "r");
    FILE* outputBIN;

    regCabecalho cab;
    CSV_cabecalhoCriar(inputCSV, &cab);
    printf("%d\n", cab.nEstacoes);
    printf("%d\n", cab.nParesEstacao);

    fclose(inputCSV);

    return 0;
}