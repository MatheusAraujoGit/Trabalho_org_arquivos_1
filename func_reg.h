#ifndef FUNC_REG_H
#define FUNC_REG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fornecidas.h"

// Deixei os structs expostos para ter menos dor de cabeça

typedef struct
{
    char status; // 0 inconsistente, 1 consistente
    int topo;    // RRN do topo da pilha de removidos
    int proxRRN; // proximo rrn disponivel
    int nEstacoes;
    int nParesEstacao;
} regHeader;

// Dados
// strings de nome sao alocadas dinamicamente na memoria
// e tambem strings nulas NA MEMORIA sao "", pq eu acho que dario erro de null pointer no fread e no fwrite se eu colocasse NULL direto
// mas eu NAO escrevo "" no arquivo, como o tamanho da string sem o \0 é 0 eu escrevo 0 bytes no arquivo
typedef struct
{
    char removido;
    int proxRRN; // proximo registro removido para o funcionamento da pilha de removidos
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

// struct criteria para as funcionalidades 3 para cima
// Valor nulo de string dentro do criteria é "" igual ao valor nulo de string na memoria
typedef struct
{
    char nomeCampo[32];
    int valorInt;
    char valorString[128];
} criteria;

void regHeader_setFileConsistent(char* fileName);

void regData_write(FILE *outputBIN, regData *pregData);

void regHeader_write(FILE *outputBIN, regHeader *pRegCab);

void regData_printData(regData registro);

int regData_printBIN(FILE *BIN);

int regData_searchReg(FILE *BIN, regData *outputReg, int m, criteria tests[]);

void regData_printWithInputCriteria(FILE *BIN);

#endif