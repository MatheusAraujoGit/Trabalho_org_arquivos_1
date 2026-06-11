// nesse header:
// funções relacionadas a criar e manipular o binario usando arvore B
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FUNC_BTREE_H
#define FUNC_BTREE_H 

#include "func_reg.h"
#include "auxiliares.h"

//struct pra header da arvore b
typedef struct {
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
} Btree_Header;

//struct pra nó da arvore b
typedef struct {
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    int C1;
    int PR1;
    int C2;
    int PR2;
    int C3;
    int PR3;
    int P1;
    int P2;
    int P3;
    int P4;
} Btree_Node;

//struct pra retorno da pesquisa
//preciso disso pra saber se o que voltou de uma pesquisa é um ponteiro de dados ou um no folha

typedef struct {
    bool is_leaf;
    int pointer;
} searchstruct;

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    void create_btree_index(FILE* BtreeBIN, FILE* dataBIN);

    
    //procurar e imprimir usando indice da arvore B
    void search_in_btree(FILE* BtreeBIN, FILE* dataBIN, Btree_Header head);

    //Lê do teclado n registros e insere no arquivo de dados e na árvore-B
    void insert_with_btree(FILE* BtreeBIN, FILE* dataBIN, regHeader* dataHeader, Btree_Header* BtreeHead, int nRegisters);
    
    void func9();
    
    void func10();
    
    // ----------------------------------------------------------------------------------------------------------------------------------------
    //                                            funções usadas nas implementações das funcionalidades
    // ----------------------------------------------------------------------------------------------------------------------------------------
    
    //vai para o começo do arquivo de indice e o retorna
    Btree_Header Btree_ReadHeader(FILE* BtreeBIN);
    
    //vai para o começo do indice, e sobreescreve o header (a partir de um ponteiro pra header)
    void Btree_WriteHeader(FILE* BtreeBIN, Btree_Header* new_head);
    
    //lê um nó de arvore b na posição atual do arquivo e o retorna
    Btree_Node Btree_ReadNode(FILE* BtreeBIN);
    
    //escreve um nó de arvore b na posição atual do arquivo (a partir de um ponteiro pra nó)
    void Btree_WriteNode(FILE* BtreeBIN, Btree_Node* node);
    
    //seta um arquivo como consistente
    void Btree_setFileConsistent(FILE* BtreeBIN);
    
    //seta um arquivo como inconsistente
    void Btree_setFileInconsistent(FILE* BtreeBIN);
    
    //aux pra pesquisa - volta pra qual ponteiro ir na pesquisa;
    //-2 = match pra chave encontrada nesse nó
    //chave está antes de c1 -> retorna ponteiro 1
    //chave está entre c1 e c2 -> retorna ponteiro 2
    //chave está entre c2 e c3 -> retorna ponteiro 3
    //chave está depois de c3 -> retorna ponteiro 4
    int search_aux(int key, Btree_Node node);
    
    //pesquisa uma chave na arvore b;
    //retorna RRN do arquivo de dados ou RRN da folha vazia de onde a pesquisa cair
    //a struct tem um bool pra dizer qual tipo achou
    searchstruct Btree_Search(FILE* BtreeBIN, int key, int RRN);
    
    //Insere ordernado em um nó que não está cheio e escreve em disco
    void write_into_incomplete_node(FILE* BtreeBIN, int RRN, int key, int offset, int right_child);
    
    //faz o split de acordo com a especificação
    void split_node(FILE* BtreeBIN, Btree_Header* head, int full_RRN, 
        int new_key, int new_offset, int new_right_child,
        int* promo_key, int* promo_offset, int* promo_right_child);
        
    //Funcao auxiliar para insert_in_btree
    //1 = promotion, 0 = no promotion, -1 = chave duplicada
    int insert_recursion(FILE* BtreeBIN, Btree_Header* head, int curr_RRN, //coisas de arquivo
        int key, int offset,        //O que eu quero inserir
        int* promo_key, int* promo_offset, int* promo_right_child);
            
    //Insere registro na arvore B
    void insert_btree(FILE* BtreeBIN, Btree_Header* head, int key, int offset);
 #endif