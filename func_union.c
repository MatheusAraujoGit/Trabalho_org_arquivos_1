#include "func_union.h"
#include "func_reg.h"
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

//func11
void union_nestedLoop(FILE* BIN, FILE* BIN2){
    regData tempData1;
    regData tempData2;

    //ó deus pq q essas strings foram ser dinamicas
    tempData1.nomeEstacao = NULL;
    tempData1.nomeLinha = NULL;
    tempData2.nomeEstacao = NULL;
    tempData2.nomeLinha = NULL;

    //ir para começo do arquivo
    fseek(BIN, 17, SEEK_SET);

    bool found_one = false;

    //para cada registro no binario
    while(regData_read(BIN, &tempData1) != -1){

        //voltar o segundo ponteiro no começo do binario
        fseek(BIN2, 17, SEEK_SET);

        //ler ele todo pra procurar matches
        while(regData_read(BIN2, &tempData2) != -1){

            //se bater, imprimir os campos q a especificaçao pede
            if(tempData1.codProxEstacao == tempData2.codEstacao){
                regData_printInt(tempData1.codEstacao);
                regData_printString(tempData1.nomeEstacao);
                regData_printString(tempData1.nomeLinha);
                regData_printInt(tempData1.codProxEstacao);
                regData_printString(tempData2.nomeEstacao);
                printf("\n");
                found_one = true;
            }

        }

    }

    if(!found_one) printf("Registro inexistente.\n");

    if (tempData1.nomeEstacao != NULL) free(tempData1.nomeEstacao);
    if (tempData1.nomeLinha != NULL) free(tempData1.nomeLinha);
    if (tempData2.nomeEstacao != NULL) free(tempData2.nomeEstacao);
    if (tempData2.nomeLinha != NULL) free(tempData2.nomeLinha);
}
    
//func12
void union_singleLoop(FILE* BIN, FILE* BIN2){
    regData tempData1;
    regData tempData2;

    tempData1.nomeEstacao = NULL;
    tempData1.nomeLinha = NULL;
    tempData2.nomeEstacao = NULL;
    tempData2.nomeLinha = NULL;

    regData_read(BIN, &tempData1);
    regData_read(BIN2, &tempData2);

    printf("todo!\n");
}

//func13
void func13(){

}
    
//func14
void func14(){

}

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------
