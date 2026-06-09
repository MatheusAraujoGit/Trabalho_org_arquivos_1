// nesse header:
// funções relacionadas a criação do binário a partir do CSV
//------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifndef FUNC_CSV_H
#define FUNC_CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"


// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    //cria o binário no arquivo outputBIN a partir de um csv de entrada inputCSV
    void createBIN(FILE* inputCSV, FILE* outputBIN);

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

    //a partir de uma string, cria e retorna um registro de dados
    regData CSV_registerPerLine(char *line);

    //parsing
    char *CSV_pointerToField(char *line, int field);

    //parsing
    void CSV_readStringField(char outputVetor[], char *line, int fieldNumber);

    //parsing
    int CSV_readIntField(char *line, int fieldNumber);

#endif