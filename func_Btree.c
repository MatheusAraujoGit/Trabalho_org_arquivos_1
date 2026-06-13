#include "func_Btree.h"

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

//cria o indice de arvore b e insere todos os registros não-apagados do databin nele
void create_btree_index(FILE* BtreeBIN, FILE* dataBIN){
    Btree_Header new_head;
    regData temp_reg;

    new_head.status = '0';
    new_head.noRaiz = -1;
    new_head.topo = -1;
    new_head.proxRRN = 0;
    new_head.nroNos = 0;

    temp_reg.nomeEstacao = NULL;
    temp_reg.nomeLinha = NULL;

    int status_leitura;
    int current_pos = 17;
    while((status_leitura = regData_read(dataBIN, &temp_reg)) != -1){
        current_pos += 80;

        // Se o registro for válido, inserir. Se não, ignorar
        if(status_leitura == 1){ 
            insert_btree(BtreeBIN, &new_head, temp_reg.codEstacao, current_pos-80);
            
            //pesadelos pq decidimos que nossas strings sao dinamicas
            if(temp_reg.nomeEstacao != NULL){
                free(temp_reg.nomeEstacao);
                temp_reg.nomeEstacao = NULL;
            }
                if(temp_reg.nomeLinha != NULL){
                    free(temp_reg.nomeLinha);
                    temp_reg.nomeLinha = NULL;
            }
        }
    }

    Btree_WriteHeader(BtreeBIN, &new_head);
}

//procurar e imprimir usando indice da arvore B
void search_in_btree(FILE* BtreeBIN, FILE* dataBIN){

    Btree_Header head = Btree_ReadHeader(BtreeBIN);
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
    }

    //se acho ninguem: falar isso
    if(result.is_leaf) printf("Registro inexistente.\n");
    printf("\n");

    //liberar memoria 
    if (criteria.nomeEstacao != NULL) free(criteria.nomeEstacao);
    if (criteria.nomeLinha != NULL) free(criteria.nomeLinha);
    return;
}

//insere um registro no arquivo de dados e na árvore-B
void insert_with_btree(FILE* BtreeBIN, FILE* dataBIN){
    
    Btree_Header BtreeHead = Btree_ReadHeader(BtreeBIN);
    
    regData tempData = createRegister();

    if(BtreeHead.noRaiz != -1){
        searchstruct a = Btree_Search(BtreeBIN, tempData.codEstacao, BtreeHead.noRaiz);
        if(!a.is_leaf){
            if(tempData.nomeEstacao != NULL) free(tempData.nomeEstacao);
            if(tempData.nomeLinha != NULL) free(tempData.nomeLinha);
            return;
        } //nao pode ter codestaçao duplicado. se achamos um match, abortar execução da funcionalidade
    }
    int offset = insert_no_keyboard(dataBIN, tempData);
    insert_btree(BtreeBIN, &BtreeHead, tempData.codEstacao, offset); 


    if(tempData.nomeEstacao != NULL) free(tempData.nomeEstacao);
    if(tempData.nomeLinha != NULL) free(tempData.nomeLinha);
    
    //atualizar header
    Btree_WriteHeader(BtreeBIN, &BtreeHead);
}

void func10(){}
    
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

//vai para o começo do arquivo de indice, o le e o retorna
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

//vai para o começo do indice, e sobreescreve o header (a partir de um ponteiro pra header)
void Btree_WriteHeader(FILE* BtreeBIN, Btree_Header* new_head){
    fseek(BtreeBIN, 0, SEEK_SET);
    fwrite(&new_head->status, sizeof(char), 1, BtreeBIN);       
    fwrite(&new_head->noRaiz, sizeof(int), 1, BtreeBIN);          
    fwrite(&new_head->topo, sizeof(int), 1, BtreeBIN);       
    fwrite(&new_head->proxRRN, sizeof(int), 1, BtreeBIN);    
    fwrite(&new_head->nroNos, sizeof(int), 1, BtreeBIN);
}

//lê um nó de arvore b na posição atual do arquivo e o retorna
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

