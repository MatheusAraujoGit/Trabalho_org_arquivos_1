// Miya Parus Rios 16910825
// Matheus Araújo Alves 16910609

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h"
#include "func_Btree.h"

int main()
{
    char csv_name[100];
    char bin_name[100];
    char btree_name[100];
    FILE* CSV;
    FILE* BIN;
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
            regHeader_updateNEstacoes(BIN);
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
            regHeader_updateNEstacoes(BIN);
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
            regHeader_updateNEstacoes(BIN);
            regHeader_setFileConsistent(BIN);
            fclose(BIN);
            BinarioNaTela(bin_name);

        break;

        //--------
        case 7: // funcionalidade 7: criar índice árvore-B

            //ler nome dos arquivos
            scanf("%s", bin_name);
            scanf("%s", btree_name);

            //tentar abrir arquivo de dados + errorcheck
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

            //criar arvore b
            BTREE = fopen(btree_name, "wb+"); //wb+ pq eu faço reads no meu insert_recursion
            if (BTREE == NULL){
                error_flag = true;
                fclose(BIN);
                break;
            }
            Btreehead.status = '0';
            Btreehead.noRaiz = -1;
            Btreehead.topo = -1;
            Btreehead.proxRRN = 0;
            Btreehead.nroNos = 0;
            Btree_WriteHeader(BTREE, &Btreehead);

            regData reg;
            reg.nomeEstacao = NULL;
            reg.nomeLinha = NULL;

            //Vou varrendo o bin para criar a arvore b
            int status_leitura;
            while((status_leitura = regData_read(BIN, &reg)) != -1){
                
                // Se o registro for válido
                if(status_leitura == 1){ 
                    insert_in_btree(BTREE, &Btreehead, reg.codEstacao, ftell(BIN) - 80);
                    
                    if(reg.nomeEstacao != NULL){
                        free(reg.nomeEstacao);
                        reg.nomeEstacao = NULL;
                    }
                    if(reg.nomeLinha != NULL){
                        free(reg.nomeLinha);
                        reg.nomeLinha = NULL;
                    }
                }
            }

            //Consistencia
            Btreehead.status = '1';
            fseek(BTREE, 0, SEEK_SET);
            Btree_WriteHeader(BTREE, &Btreehead);

            //fechar arquivos e imprimir
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
                search_in_btree(BTREE, BIN, Btreehead);
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
            Btreehead= Btree_ReadHeader(BTREE);
            if(Btreehead.status == '0'){
                error_flag = true;
                fclose(BIN);
                fclose(BTREE);
                break;
            }

            //setar arquivo como inconsistente
            regHeader_setFileInconsistent(BIN);

            Btreehead.status = '0';
            Btree_WriteHeader(BTREE, &Btreehead);
    
            //ler n
            scanf("%d ", &n);

            //executar funcionalidade n vezes
            for (int i = 0; i < n; i++){
                regData tempData = createRegister();

                //Inserir
                int offset = insert_no_keyboard(BIN, &header, tempData);
                insert_in_btree(BTREE, &Btreehead, tempData.codEstacao, offset);

                if(tempData.nomeEstacao != NULL) free(tempData.nomeEstacao);
                if(tempData.nomeLinha != NULL) free(tempData.nomeLinha);
            }

            //atualizar header, setar arquivo como consistente, fechar e imprimir
            regHeader_write(BIN, &header);
            regHeader_updateNEstacoes(BIN);
            regHeader_setFileConsistent(BIN);
            Btreehead.status = '1';
            Btree_WriteHeader(BTREE, &Btreehead);
            fclose(BIN);
            fclose(BTREE);
            BinarioNaTela(bin_name);
            BinarioNaTela(btree_name);

        break;

        //--------
        case 10:
            printf("funcionalidade %d ainda nao implementada!", option);
        break;

    default:
        printf("funcionalidade %d não definida!", option);
    }

    if(error_flag) printf("Falha no processamento do arquivo.\n");

    return 0;
}