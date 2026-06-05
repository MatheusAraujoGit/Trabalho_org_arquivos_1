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

// struct para criterios
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

//cria registro de dado
regData createRegister();

//cria registro de critério
regCriteria createCriteriaRegister();

//ve se o registro de dado satisfaz o registro de criterio
bool do_they_match(regCriteria criteria, regData data);

//seta um arquivo como consistente
void regHeader_setFileConsistent(FILE *file);

//seta um arquivo como inconsistente
void regHeader_setFileInconsistent(FILE *file);

//escreve um novo header
void regHeader_write(FILE *outputBIN, regHeader *pRegCab);

//Lê um header
void regHeader_read(FILE *outputBIN, regHeader *pRegCab);

//escreve um novo registro de dado
void regData_write(FILE *outputBIN, regData *pregData);

//imprime um registro de dado
void regData_printData(regData registro);

//recalcula o numero de estaçoes de um binario
void regHeader_recalculateNEstacoes(FILE* BIN);

//deleta um registro de dado em RRN (passar o RRN economiza fseeks)
void regData_DeleteRegistry(FILE* BIN, regHeader* header, int RRN);

//atualiza um registro de dado com base num registro de criterio
void regData_updateReg(FILE* BIN, regCriteria updates, regData old);

//imprime todos os registros de dados de um binario
int regData_printBIN(FILE *BIN);

//cria um registro de criterio, e deleta todos os registros de dados que o satisfazer
void search_and_delete(FILE* BIN, regHeader* header);

//insere um registro de dados, aproveitando memoria se possivel
void insert(FILE* BIN);

//cria um registro de criterio e um de atualizaçao, e atualiza todos os
//registros que satisfazerem o de criterio com o de atualizaçao
void update(FILE* BIN);

//cria um registro de criteiro, e imprime todos os registros que o satisfazerem
int print_with_criteria(FILE* BIN);

//calcular hash de uma string. auxiliar pra deixar o calculo do Nestações mais eficiente
int fnv1a_hash(const char* str);

#endif