//escreve um nó de arvore b na posição atual do arquivo (a partir de um ponteiro pra nó)
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

//seta um arquivo como consistente
void Btree_setFileConsistent(FILE* BtreeBIN){
    fseek(BtreeBIN, 0, SEEK_SET);
    char temp = '1';
    fwrite(&temp, sizeof(char), 1, BtreeBIN);
}

//seta um arquivo como inconsistente
void Btree_setFileInconsistent(FILE* BtreeBIN){
    fseek(BtreeBIN, 0, SEEK_SET);
    char temp = '0';
    fwrite(&temp, sizeof(char), 1, BtreeBIN);
}

//aux pra pesquisa - volta pra qual ponteiro ir na pesquisa;
int search_aux(int key, Btree_Node node){
    if(node.C1 == key || node.C2 == key || node.C3 == key) return -2;
    
    if(key < node.C1) return node.P1;
    
    if(key < node.C2 || node.C2 == -1) return node.P2;
    
    if(key < node.C3 || node.C3 == -1) return node.P3;
    
    return node.P4;
    
}

//pesquisa uma chave na arvore b;
//retorna RRN do arquivo de dados ou RRN da folha vazia de onde a pesquisa cair
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
    
//Insere ordernado em um nó que não está cheio e escreve em disco
void write_into_incomplete_node(FILE* BtreeBIN, int RRN, int key, int offset, int right_child){
    fseek(BtreeBIN, BTree_RRN2BYTE(RRN), SEEK_SET);
    Btree_Node node = Btree_ReadNode(BtreeBIN);
    
    //Inserir ordenado e ajustar os ponteiros
    if(node.nroChaves == 0){
        node.C1 = key;
        node.PR1 = offset;
        node.P2 = right_child;
    }
    else if(node.nroChaves == 1){
        if(key < node.C1){
            node.C2 = node.C1;
            node.PR2 = node.PR1;
            node.P3 = node.P2;
            node.C1 = key;
            node.PR1 = offset;
            node.P2 = right_child;
        }
        else{
            node.C2 = key;
            node.PR2 = offset;
            node.P3 = right_child;
        }
    }
    else if(node.nroChaves == 2){
        if(key < node.C1){
            node.C3 = node.C2;
            node.PR3 = node.PR2;
            node.P4 = node.P3;
            node.C2 = node.C1;
            node.PR2 = node.PR1;
            node.P3 = node.P2;
            node.C1 = key;
            node.PR1 = offset;
            node.P2 = right_child;
        }
        else if(key < node.C2){
            node.C3 = node.C2;
            node.PR3 = node.PR2;
            node.P4 = node.P3;
            node.C2 = key;
            node.PR2 = offset;
            node.P3 = right_child;
        }
        else{
            node.C3 = key;
            node.PR3 = offset;
            node.P4 = right_child;
        }
    }
    node.nroChaves++;
    
    // escrever o nó atualizado de volta para o disco
    fseek(BtreeBIN, BTree_RRN2BYTE(RRN), SEEK_SET);
    Btree_WriteNode(BtreeBIN, &node);
}
    
