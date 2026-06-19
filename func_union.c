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
//Importa tudo para memoria, ordena de acordo com um dos dois campos possiveis e reescreve em um novo arquivo
void sort_file(FILE* BIN, FILE* newBIN, regHeader header, char* sortField){
    int type = -1;
    if(strcmp("codEstacao", sortField) == 0) type = 0;
    if(strcmp("codProxEstacao", sortField) == 0) type = 1;

    int arraySize = 0;
    regData* dataArray = dataBIN2MemoryData(BIN, &arraySize);
    sortMemoryData(dataArray, arraySize, type);
    writeMemoryDataBIN(newBIN, dataArray, arraySize, &header);
    freeMemoryData(dataArray, arraySize);
}
    
//func14
void func14(){

}

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

//Supõe que a posição do arquivo ja passou do header
//retorna array de dados na memoria e escreve o numero de registros no endereco sizeOutput
regData* dataBIN2MemoryData(FILE* BIN, int* sizeOutput){
    //No aulão falaram para importar o arquivo na memória e depois ordenar

    int nRegisters = 0; //vou usar como index para o array tambem
    int capacity = 50;
    regData* dataArray = (regData*) malloc(capacity * sizeof(regData)); //vetor dinamico com 50 elementos no começo

    int status_leitura;
    regData readReg;

    readReg.nomeEstacao = NULL;
    readReg.nomeLinha = NULL;
    
    while((status_leitura = regData_read(BIN, &readReg)) != -1){

        //realoco memoria caso esteja no limite
        if(nRegisters >= capacity){
            capacity += 50;
            dataArray = (regData*) realloc(dataArray, capacity * sizeof(regData));
        }

        if(status_leitura == 1){
            dataArray[nRegisters] = readReg;
            nRegisters++;
        }

        readReg.nomeEstacao = NULL;
        readReg.nomeLinha = NULL;
    }

    if(readReg.nomeEstacao != NULL) free(readReg.nomeEstacao);
    if(readReg.nomeLinha != NULL) free(readReg.nomeLinha);
    
    *sizeOutput = nRegisters;
    return dataArray;
}

int compare_codEstacao(const void *a, const void *b){
    //Que sintaxe abominável meu deus
    return ((regData *) a)->codEstacao - ((regData *)b)->codEstacao;
}

int compare_codProxEstacao(const void *a, const void *b){
    //Que sintaxe abominável meu deus
    return ((regData *) a)->codProxEstacao - ((regData *)b)->codProxEstacao;
}

//ordena os registros de dados na memória
//type = 0 ordena por codEstacao e type = 1 ordena por codProxEstacao
void sortMemoryData(regData* dataArray, int size, int type){
    //Usei a funcao qsort do C que comentaram no aulão
    switch (type){
        case 0:
            qsort(dataArray, size, sizeof(regData), compare_codEstacao);
            break;
        case 1:
            qsort(dataArray, size, sizeof(regData), compare_codProxEstacao);
            break;
        default:
            break;
    }
}


//Cria novo arquivo ordenado, NÃO cuida da desalocação do array
void writeMemoryDataBIN(FILE* BIN, regData* dataArray, int size, regHeader* header){
    //Pilha de removidos vai resetar e proxRRN vai mudar
    header->topo = -1;
    header->proxRRN = size;
    regHeader_write(BIN, header);

    for(int i = 0; i<size; i++){
        regData_write(BIN, &dataArray[i]);
    }
}

//Desaloca o array
void freeMemoryData(regData* dataArray, int size){
    for(int i = 0; i<size; i++){
        regData* reg = &dataArray[i];
        if(reg->nomeEstacao != NULL) free(reg->nomeEstacao);
        if(reg->nomeLinha != NULL) free(reg->nomeLinha);
    }
    free(dataArray);
}