// Miya Parus Rios 16910825
// Matheus Araújo Alves 16910609

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h"
#include "func_Btree.h"
#include "func_union.h"

int main()
{
    char csv_name[100];
    char bin_name[100];
    char bin2_name[100];
    char trash[100];
    char btree_name[100];
    FILE* CSV;
    FILE* BIN;
    FILE* BIN2;
    FILE* BTREE;
    regHeader header;
    Btree_Header Btreehead;


    int option = -1;
    int n = 0;
    bool error_flag = false;

    scanf("%d", &option);
    switch(option){
        //--------
        case 1: // funcionalidade 1: ler um csv e converter para binário

            //ler nomes de arquivos
            scanf("%s", csv_name);
            scanf("%s", bin_name);

            //tentar abrir arquivos + errorcheck
            CSV = fopen(csv_name, "r");
            BIN = fopen(bin_name, "wb");
            if (CSV == NULL || BIN == NULL){
                error_flag = true;
                break;
            }
            
            //executar a funcionalidade
            createBIN(CSV, BIN);

            //fechar arquivos e imprimir
            fclose(CSV);
            fclose(BIN);
            BinarioNaTela(bin_name);

        break;

        //--------
        case 2: // funcionalidade 2: ler um binario e o printar na tela
            
            //ler nome do arquivo
            scanf("%s", bin_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                break;
            }

            //executar a funcionalidade
            printBIN(BIN);
            
            //fechar arquivo
            fclose(BIN);

        break;

        //-------- 
        case 3: // funcionalidade 3: pesquisar e imprimir n registros

            //ler nome do arquivo
            scanf("%s", bin_name);

            //tentar abrir arquivo + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                break;
            }

            //ler n
            scanf("%d ", &n);

            //executar a funcionalidade n vezes
            for (int i = 0; i < n; i++){
                search(BIN);
            }
            
            //fechar arquivo
            fclose(BIN);

        break;

        //-------- checar essa funcionalidade: pq o search and delete passa um endereço de header como parametro?
        case 4: // funcionalidade 4: apagar n registros

            //ler nome do arquivo
            scanf("%s ", bin_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                break;
            }
            
            //setar arquivo como inconsistente
            regHeader_setFileInconsistent(BIN);
            
            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for (int i = 0; i < n; i++){
                delete(BIN);
            }

            //atualizar header, setar arquivo como consistente, fechar e imprimir
            regHeader_updateNEstacoesEPares(BIN);
            regHeader_setFileConsistent(BIN);
            fclose(BIN);
            BinarioNaTela(bin_name);

        break;

        //--------
        case 5: // funcionalidade 5: inserir n registros

            //ler nome do arquivo
            scanf("%s ", bin_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                break;
            }

            //setar arquivo como inconsistente
            regHeader_setFileInconsistent(BIN);
    
            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for (int i = 0; i < n; i++){
                insert(BIN);
            }

            //atualizar header, setar arquivo como consistente, fechar e imprimir
            regHeader_updateNEstacoesEPares(BIN);
            regHeader_setFileConsistent(BIN);
            fclose(BIN);
            BinarioNaTela(bin_name);

        break;

        //--------
        case 6: //funcionalidade 6: atualizar n registros 

            //ler nome do arquivo
            scanf("%s ", bin_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                break;
            }

            //setar arquivo como inconsistente
            regHeader_setFileInconsistent(BIN);

            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for (int i = 0; i < n; i++){
                update(BIN);
            }

            //atualizar header, setar arquivo como consistente, fechar e imprimir
            regHeader_updateNEstacoesEPares(BIN);
            regHeader_setFileConsistent(BIN);
            fclose(BIN);
            BinarioNaTela(bin_name);

        break;

        //--------
        case 7: // funcionalidade 7: criar índice árvore-B

            //ler nome dos arquivos
            scanf("%s", bin_name);
            scanf("%s", btree_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BTREE = fopen(btree_name, "wb+");
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }

            //setar inconsistente e executar funcionalidade
            Btree_setFileInconsistent(BTREE);
            create_btree_index(BTREE, BIN);

            //setar consistente, fechar arquivos e imprimir
            Btree_setFileConsistent(BTREE);
            fclose(BIN);
            fclose(BTREE);
            BinarioNaTela(btree_name);

        break;

        //--------
        case 8: // funcionalidade 8: ler e imprimir n registros com arvore B
            //ler nome dos arquivos
            scanf("%s", bin_name);
            scanf("%s", btree_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BTREE = fopen(btree_name, "rb");
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            Btreehead= Btree_ReadHeader(BTREE);
            if(Btreehead.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BTREE);
                break;
            }

            //ler n
            scanf("%d ", &n);

            //executar a funcionalidade n vezes
            for (int i = 0; i < n; i++){
                search_in_btree(BTREE, BIN);
            }
            
            //fechar arquivos
            fclose(BIN);
            fclose(BTREE);
        break;

        //--------
        case 9: // funcionalidade 9: inserir n registros com btree
            //ler nome dos arquivos
            scanf("%s", bin_name);
            scanf("%s", btree_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }
            BTREE = fopen(btree_name, "rb+");
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            Btreehead = Btree_ReadHeader(BTREE);
            if(Btreehead.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BTREE);
                break;
            }

            //setar arquivos como inconsistentes
            regHeader_setFileInconsistent(BIN);
            Btree_setFileInconsistent(BTREE);
    
            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for(int i = 0; i<n; i++){
                insert_with_btree(BTREE, BIN);
            }

            //setar arquivos como consistentes e atualizar header
            regHeader_updateNEstacoesEPares(BIN);
            regHeader_setFileConsistent(BIN);
            Btree_setFileConsistent(BTREE);

            //fechar e imprimir
            fclose(BIN);
            fclose(BTREE);

            BinarioNaTela(bin_name);
            BinarioNaTela(btree_name);

        break;

        //--------
        case 10: //funcionalidade 10: deletar registros incluindo btree
            //ler nome dos arquivos
            scanf("%s", bin_name);
            scanf("%s", btree_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }
            BTREE = fopen(btree_name, "rb+");
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            Btreehead = Btree_ReadHeader(BTREE);
            if(Btreehead.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BTREE);
                break;
            }

            //setar arquivos como inconsistentes
            regHeader_setFileInconsistent(BIN);
            Btree_setFileInconsistent(BTREE);
    
            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for(int i = 0; i<n; i++){
                delete_with_btree(BTREE, BIN);
            }

            //setar arquivos como consistentes e atualizar header
            regHeader_updateNEstacoesEPares(BIN);
            regHeader_setFileConsistent(BIN);
            Btree_setFileConsistent(BTREE);

            //fechar e imprimir
            fclose(BIN);
            fclose(BTREE);

            BinarioNaTela(bin_name);
            BinarioNaTela(btree_name);
        break;

        //--------
        case 11: //funcionalidade 11: self-union de codEstacao e codProxEstacao usando nested loop

            //ler nomes dos arquivos (com essa especificaçao, so preciso ler os nomes de arquivos)
            scanf("%s", bin_name);
            scanf("%s", trash);
            scanf("%s", bin2_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BIN2 = fopen(bin2_name, "rb");
            if (BIN2 == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            regHeader_read(BIN2, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                break;
            }

            //executar funcionalidade
            union_nestedLoop(BIN, BIN2);

            //fechar
            fclose(BIN);
            fclose(BIN2);
        break;
         
        //--------
        case 12: //funcionalidade 12: self-union de codEstacao e codProxEstacao usando single loop
            //ler nomes dos arquivos (com essa especificaçao, so preciso ler os nomes de arquivos)

            scanf("%s", bin_name);
            scanf("%s", trash);
            scanf("%s", bin2_name);
            scanf("%s", trash);
            scanf("%s", btree_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BIN2 = fopen(bin2_name, "rb");
            if (BIN2 == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            regHeader_read(BIN2, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                break;
            }

            BTREE = fopen(btree_name, "rb");
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                break;
            }
            Btreehead = Btree_ReadHeader(BTREE);
            if(Btreehead.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                fclose(BTREE);
                break;
            }

            //executar funcionalidade
            union_singleLoop(BIN, BIN2, BTREE);

            //fechar
            fclose(BIN);
            fclose(BIN2);
            fclose(BTREE);
        break;

        //--------
        case 13: //funcionalidade 13: sortar um arquivo por codEstacao ou codProxEst
            char sortField[100];
            scanf("%s", bin_name);
            scanf("%s", sortField);
            scanf("%s", bin2_name);

            //tentar abrir arquivos + errorcheck
            BIN = fopen(bin_name, "rb");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BIN2 = fopen(bin2_name, "wb");
            if (BIN2 == NULL){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                break;
            }

            //executar funcionalidade
            sort_file(BIN, BIN2, sortField);

            //setar consistente
            regHeader_setFileConsistent(BIN2);

            //fechar e imprimir
            fclose(BIN);
            fclose(BIN2);

            BinarioNaTela(bin2_name);
        break;

        //-------- 
        case 14: //funcionalidade 14: unir 2 arquivos sortados
            scanf("%s", bin_name);
            scanf("%s", trash);
            scanf("%s", bin2_name);
            scanf("%s", trash);

            //tentar abrir arquivos + errorcheck
            //uso rb+ pq vou ordenar os arquivos no sort_merge
            BIN = fopen(bin_name, "rb+");
            if (BIN == NULL){
                error_flag = true;
                break;
            }
            regHeader_read(BIN, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                break;
            }

            BIN2 = fopen(bin2_name, "rb+");
            if (BIN2 == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            regHeader_read(BIN2, &header);
            if(header.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BIN2);
                break;
            }

            //executar funcionalidade
            union_sort_merge(BIN, BIN2);
            
            //setar consistente
            regHeader_setFileConsistent(BIN);
            regHeader_setFileConsistent(BIN2);

            //fechar
            fclose(BIN);
            fclose(BIN2);
        break;

    default:
        printf("funcionalidade %d não definida!", option);
    }

    if(error_flag) printf("Falha no processamento do arquivo.\n");

    return 0;
}