//faz o split de acordo com a especificação
void split_node(FILE* BtreeBIN, Btree_Header* head, int full_RRN, //rrn do nó cheio
    int new_key, int new_offset, int new_right_child,   // chave e filho direito do cara que não cabe no nó cheio
    int* promo_key, int* promo_offset, int* promo_right_child){ //Essas são as coisas que vão subir por causa do split
        
        fseek(BtreeBIN, BTree_RRN2BYTE(full_RRN), SEEK_SET);
        Btree_Node full_node = Btree_ReadNode(BtreeBIN);
        if(full_node.tipoNo == 0) full_node.tipoNo = 1; // nó raiz vai virar um nó intermediário
        
        //Estrutura auxiliar para eu conseguir ordenar todos os elementos
        typedef struct {
            int key;
            int offset;
            int child_after; //filho direito dessa chave
        } KeyEntry;
        
        KeyEntry entries[4];
        
        entries[0].key = full_node.C1;
        entries[0].offset = full_node.PR1;
        entries[0].child_after = full_node.P2;
        
        entries[1].key = full_node.C2;
        entries[1].offset = full_node.PR2;
        entries[1].child_after = full_node.P3;
        
        entries[2].key = full_node.C3;
        entries[2].offset = full_node.PR3;
        entries[2].child_after = full_node.P4;
        
        entries[3].key = new_key;
        entries[3].offset = new_offset;
        entries[3].child_after = new_right_child;
        
        // bubble para ordenar
        for(int i = 0; i < 4; i++){
            for(int j = i + 1; j < 4; j++){
                if(entries[j].key < entries[i].key){
                    KeyEntry temp = entries[i];
                    entries[i] = entries[j];
                    entries[j] = temp;
                }
            }
        }
        
        //Crio um novo nó e reutilizo espaço da pilha de nós removidos se disponível
        int new_RRN;
        if(head->topo != -1){
            new_RRN = head->topo;
            
            //ler apenas o campo de proximo removido do nó
            fseek(BtreeBIN, BTree_RRN2BYTE(new_RRN) + 1, SEEK_SET);
            int proximo;
            fread(&proximo, sizeof(int), 1, BtreeBIN);
            
            //atualizar topo
            head->topo = proximo;
            head->nroNos++;
        } else {
            //não tem espaço reutilizavel
            new_RRN = head->proxRRN;
            head->proxRRN++;
            head->nroNos++;
        }
        
        //P1 nao vai mudar de lugar ent eu nem preciso mexer nele
        //atualizar nó esquerda (original)
        full_node.C1 = entries[0].key;
        full_node.PR1 = entries[0].offset;
        full_node.P2 = entries[0].child_after;
        
        full_node.C2 = entries[1].key;
        full_node.PR2 = entries[1].offset;
        full_node.P3 = entries[1].child_after;
        
        full_node.C3 = -1;
        full_node.PR3 = -1;
        full_node.P4 = -1;
        full_node.nroChaves = 2;
        
        //atualizo o nó esquerdo em disco
        fseek(BtreeBIN, BTree_RRN2BYTE(full_RRN), SEEK_SET);
        Btree_WriteNode(BtreeBIN, &full_node);
        
        //nó direito
        Btree_Node new_node;
        new_node.removido = '0';
        new_node.proximo = -1;
        new_node.tipoNo = full_node.tipoNo; //mesmo tipo que o nó original
        new_node.nroChaves = 1;
        
        new_node.C1 = entries[3].key;
        new_node.PR1 = entries[3].offset;
        new_node.P1 = entries[2].child_after; //filho que vem após a chave promovida
        new_node.P2 = entries[3].child_after;
        
        new_node.C2 = -1;
        new_node.PR2 = -1;
        new_node.P3 = -1;
        
        new_node.C3 = -1;
        new_node.PR3 = -1;
        new_node.P4 = -1;
        
        //escrevo nó direito no disco
        fseek(BtreeBIN, BTree_RRN2BYTE(new_RRN), SEEK_SET);
        Btree_WriteNode(BtreeBIN, &new_node);
        
        //retorno chave promovida e o ponteiro direito (que é o novo nó que nasceu do split)
        *promo_key = entries[2].key;
        *promo_offset = entries[2].offset;
        *promo_right_child = new_RRN;
}
        
