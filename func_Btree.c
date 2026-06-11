#include "func_Btree.h"

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

    void func7(){}

    void search_in_btree(FILE* BtreeBIN, FILE* dataBIN, Btree_Header head){
        regCriteria criteria;        
        criteria = createCriteriaRegister();


        //se nao tiver codEstaçao na pesquisa, fallback pra funcionalidade 3
        if(criteria.codEstacao == -2){
            search_no_keyboard(dataBIN, criteria);
            return;
        }

        //se tem codEstaçao na pesquisa; ou nao acho ninguem (folha vazia), ou acho 1 so
        searchstruct result = Btree_Search(BtreeBIN, criteria.codEstacao, head.noRaiz);

        //se acho 1: o imprimir
        if(!result.is_leaf){
            fseek(dataBIN, result.pointer, SEEK_SET);
            regData found;

            found.nomeEstacao = NULL;
            found.nomeLinha = NULL;

            regData_read(dataBIN, &found);
            regData_printData(found);

            if (found.nomeEstacao != NULL) free(criteria.nomeEstacao);
            if (found.nomeLinha != NULL) free(criteria.nomeLinha);
        }//verificar: precisamos disso tudo msm pra imprimir um registro?
        //sera q tem como melhorar nessa parte de ter que manualmente colocar como null e dps dar free?

        //se acho ninguem: falar isso
        if(result.is_leaf) printf("Registro inexistente.\n");
        printf("\n");

        //liberar memoria 
        if (criteria.nomeEstacao != NULL) free(criteria.nomeEstacao);
        if (criteria.nomeLinha != NULL) free(criteria.nomeLinha);
        return;
    }

    void func9(){}

    void func10(){}

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

Btree_Header Btree_ReadHeader(FILE* BtreeBIN){
    Btree_Header head;

    fseek(BtreeBIN, 0, SEEK_SET);
    fread(&head.status, sizeof(char), 1, BtreeBIN);
    fread(&head.noRaiz, sizeof(int), 1, BtreeBIN);
    fread(&head.topo, sizeof(int), 1, BtreeBIN);
    fread(&head.proxRRN, sizeof(int), 1, BtreeBIN);
    fread(&head.nroNos, sizeof(int), 1, BtreeBIN);

    return head;
}

void Btree_WriteHeader(FILE* BtreeBIN, Btree_Header* new_head){
    fseek(BtreeBIN, 0, SEEK_SET);
    fwrite(&new_head->status, sizeof(char), 1, BtreeBIN);       
    fwrite(&new_head->noRaiz, sizeof(int), 1, BtreeBIN);          
    fwrite(&new_head->topo, sizeof(int), 1, BtreeBIN);       
    fwrite(&new_head->proxRRN, sizeof(int), 1, BtreeBIN);    
    fwrite(&new_head->nroNos, sizeof(int), 1, BtreeBIN);
}

Btree_Node Btree_ReadNode(FILE* BtreeBIN){
    Btree_Node node;

    fread(&node.removido, sizeof(char), 1, BtreeBIN);
    fread(&node.proximo, sizeof(int), 1, BtreeBIN);
    fread(&node.tipoNo, sizeof(int), 1, BtreeBIN);
    fread(&node.nroChaves, sizeof(int), 1, BtreeBIN);
    fread(&node.C1, sizeof(int), 1, BtreeBIN);
    fread(&node.PR1, sizeof(int), 1, BtreeBIN);
    fread(&node.C2, sizeof(int), 1, BtreeBIN);
    fread(&node.PR2, sizeof(int), 1, BtreeBIN);
    fread(&node.C3, sizeof(int), 1, BtreeBIN);
    fread(&node.PR3, sizeof(int), 1, BtreeBIN);
    fread(&node.P1, sizeof(int), 1, BtreeBIN);
    fread(&node.P2, sizeof(int), 1, BtreeBIN);
    fread(&node.P3, sizeof(int), 1, BtreeBIN);
    fread(&node.P4, sizeof(int), 1, BtreeBIN);

    return node;
}

void Btree_WriteNode(FILE* BtreeBIN, Btree_Node* node){
    fwrite(&node->removido, sizeof(char), 1, BtreeBIN);
    fwrite(&node->proximo, sizeof(int), 1, BtreeBIN);
    fwrite(&node->tipoNo, sizeof(int), 1, BtreeBIN);
    fwrite(&node->nroChaves, sizeof(int), 1, BtreeBIN);
    fwrite(&node->C1, sizeof(int), 1, BtreeBIN);
    fwrite(&node->PR1, sizeof(int), 1, BtreeBIN);
    fwrite(&node->C2, sizeof(int), 1, BtreeBIN);
    fwrite(&node->PR2, sizeof(int), 1, BtreeBIN);
    fwrite(&node->C3, sizeof(int), 1, BtreeBIN);
    fwrite(&node->PR3, sizeof(int), 1, BtreeBIN);
    fwrite(&node->P1, sizeof(int), 1, BtreeBIN);
    fwrite(&node->P2, sizeof(int), 1, BtreeBIN);
    fwrite(&node->P3, sizeof(int), 1, BtreeBIN);
    fwrite(&node->P4, sizeof(int), 1, BtreeBIN);
 }

int search_aux(int key, Btree_Node node){
    if(node.C1 == key || node.C2 == key || node.C3 == key) return -2;
  
    if(key < node.C1) return node.P1;
    
    if(key < node.C2 || node.C2 == -1) return node.P2;
 
    if(key < node.C3 || node.C3 == -1) return node.P3;
    
    return node.P4;
    
}

searchstruct Btree_Search(FILE* BtreeBIN, int key, int RRN){
    fseek(BtreeBIN, BTree_RRN2BYTE(RRN), SEEK_SET);
    Btree_Node node = Btree_ReadNode(BtreeBIN);

    int next = search_aux(key, node);

    //se deu match
    if(next == -2){
        searchstruct result;
        result.is_leaf = false;

        if(key == node.C1) result.pointer = node.PR1;
        else if(key == node.C2) result.pointer = node.PR2;
        else if(key == node.C3) result.pointer = node.PR3;
        return result;
    }
    //se encontrou uma folha vazia
    if(next == -1){
        searchstruct result;
        result.is_leaf = true;

        result.pointer = RRN;
        return result;
    }

    //recursão
    return Btree_Search(BtreeBIN, key, next);

}