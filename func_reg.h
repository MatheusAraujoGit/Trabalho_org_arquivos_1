// nesse header:
// funções relacionadas a manipulação dos registros
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FUNC_REG_H
#define FUNC_REG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "auxiliares.h"

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

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    //imprime todos os registros de dados de um binario
    void printBIN(FILE *BIN);

    //cria um registro de criterio, e deleta todos os registros de dados que o satisfazer
    void delete(FILE* BIN);

    //insere um registro de dados, aproveitando memoria se possivel
    void insert(FILE* BIN);

    //cria um registro de criterio e um de atualizaçao, e atualiza todos os
    //registros que satisfazerem o de criterio com o de atualizaçao
    void update(FILE* BIN);

    //cria um registro de criteiro, e imprime todos os registros que o satisfazerem
    void search(FILE* BIN);

    //faz a função de search mas com um regCriteria passado pela função
    void search_no_keyboard(FILE*BIN, regCriteria criteria);

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

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

    //lê um registro de dado
    int regData_read(FILE *BIN, regData *pregData);

    //escreve um novo registro de dado
    void regData_write(FILE *outputBIN, regData *pregData);

    //imprime um registro de dado
    void regData_printData(regData registro);

    //recalcula o numero de estaçoes de um binario
    void regHeader_updateNEstacoes(FILE* BIN);

    //deleta um registro de dado em RRN (passar o RRN economiza fseeks)
    void regData_DeleteRegistry(FILE* BIN, regHeader* header, int RRN);

    //atualiza um registro de dado com base num registro de criterio
    void regData_updateReg(FILE* BIN, regCriteria updates, regData old);

#endif