//Funcao auxiliar para insert_in_btree
//1 = promotion, 0 = no promotion, -1 = chave duplicada
int insert_recursion(FILE* BtreeBIN, Btree_Header* head, int curr_RRN, //coisas de arquivo e o rrn do nó em que eu estou
int key, int offset,        //O que eu quero inserir
int* promo_key, int* promo_offset, int* promo_right_child){ //Coisas de promoção (que vão subir na recursão)
    
    int pos = 0;
    Btree_Node node;
    int promo_key_below = -1;
    int promo_offset_below = -1;
    int promo_right_child_below = -1;
    
    if(curr_RRN != -1) { //Estou em um nó qualquer
        fseek(BtreeBIN, BTree_RRN2BYTE(curr_RRN), SEEK_SET);
        node = Btree_ReadNode(BtreeBIN);
        
        pos = search_aux(key, node);
        if(pos == -2) return -1; //Chave duplicada
    }
    else{ // Estou abaixo da arvore, vai ter promoção
        *promo_key = key;
        *promo_offset = offset;
        *promo_right_child = -1;
        return 1;
    }
    
    int return_value = insert_recursion(BtreeBIN, head, pos, key, offset, &promo_key_below, &promo_offset_below, &promo_right_child_below);
    
    if(return_value == 0 || return_value == -1){ //Propagar erro ou no promotion para cima da recursao
        return return_value;
    }
    
    if(node.nroChaves < 3){ //Se tenho espaço no meu nó atual para inserir a chave promovida de baixo, insiro ela
        write_into_incomplete_node(BtreeBIN, curr_RRN, promo_key_below, promo_offset_below, promo_right_child_below);
        return 0;
    }
    
    else{
        //nó está cheio, precisa fazer split
        split_node(BtreeBIN, head, curr_RRN, promo_key_below, promo_offset_below, promo_right_child_below, promo_key, promo_offset, promo_right_child); //o promo_key, promo_offset e promo_right_child vão virar os promo_below da chamada recursiva acima
        return 1; // promovo a chave do meio com o filho direito (novo nó)
    }
    
}

//Insere registro na arvore B
void insert_btree(FILE* BtreeBIN, Btree_Header* head, int key, int offset){
    if(head->noRaiz == -1){
        // árvore vazia -> criar raiz inicial
        head->noRaiz = head->proxRRN;
        head->proxRRN++;
        head->nroNos++;
        
        // Criar nó raiz
        Btree_Node root;
        root.removido = '0';
        root.proximo = -1;
        root.tipoNo = -1; // Raiz que é folha
        root.nroChaves = 1;
        root.C1 = key;
        root.PR1 = offset;
        root.C2 = -1;
        root.PR2 = -1;
        root.C3 = -1;
        root.PR3 = -1;
        root.P1 = -1;
        root.P2 = -1;
        root.P3 = -1;
        root.P4 = -1;
        
        fseek(BtreeBIN, BTree_RRN2BYTE(head->noRaiz), SEEK_SET);
        Btree_WriteNode(BtreeBIN, &root);
    }
    else {
        // Árvore não vazia -> inserir recursivo
        int promo_key = -1;
        int promo_offset = -1;
        int promo_right_child = -1;
        
        int result = insert_recursion(BtreeBIN, head, head->noRaiz, key, offset, 
                                        &promo_key, &promo_offset, &promo_right_child);
        
        // Se houve split da raiz, criar nova raiz
        if(result == 1){
            int old_root_RRN = head->noRaiz;
            
            // Criar nova raiz
            int new_root_RRN;
            if(head->topo != -1){
                //tentar reutilizar espaço
                new_root_RRN = head->topo;
                
                //ler apenas o campo de proximo removido do nó
                fseek(BtreeBIN, BTree_RRN2BYTE(new_root_RRN) + 1, SEEK_SET);
                int proximo;
                fread(&proximo, sizeof(int), 1, BtreeBIN);
                
                //atualizar topo
                head->topo = proximo;
                head->nroNos++;
            } else {
                //não tem espaço reutilizavel
                new_root_RRN = head->proxRRN;
                head->proxRRN++;
                head->nroNos++;
            }
            
            Btree_Node new_root;
            new_root.removido = '0';
            new_root.proximo = -1;
            new_root.tipoNo = 0; // Novo tipo raiz
            new_root.nroChaves = 1;
            new_root.C1 = promo_key;
            new_root.PR1 = promo_offset;
            new_root.C2 = -1;
            new_root.PR2 = -1;
            new_root.C3 = -1;
            new_root.PR3 = -1;
            new_root.P1 = old_root_RRN;
            new_root.P2 = promo_right_child;
            new_root.P3 = -1;
            new_root.P4 = -1;
            
            fseek(BtreeBIN, BTree_RRN2BYTE(new_root_RRN), SEEK_SET);
            Btree_WriteNode(BtreeBIN, &new_root);
            
            head->noRaiz = new_root_RRN;
        }
    }
}

