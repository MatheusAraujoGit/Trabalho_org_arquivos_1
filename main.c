#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h" 

int main() {
    FILE* inputCSV = fopen("estacoes.csv", "r");
    FILE* outputBIN = fopen("arquivoSaida.bin", "rb+");

    //Se outputBIN nao existe eu crio ele
    if(outputBIN == NULL){
        outputBIN = fopen("arquivoSaida.bin", "wb+");
    }

    regCabecalho cab;
    int tamVetorDados = 0;
    regDados** vetorDados;  //esse cara tem todos os registros na memoria, provavelmente só vai precisar dele na primeira funcionalidade, entao vamos poder
                            // desalocar ele antes do fim da execucao eu acho

    CSV_cabecalhoCriar(inputCSV, &cab);
    vetorDados = CSV_criarVetorRegDados(inputCSV, &tamVetorDados);

    //Primeira funcionalidade
    REG_criarBIN(outputBIN, &cab, vetorDados, tamVetorDados);


    
    CSV_apagarVetorRegDados(vetorDados, tamVetorDados);

    fclose(inputCSV);
    fclose(outputBIN);

    return 0;
}