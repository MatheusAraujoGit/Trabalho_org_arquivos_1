#ifndef FUNC_REG_H
#define FUNC_REG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fornecidas.h"

//struct pra registro de header
typedef struct
{
    char status;
    int topo;   
    int proxRRN; 
    int nEstacoes;
    int nParesEstacao;
} regHeader;

// struct pra registro de dado
typedef struct
{
    char removido;
    int proxRemovido;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char *nomeEstacao;
    int tamNomeLinha;
    char *nomeLinha;
} regData;

// struct para criterios. não tem proxremovido nem removido
typedef struct 
{
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char *nomeEstacao;
    int tamNomeLinha;
    char *nomeLinha;
} regCriteria;

regData createRegister();

regCriteria createCriteriaRegister();

bool do_they_match(regCriteria criteria, regData data);

void regHeader_setFileConsistent(FILE *file);

void regHeader_setFileInconsistent(FILE *file);

void regHeader_write(FILE *outputBIN, regHeader *pRegCab);

void regData_write(FILE *outputBIN, regData *pregData);

void regData_printData(regData registro);

int regData_printBIN(FILE *BIN);

int search_and_delete(FILE* BIN);

void regData_DeleteRegistry(FILE* BIN, int curr_RRN, int head_topo);

int insert(FILE* BIN);

int update(FILE* BIN);

int print_with_criteria(FILE* BIN);

#endif