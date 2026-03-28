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
    int QuantidadeRegDados = 0;
    regDados** vetorDados;  //esse cara tem todos os registros na memoria, provavelmente só vai precisar dele na primeira funcionalidade, entao vamos poder
                            //desalocar ele antes do fim da execucao eu acho

    CSV_cabecalhoCriar(inputCSV, &cab);
    vetorDados = CSV_criarVetorRegDados(inputCSV, &QuantidadeRegDados);

    //Primeira funcionalidade
    REG_criarBIN(outputBIN, &cab, vetorDados, QuantidadeRegDados);

    //Não sei se contaria como trapaça eu fazer a segunda funcionalidade pelo vetor alocado na memoria, entao vou fazer pela leitura do arquivo mesmo
    REGDados_printBIN(outputBIN);

    
    CSV_apagarVetorRegDados(vetorDados, QuantidadeRegDados);

    fclose(inputCSV);
    fclose(outputBIN);

    return 0;
}