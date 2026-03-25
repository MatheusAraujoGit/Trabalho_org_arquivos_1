#ifndef FUNC_CSV_H
#define FUNC_CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"

//Ver comentarios no .c

char* CSV_ponteiroCampo(char* linha, int nCampo);

void CSV_lerStringCampo(char outputVetor[], char* linha, int nCampo);

int CSV_lerIntCampo(char* linha, int nCampo);

void CSV_cabecalhoCriar(FILE* inputCSV, regCabecalho* inputCab);

#endif