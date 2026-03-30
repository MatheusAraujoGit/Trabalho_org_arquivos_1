#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h"
#include "fornecidas.h"

int main()
{
    char csv_name[100];
    char bin_name[100];
    FILE* inputCSV;
    FILE* outputBIN;
    int option = -1;
    bool error_flag = false;

    regHeader head;
    int dataCount= 0;
    regData **dataVector;


    while(option != 0 && error_flag == false){
        scanf("%d ", &option);
        switch(option){
            
            case 1: // funcionalidade 1: ler um csv e converter para binário

                //ler e validar nomes de arquivos
                scanf("%s ", csv_name);
                scanf("%s ", bin_name);
                inputCSV = fopen(csv_name, "r");
                outputBIN = fopen(bin_name, "rb+");

                if(inputCSV == NULL){
                    error_flag = true;
                    break;
                }
                if(outputBIN == NULL){
                    outputBIN = fopen(bin_name, "wb+");
                }

                //TO-DO: não devemos armazenar todos os dados num vetor na memória;
                //devemos ler os registros do csv um por um na memória em quanto os escrevemos no arquivo, de modo a so ter um na memória por vez

                CSV_cabecalhoCriar(inputCSV, &head);
                dataVector = CSV_criarVetorregData(inputCSV, &dataCount);
                REG_criarBIN(outputBIN, &head, dataVector, dataCount);
                CSV_apagarVetorregData(dataVector, dataCount);

                fclose(inputCSV);
                fclose(outputBIN);

                //TO-DO: binario na tela não está imprimindo o valor esperado; estamos guardando errado?
                BinarioNaTela(bin_name);

            break;

            case 2: // funcionalidade 2: ler um binario e o printar na tela
                
                //TO-DO: função dá segfault - estou usando ela certo?
                fseek(outputBIN, 0, SEEK_SET);
                int status = regData_printBIN(outputBIN);
                if(status == -1){
                    error_flag = true;
                    break;
                }
                if(status == 0) printf("Registro inexistente.\n");

            break;

            case 3:
            printf("funcão ainda não implementada! \n");
            break;

            case 4:
            printf("funcão ainda não implementada! \n");
            break;

            case 5:
            printf("funcão ainda não implementada! \n");
            break;

            case 6:
            printf("funcão ainda não implementada! \n");
            break;

            default: printf("funcionalidade %d não definida!", option);
        }
    }
    if(error_flag) printf("Falha no processamento de arquivo.\n");

    return 0;
}