#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_reg.h"
#include "func_csv.h"

int main()
{
    FILE *inputCSV = fopen("estacoes.csv", "r");
    FILE *outputBIN = fopen("arquivoSaida.bin", "rb+");

    // Se outputBIN nao existe eu crio ele
    if (outputBIN == NULL)
    {
        outputBIN = fopen("arquivoSaida.bin", "wb+");
    }

    regCabecalho cab;
    int QuantidadeRegDados = 0;
    regDados **vetorDados; // esse cara tem todos os registros na memoria, provavelmente só vai precisar dele na primeira funcionalidade, entao vamos poder
                           // desalocar ele antes do fim da execucao eu acho

    CSV_cabecalhoCriar(inputCSV, &cab);
    vetorDados = CSV_criarVetorRegDados(inputCSV, &QuantidadeRegDados);

    // Primeira funcionalidade
    REG_criarBIN(outputBIN, &cab, vetorDados, QuantidadeRegDados);
    CSV_apagarVetorRegDados(vetorDados, QuantidadeRegDados);

    // Segunda funcionalidade
    fseek(outputBIN, 0, SEEK_SET);
    REGDados_printBIN(outputBIN);

    // testes para ver se a busca esta funcionando
    criterio testes[2];

    strcpy(testes[0].nomeCampo, "codProxEstacao");
    testes[0].valorInt = -1;

    strcpy(testes[1].nomeCampo, "nomeEstacao");
    strcpy(testes[1].valorString, "Vila Prudente");

    regDados resultadoBusca;
    resultadoBusca.nomeEstacao = NULL;
    resultadoBusca.nomeLinha = NULL;

    fseek(outputBIN, sizeof(char) + sizeof(int) * 4, SEEK_SET); // Pular cabecalho
    int posicao = REGDados_buscaReg(outputBIN, &resultadoBusca, 2, testes);

    if (posicao == -1)
    {
        printf("Não achou\n");
    }
    else
    {
        printf("byte offset %d\n", posicao);
        REGDados_printDados(resultadoBusca);
    }

    free(resultadoBusca.nomeEstacao);
    free(resultadoBusca.nomeLinha);

    fclose(inputCSV);
    fclose(outputBIN);

    return 0;
}