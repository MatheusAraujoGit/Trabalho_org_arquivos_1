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
    void union_singleLoop(FILE* BIN, FILE* BIN2);

    //ordernar arquivo
    void sort_file(FILE* BIN, FILE* newBIN, regHeader header, char* sortField);
    
    //uniao c/ arquivos ordenados
    void union_sort_merge(FILE* BIN, regHeader header, FILE* BIN2, regHeader header2);
    
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

regData* dataBIN2MemoryData(FILE* BIN, int* sizeOutput);

void sortMemoryData(regData* dataArray, int size, int type);

void writeMemoryDataBIN(FILE* BIN, regData* dataArray, int size, regHeader* header);

void freeMemoryData(regData* dataArray, int size);
#endif