//Utilidade para remocao de chave em nó intermediario
//retorna o rrn do nó no qual a chave que eu quero deletar foi parar
//retorna -1 caso eu não consiga achar sucessor
//NOTA: Na especificacao eles só falam de trocar pelo sucessor, não sei se é para fazer o predecessor tambem
int swap_for_immediate_successor(FILE* BtreeBIN, int key, int keyNodeRRN){
    Btree_Node intermediate;
    Btree_Node leaf;
    int* offsetAddress = NULL;
    int* keyAddress = NULL; // vou usar esses ponteiros para alterar a chave no intermediate

    fseek(BtreeBIN, BTree_RRN2BYTE(keyNodeRRN), SEEK_SET);
    intermediate = Btree_ReadNode(BtreeBIN);

    //Ver onde esta minha chave e por qual caminho seguir
    int pointerRRN = -1;
    if(key == intermediate.C1){
        pointerRRN = intermediate.P2;
        keyAddress = &intermediate.C1;
        offsetAddress = &intermediate.PR1;
    }
    else if(key == intermediate.C2){ 
        pointerRRN = intermediate.P3;
        keyAddress = &intermediate.C2;
        offsetAddress = &intermediate.PR2;
    }
    else if(key == intermediate.C3){
        pointerRRN = intermediate.P4;
        keyAddress = &intermediate.C3;
        offsetAddress = &intermediate.PR3;
    }

    //vou descer na arvore até o nó que tem o sucessor imediato
    if(pointerRRN != -1){
        //primeiro passo
        fseek(BtreeBIN, BTree_RRN2BYTE(pointerRRN), SEEK_SET);
        leaf = Btree_ReadNode(BtreeBIN);

        //loop
        while(leaf.P1 != -1){
            pointerRRN = leaf.P1;
            fseek(BtreeBIN, BTree_RRN2BYTE(leaf.P1), SEEK_SET);
            leaf = Btree_ReadNode(BtreeBIN);
        }

        //Troca
        int tempKey = leaf.C1;
        int tempOffset = leaf.PR1;
        leaf.C1 = key;
        leaf.PR1 = *offsetAddress;
        *keyAddress = tempKey;
        *offsetAddress = tempOffset;

        //Escrita dos nós no disco
        //Atualizo o nó folha
        fseek(BtreeBIN, BTree_RRN2BYTE(pointerRRN), SEEK_SET);
        Btree_WriteNode(BtreeBIN, &leaf);

        //Agora vou atualizar o intermediario
        fseek(BtreeBIN, BTree_RRN2BYTE(keyNodeRRN), SEEK_SET);
        Btree_WriteNode(BtreeBIN, &intermediate);

    }

    return pointerRRN;

}

//Redistribuiçao direita
//Não mexe com o arquivo, apenas com nós para modularizar o código
//Vou usar o fato de que eu sei que o nó com underflow tem 0 chaves
void node_right_redistribution(int underflowNodeRRN, Btree_Node* underflowNode,  Btree_Node* father, Btree_Node* rightNode){
    int* fatherKeyAddress = NULL;
    int* fatherOffsetAddress = NULL;

    //Vou pegar qual chave vai mudar de lugar no nó pai
    if(underflowNodeRRN == father->P3){
        fatherKeyAddress = &father->C3;
        fatherOffsetAddress = &father->PR3;
    }
    else if(underflowNodeRRN == father->P2){
        fatherKeyAddress = &father->C2;
        fatherOffsetAddress = &father->PR2;
    }
    else if(underflowNodeRRN == father->P1){
        fatherKeyAddress = &father->C1;
        fatherOffsetAddress = &father->PR1;
    }

    //Nó com underflow
    underflowNode->C1 = *fatherKeyAddress;
    underflowNode->PR1 = *fatherOffsetAddress;
    underflowNode->P2 = rightNode->P1;
    underflowNode->nroChaves++;

    //Nó pai
    *fatherKeyAddress = rightNode->C1;
    *fatherOffsetAddress = rightNode->PR1;

    //Nó da direita
    rightNode->P1 = rightNode->P2;
    rightNode->C1 = rightNode->C2;
    rightNode->PR1 = rightNode->PR2;
    rightNode->P2 = rightNode->P3;
    rightNode->C2 = rightNode->C3;
    rightNode->PR2 = rightNode->PR3;
    rightNode->P3 = rightNode->P4;
    rightNode->C3 = -1;
    rightNode->PR3 = -1;
    rightNode->P4 = -1;
    rightNode->nroChaves--;

}

