#ifndef FUNC_CSV_H
#define FUNC_CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"

// Ver comentarios no .c

void CSV_cabecalhoCriar(FILE *inputCSV, regHeader *inputCab);

regData **CSV_criarVetorregData(FILE *inputCSV, int *TamanhoVetor);

void CSV_apagarVetorregData(regData **VetorDados, int TamanhoVetor);

#endif