#ifndef FUNC_CSV_H
#define FUNC_CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"

//Ver comentarios no .c

void CSV_cabecalhoCriar(FILE* inputCSV, regCabecalho* inputCab);

regDados** CSV_criarVetorRegDados(FILE* inputCSV, int* TamanhoVetor);

void CSV_apagarVetorRegDados(regDados** VetorDados, int TamanhoVetor);

#endif