//Redistribuiçao esquerda
//Não mexe com o arquivo, apenas com nós para modularizar o código
//Vou usar o fato de que eu sei que o nó com underflow tem 0 chaves
void node_left_redistribution(int underflowNodeRRN, Btree_Node* underflowNode, Btree_Node* father, Btree_Node* leftNode){
    int* fatherKeyAddress = NULL;
    int* fatherOffsetAddress = NULL;

    int* leftKeyAddress = NULL;
    int* leftOffsetAddress = NULL;
    int* leftChildAddress = NULL; //Isso é o filho direito da chave, não confundir pensando que é o filho esquerdo do nó

    //Vou pegar qual chave vai mudar de lugar no nó pai
    if(underflowNodeRRN == father->P4){
        fatherKeyAddress = &father->C3;
        fatherOffsetAddress = &father->PR3;
    }
    else if(underflowNodeRRN == father->P3){
        fatherKeyAddress = &father->C2;
        fatherOffsetAddress = &father->PR2;
    }
    else if(underflowNodeRRN == father->P2){
        fatherKeyAddress = &father->C1;
        fatherOffsetAddress = &father->PR1;
    }

    //Pegar qual chave e filho vou pegar do irmao esquerdo
    if(leftNode->nroChaves == 3){
        leftKeyAddress = &leftNode->C3;
        leftOffsetAddress = &leftNode->PR3;
        leftChildAddress = &leftNode->P4;
    }
    else if(leftNode->nroChaves == 2){
        leftKeyAddress = &leftNode->C2;
        leftOffsetAddress = &leftNode->PR2;
        leftChildAddress = &leftNode->P3;
    }
    else if(leftNode->nroChaves == 1){
        leftKeyAddress = &leftNode->C1;
        leftOffsetAddress = &leftNode->PR1;
        leftChildAddress = &leftNode->P2;
    }

    //Nó com underflow
    underflowNode->P2 = underflowNode->P1;
    underflowNode->C1 = *fatherKeyAddress;
    underflowNode->PR1 = *fatherOffsetAddress;
    underflowNode->P1 = *leftChildAddress;
    underflowNode->nroChaves++;

    //Nó pai
    *fatherKeyAddress = *leftKeyAddress;
    *fatherOffsetAddress = *leftOffsetAddress;

    //Nó da esquerda
    *leftKeyAddress = -1;
    *leftOffsetAddress = -1;
    *leftChildAddress = -1;
    leftNode->nroChaves--;

}

