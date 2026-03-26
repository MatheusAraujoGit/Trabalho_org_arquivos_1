#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h" 

int main() {
    FILE* inputCSV = fopen("estacoes.csv", "r");
    FILE* outputBIN;

    regCabecalho cab;
    int tamVetorDados = 0;
    regDados** vetorDados;

    CSV_cabecalhoCriar(inputCSV, &cab);
    vetorDados = CSV_criarVetorRegDados(inputCSV, &tamVetorDados);

    printf("%d\n", tamVetorDados);
    printf("%s\n", vetorDados[39]->nomeEstacao);
    printf("%d\n", cab.nEstacoes);
    printf("%d\n", cab.nParesEstacao);
    
    CSV_apagarVetorRegDados(vetorDados, tamVetorDados);

    fclose(inputCSV);

    return 0;
}