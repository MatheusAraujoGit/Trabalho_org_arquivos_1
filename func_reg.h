#ifndef FUNC_REG_H
#define FUNC_REG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Deixei os structs expostos para ter menos dor de cabeça

typedef struct {
    char status;    // 0 inconsistente, 1 consistente
    int topo;       // RRN do topo da pilha de removidos
    int proxRRN;    // proximo rrn disponivel
    int nEstacoes;
    int nParesEstacao;
} regCabecalho;

//Dados
typedef struct {
    char removido;
    int proxRRN;    // proximo registro removido para o funcionamento da pilha de removidos
    int codEstacao;         
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;     
    char* nomeEstacao;   
    int tamNomeLinha;       
    char* nomeLinha;     
} regDados;

void REGDados_escrever(FILE* outputBIN, regDados* pRegDados);

void REGCab_escrever(FILE* outputBIN, regCabecalho* pRegCab);

void REG_criarBIN(FILE* outputBIN, regCabecalho* pRegCab, regDados** vetorRegDados, int TamanhoVetor);

void REGDados_printBIN(FILE* BIN);

#endif