//Concatenacao esquerda, só tem a lógica de concatenacao entre nós, não mexe no disco
void node_left_merge(int underflowNodeRRN, Btree_Node* underflowNode, Btree_Node* father, Btree_Node* leftNode){
    int* fatherKeyAddress = NULL;
    int* fatherOffsetAddress = NULL;

    int* leftKeyAddress = NULL;
    int* leftOffsetAddress = NULL;
    int* leftChildAddress = NULL;

    //Vou pegar qual chave vai mudar de lugar no nó pai
    if(underflowNodeRRN == father->P4){
        fatherKeyAddress = &father->C3;
        fatherOffsetAddress = &father->PR3;
    }
    else if(underflowNodeRRN == father->P3){
        fatherKeyAddress = &father->C2;
        fatherOffsetAddress = &father->PR2;
    }
    else if(underflowNodeRRN == father->P2){
        fatherKeyAddress = &father->C1;
        fatherOffsetAddress = &father->PR1;
    }

    //Pegar onde vou adicionar a chave do pai no filho esquerdo
    if(leftNode->nroChaves == 2){
        leftKeyAddress = &leftNode->C3;
        leftOffsetAddress = &leftNode->PR3;
        leftChildAddress = &leftNode->P4;
    }
    else if(leftNode->nroChaves == 1){
        leftKeyAddress = &leftNode->C2;
        leftOffsetAddress = &leftNode->PR2;
        leftChildAddress = &leftNode->P3;
    }

    //Nó da esquerda
    *leftKeyAddress = *fatherKeyAddress;
    *leftOffsetAddress = *fatherOffsetAddress;
    *leftChildAddress = underflowNode->P1;
    leftNode->nroChaves++;

    //Nó com underflow
    //Não vou mexer com disco aqui, ele precisa ser escrito como removido externamente dessa função
    //e precisa atualizar a pilha de removidos externamente tbm
    underflowNode->removido = '1';

    //Nó pai
    //Vou precisar dar shift para tras nele
    if(fatherKeyAddress == &father->C1){ //significa que tirei o C1 do pai
        father->C1 = father->C2;
        father->PR1 = father->PR2;
        father->P2 = father->P3;

        father->C2 = father->C3;
        father->PR2 = father->PR3;
        father->P3 = father->P4;
    }
    else if(fatherKeyAddress == &father->C2){ //tirei o C2 do pai
        father->C2 = father->C3;
        father->PR2 = father->PR3;
        father->P3 = father->P4;
    }
    //C3 toma shift em todos os casos
    father->C3 = -1;
    father->PR3 = -1;
    father->P4 = -1;
    father->nroChaves--;

}

//Concatenacao direita, só tem a lógica de concatenacao entre nós, não mexe no disco
void node_right_merge(int underflowNodeRRN, Btree_Node* underflowNode,  Btree_Node* father, Btree_Node* rightNode){
    int* fatherKeyAddress = NULL;
    int* fatherOffsetAddress = NULL;

    //Vou pegar qual chave vai mudar de lugar no nó pai
    if(underflowNodeRRN == father->P3){
        fatherKeyAddress = &father->C3;
        fatherOffsetAddress = &father->PR3;
    }
    else if(underflowNodeRRN == father->P2){
        fatherKeyAddress = &father->C2;
        fatherOffsetAddress = &father->PR2;
    }
    else if(underflowNodeRRN == father->P1){
        fatherKeyAddress = &father->C1;
        fatherOffsetAddress = &father->PR1;
    }

    //Nó da direita
    rightNode->removido = '1';

    //Nó com underflow
    //Sempre coloco as chaves no nó mais a esquerda, nesse caso é o nó de underflow
    underflowNode->C1 = *fatherKeyAddress;
    underflowNode->PR1 = *fatherOffsetAddress;
    underflowNode->P2 = rightNode->P1;
    //Para o merge acontecer, o nó vizinho só vai ter uma chave, então só vou me preocupar com ela
    underflowNode->C2 = rightNode->C1;
    underflowNode->PR2 = rightNode->PR1;
    underflowNode->P3 = rightNode->P2;
    underflowNode->nroChaves += 2;

    //Nó pai
    //Vou precisar dar shift para tras nele
    if(fatherKeyAddress == &father->C1){ //significa que tirei o C1 do pai
        father->C1 = father->C2;
        father->PR1 = father->PR2;
        father->P2 = father->P3;

        father->C2 = father->C3;
        father->PR2 = father->PR3;
        father->P3 = father->P4;
    }
    else if(fatherKeyAddress == &father->C2){ //tirei o C2 do pai
        father->C2 = father->C3;
        father->PR2 = father->PR3;
        father->P3 = father->P4;
    }
    //C3 toma shift em todos os casos
    father->C3 = -1;
    father->PR3 = -1;
    father->P4 = -1;
    father->nroChaves--;
}