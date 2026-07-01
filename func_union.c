#include "func_union.h"
#include "func_Btree.h"
#include "func_reg.h"
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

//uniao com loop aninhado
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
    
//uniao com loop unico
void union_singleLoop(FILE* BIN, FILE* BIN2, FILE* BTREE){
    regData tempData1;
    Btree_Header treehead;

    treehead = Btree_ReadHeader(BTREE);

    tempData1.nomeEstacao = NULL;
    tempData1.nomeLinha = NULL;
    bool found_one = false;

    //ir para começo do arquivo
    fseek(BIN, 17, SEEK_SET);

    //para cada registro no binario 1
    while(regData_read(BIN, &tempData1) != -1){
        //se seu codproxestaçao nao for nulo
        if(tempData1.codProxEstacao == -1) continue;
        
        //procurar um match pro codProxEstacao dele no indice de arvB do binario 2
        searchstruct result = Btree_Search(BTREE, tempData1.codProxEstacao, treehead.noRaiz);

        //se achou, ai eu leio o registro de dados do binario 2 pra memoria e printo as coisas desejadas na especificacao
        if(result.found){
            //ler
            fseek(BIN2, result.pointer, SEEK_SET);
            regData found;

            found.nomeEstacao = NULL;
            found.nomeLinha = NULL;

            regData_read(BIN2, &found);

            //printar
            regData_printInt(tempData1.codEstacao);
            regData_printString(tempData1.nomeEstacao);
            regData_printString(tempData1.nomeLinha);
            regData_printInt(tempData1.codProxEstacao);
            regData_printString(found.nomeEstacao);
            printf("\n");
            found_one = true;

            if (found.nomeEstacao != NULL) free(found.nomeEstacao);
            if (found.nomeLinha != NULL) free(found.nomeLinha);
        }
    }

    if(!found_one) printf("Registro inexistente.\n");

    if (tempData1.nomeEstacao != NULL) free(tempData1.nomeEstacao);
    if (tempData1.nomeLinha != NULL) free(tempData1.nomeLinha);
}

//Importa tudo para memoria, ordena de acordo com um dos dois campos possiveis e reescreve em um novo arquivo
void sort_file(FILE* BIN, FILE* newBIN, char* sortField){

    regHeader header;
    regHeader_read(BIN, &header);

    int type = -1;
    if(strcmp("codEstacao", sortField) == 0) type = 0;
    if(strcmp("codProxEstacao", sortField) == 0) type = 1;

    int arraySize = 0;
    regData* dataArray = dataBIN2MemoryData(BIN, &arraySize);
    sortMemoryData(dataArray, arraySize, type);
    create_DataBIN_from_memory(newBIN, dataArray, arraySize, &header);

    free_MemoryData(dataArray, arraySize);
}
    
//func14
//Reescreve os arquivos para ficarem ordenados
//como nesse trabalho os nomeCampos são fixos, vou ja considerar os valores deles como na especificacao
void union_sort_merge(FILE* BIN, FILE* BIN2){
    //Ordeno os dois arquivos, já que eu estou sobreescrevendo os arquivos, eles nao vao diminuir de tamanho e vou precisar
    //funçao de ordenar ja cuida das consistencias
    //usar o proxRRN para saber o fim deles e nao ler lixo
    sort_file(BIN, BIN, "codProxEstacao");
    sort_file(BIN2, BIN2, "codEstacao");
    
    fseek(BIN, 0, SEEK_SET);
    fseek(BIN2, 0, SEEK_SET);
    
    //Leio novos headers
    regHeader newHeader;
    regHeader newHeader2;
    regHeader_read(BIN, &newHeader);
    regHeader_read(BIN2, &newHeader2);

    //Se alguns dos arquivos é vazio não tem o que juntar
    if(newHeader.proxRRN == 0 || newHeader2.proxRRN == 0) return;

    regData regA;
    regData regB;

    //Adoro strings dinâmicas
    regA.nomeEstacao = NULL;
    regA.nomeLinha = NULL;
    regB.nomeEstacao = NULL;
    regB.nomeLinha = NULL;

    //Ler A e B
    regData_read(BIN, &regA);
    regData_read(BIN2, &regB);

    //Como eu sobreescrevi os arquivos, pode ainda haver um resto de registros duplicados inválidos no final já que
    //a sobreescrição não diminui o tamanho dos arquivos, assim eu vou usar o proxRRN que me mostra onde
    //a parte válida acaba e onde o lixo começa

    //Contadores para eu parar no fim da parte válida dos arquivos 
    int firstBINsteps = 0; 
    int secBINsteps = 0;
    while(firstBINsteps < newHeader.proxRRN && secBINsteps < newHeader2.proxRRN){ //Para se chegar no fim dos registros validos do primeiro BIN ou no fim do segundo
        //Se A < B
        if(regA.codProxEstacao < regB.codEstacao){
            regData_read(BIN, &regA);
            firstBINsteps++;
        }
        // A > B
        if(regA.codProxEstacao > regB.codEstacao){
            regData_read(BIN2, &regB);
            secBINsteps++;
        }
        // A = B
        if(regA.codProxEstacao == regB.codEstacao){
                regData_printInt(regA.codEstacao);
                regData_printString(regA.nomeEstacao);
                regData_printString(regA.nomeLinha);
                regData_printInt(regA.codProxEstacao);
                regData_printString(regB.nomeEstacao);
                printf("\n");
                regData_read(BIN2, &regB);
                regData_read(BIN, &regA);
                firstBINsteps++;
                secBINsteps++;
        }
    }

    //Limpeza de memória(adoro strings dinâmicas)
    if(regA.nomeEstacao != NULL) free(regA.nomeEstacao);
    if(regA.nomeLinha != NULL) free(regA.nomeLinha);
    if(regB.nomeEstacao != NULL) free(regB.nomeEstacao);
    if(regB.nomeLinha != NULL) free(regB.nomeLinha);
}

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

//cria e retorna array de dados na memoria e escreve o numero de registros no endereco sizeOutput
//ignora o header
regData* dataBIN2MemoryData(FILE* BIN, int* sizeOutput){

    fseek(BIN, 17, SEEK_SET);

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

//código torturativo em C: (aux pra comparação)
int compare_codEstacao(const void *a, const void *b){
    //Que sintaxe abominável meu deus
    return ((regData *) a)->codEstacao - ((regData *)b)->codEstacao;
}

int compare_codProxEstacao(const void *a, const void *b){
    //Que sintaxe abominável meu deus
    return ((regData *) a)->codProxEstacao - ((regData *)b)->codProxEstacao;
}

//ordena um array de registros de dados na memória
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


//Cria novo arquivo a partir de um array em memória
void create_DataBIN_from_memory(FILE* BIN, regData* dataArray, int size, regHeader* header){
    
    //começar escrevendo como inconsistente, depois no fim voltar e deixar consistente
    header->status = '0';
    
    regHeader_write(BIN, header);

    for(int i = 0; i<size; i++){
        regData_write(BIN, &dataArray[i]);
    }

    //Pilha de removidos vai resetar e proxRRN vai mudar
    //setar consistencia
    header->status = '1';
    header->topo = -1;
    header->proxRRN = size;
    regHeader_write(BIN, header);
}

//Desaloca um array em memória
void free_MemoryData(regData* dataArray, int size){
    for(int i = 0; i<size; i++){
        regData* reg = &dataArray[i];
        if(reg->nomeEstacao != NULL) free(reg->nomeEstacao);
        if(reg->nomeLinha != NULL) free(reg->nomeLinha);
    }
    free(dataArray);
}