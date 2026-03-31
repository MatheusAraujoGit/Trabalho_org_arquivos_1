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


    //TO-DO colocar esses cases como funcoes separadas em um header
    while(option != 0 && error_flag == false){
        scanf("%d", &option);
        switch(option){
            
            case 1: // funcionalidade 1: ler um csv e converter para binário

                //ler e validar nomes de arquivos
                scanf("%s", csv_name);
                scanf("%s", bin_name);
                inputCSV = fopen(csv_name, "r");
                outputBIN = fopen(bin_name, "rb+");

                if(inputCSV == NULL){
                    error_flag = true;
                    break;
                }
                if(outputBIN == NULL){
                    outputBIN = fopen(bin_name, "wb+");
                }

                CSV_createBIN(inputCSV, outputBIN);

                fclose(inputCSV);
                fclose(outputBIN);

                BinarioNaTela(bin_name);

                regHeader_setFileConsistent(bin_name);

            break;

            case 2: // funcionalidade 2: ler um binario e o printar na tela
                    // só consegue ler arquivos consistentes
                
                scanf("%s", bin_name);
                outputBIN = fopen(bin_name, "rb+");

                if(outputBIN == NULL){
                    outputBIN = fopen(bin_name, "wb+");
                }

                fseek(outputBIN, 0, SEEK_SET);

                int status = regData_printBIN(outputBIN);
                if(status == -1){
                    error_flag = true;
                    break;
                }
                if(status == 0) printf("Registro inexistente.\n");

            break;

            case 3: //Teste da funcionalidade 3
                    //Pelo visto esta funcionando
                scanf("%s", bin_name);
                outputBIN = fopen(bin_name, "rb+");

                if(outputBIN == NULL){
                    outputBIN = fopen(bin_name, "wb+");
                }

                regData_printWithInputCriteria(outputBIN);
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