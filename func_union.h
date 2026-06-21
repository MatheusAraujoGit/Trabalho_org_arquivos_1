// nesse header:
// funções relacionadas a união de registros
//------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef FUNC_UNION_H
#define FUNC_UNION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "func_reg.h"
#include "auxiliares.h"

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------
    
    //uniao com loop aninhado
    void union_nestedLoop(FILE* BIN, FILE* BIN2);
    
    //uniao com loop unico
    void union_singleLoop(FILE* BIN, FILE* BIN2, FILE* BTREE);

    //ordernar arquivo
    void sort_file(FILE* BIN, FILE* newBIN, char* sortField);
    
    //uniao c/ arquivos ordenados
    void union_sort_merge(FILE* BIN, FILE* BIN2);
    
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

    //cria e retorna array de dados na memoria e escreve o numero de registros no endereco sizeOutput
    regData* dataBIN2MemoryData(FILE* BIN, int* sizeOutput);

    //ordena um array de registros de dados na memória
    void sortMemoryData(regData* dataArray, int size, int type);

    //Cria novo arquivo a partir de um array em memória
    void create_DataBIN_from_memory(FILE* BIN, regData* dataArray, int size, regHeader* header);

    //Desaloca um array em memória
    void free_MemoryData(regData* dataArray, int size);

#endif