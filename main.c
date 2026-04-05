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
    int n = 0;
    int status = 0;
    bool error_flag = false;



    scanf("%d", &option);
    switch(option){

    case 1: // funcionalidade 1: ler um csv e converter para binário
        scanf("%s", csv_name);
        scanf("%s", bin_name);
        inputCSV = fopen(csv_name, "r");
        outputBIN = fopen(bin_name, "rb+");

        if (inputCSV == NULL)
        {
            error_flag = true;
            break;
        }
        if (outputBIN == NULL)
        {
            outputBIN = fopen(bin_name, "wb+");
        }

        CSV_createBIN(inputCSV, outputBIN);

        fclose(inputCSV);
        fclose(outputBIN);

        BinarioNaTela(bin_name);

        break;

    case 2: // funcionalidade 2: ler um binario e o printar na tela
        scanf("%s", bin_name);
        outputBIN = fopen(bin_name, "rb+");

        if (outputBIN == NULL)
        {
            outputBIN = fopen(bin_name, "wb+");
        }

        fseek(outputBIN, 0, SEEK_SET);

        status = regData_printBIN(outputBIN);
        if (status == -1)
        {
            error_flag = true;
            break;
        }

        fclose(outputBIN);

        break;

    case 3: // funcionalidade 3: pesquisar e imprimir registros
        scanf("%s", bin_name);
        outputBIN = fopen(bin_name, "rb+");

        if (outputBIN == NULL)
        {
            error_flag = true;
            break;
        }

        scanf("%d ", &n);

        for (int i = 0; i < n; i++){
            status = print_with_criteria(outputBIN);
            if (status == -1) break;
        }
        if (status == -1){
            error_flag = true;
            break;
        }

        fclose(outputBIN);

        break;

    case 4: // funcionalidade 4: apagar n registros
        scanf("%s ", bin_name);
        outputBIN = fopen(bin_name, "rb+");

        if (outputBIN == NULL){
            error_flag = true;
            break;
        }

        scanf("%d ", &n);
        for (int i = 0; i < n; i++){
            status = search_and_delete(outputBIN);
            if (status == -1)
                break;
        }
        if (status == -1){
            error_flag = true;
            break;
        }

        BinarioNaTela(bin_name);

        break;

    case 5: // funcionalidade 5: inserir n registros
        scanf("%s ", bin_name);
        outputBIN = fopen(bin_name, "rb+");

        if (outputBIN == NULL)
        {
            error_flag = true;
            break;
        }

        scanf("%d ", &n);

        for (int i = 0; i < n; i++)
        {
            status = insert(outputBIN);
            if (status == -1)
                break;
        }
        if (status == -1)
        {
            error_flag = true;
            break;
        }

        fclose(outputBIN);
        BinarioNaTela(bin_name);
        break;

    case 6:
        scanf("%s ", bin_name);
        outputBIN = fopen(bin_name, "rb+");

        if (outputBIN == NULL)
        {
            error_flag = true;
            break;
        }

        fclose(outputBIN);
        BinarioNaTela(bin_name);
        break;

    default:
        printf("funcionalidade %d não definida!", option);
    }

    if(error_flag) printf("Falha no processamento do arquivo.\n");

    return 0;
}