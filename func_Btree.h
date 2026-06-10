// nesse header:
// funções relacionadas a criar e manipular o binario usando arvore B
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef FUNC_BTREE_H
#define FUNC_BTREE_H 

//struct pra header da arvore b
typedef struct {
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
} Btree_header;

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
} Btree_node;

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    void func7();

    void func8();

    void func9();

    void func10();

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

    //coisas pras implementaçoes vao aqui


#endif