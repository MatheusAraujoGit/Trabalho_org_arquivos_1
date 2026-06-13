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
//preciso disso pra saber se o que voltou de uma pesquisa é um ponteiro de dados ou um no em arvore b
//e tambem pra saber se encontrou ou nao

typedef struct {
    bool is_leaf;
    bool found;
    int pointer;
    int pos_in_node;
} searchstruct;

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    //cria o indice de arvore b e insere todos os registros não-apagados do databin nele
    void create_btree_index(FILE* BtreeBIN, FILE* dataBIN);
    
    //procurar e imprimir usando indice da arvore B
    void search_in_btree(FILE* BtreeBIN, FILE* dataBIN);

    //insere um registro no arquivo de dados e na árvore-B
    void insert_with_btree(FILE* BtreeBIN, FILE* dataBIN);
    
    //deleta um registro no arquivo de dados e na árvore-B
    void delete_with_btree(FILE* BtreeBIN, FILE* dataBIN);
    
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------
    
    //vai para o começo do arquivo de indice, o le e o retorna
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
    int search_aux(int key, Btree_Node node, int* pos_in_node);
    
    //pesquisa uma chave na arvore b;
    //retorna byte offset do arquivo de dados ou byte offset do arquivo de indice do que achou
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

    //Utilidade para remocao de chave em nó intermediario
    //retorna o rrn do nó no qual a chave que eu quero deletar foi parar
    //retorna -1 caso eu não consiga achar sucessor
    //NOTA: Na especificacao eles só falam de trocar pelo sucessor, não sei se é para fazer o predecessor tambem
    int swap_for_immediate_successor(FILE* BtreeBIN, int key, int keyNodeRRN);

    //Redistribuiçao direita
    //Não mexe com o arquivo, apenas com nós para modularizar o código
    //Vou usar o fato de que eu sei que o nó com underflow tem 0 chaves
    void node_right_redistribution(int underflowNodeRRN, Btree_Node* underflowNode,  Btree_Node* father, Btree_Node* rightNode);

    //Redistribuiçao esquerda
    //Não mexe com o arquivo, apenas com nós para modularizar o código
    //Vou usar o fato de que eu sei que o nó com underflow tem 0 chaves
    void node_left_redistribution(int underflowNodeRRN, Btree_Node* underflowNode, Btree_Node* father, Btree_Node* leftNode);

    //Concatenacao esquerda, só tem a lógica de concatenacao entre nós, não mexe no disco
    void node_left_merge(int underflowNodeRRN, Btree_Node* underflowNode, Btree_Node* father, Btree_Node* leftNode);

    //Concatenacao direita, só tem a lógica de concatenacao entre nós, não mexe no disco
    void node_right_merge(int underflowNodeRRN, Btree_Node* underflowNode,  Btree_Node* father, Btree_Node* rightNode);

    //função recursiva pra deleção. retorna se houve underflow
    void Btree_Delete_Recursive();

 #endif