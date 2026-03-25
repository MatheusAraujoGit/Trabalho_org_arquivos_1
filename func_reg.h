#ifndef FUNC_REG_H
#define FUNC_REG_H

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
    int proximoRRNRemovido; // proximo registro removido para o funcionamento da pilha de removidos
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

#endif