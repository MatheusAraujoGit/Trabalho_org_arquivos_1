#include "func_reg.h"

//só abre o arquivo e coloca ele como consistente
void regHeader_setFileConsistent(char* fileName){
    FILE* file = fopen(fileName, "rb+");
    fseek(file, 0, SEEK_SET);
    const char consistent = '1';
    fwrite(&consistent, sizeof(char), 1, file);
    fclose(file);
}

// Eu uso fseek aqui pq de qualquer forma o cabecalho vai estar no comeco no arquivo
void regHeader_write(FILE *outputBIN, regHeader *pRegHeader){
    fseek(outputBIN, 0, SEEK_SET);
    fwrite(&(pRegHeader->status), sizeof(char), 1, outputBIN);       // off 0
    fwrite(&(pRegHeader->topo), sizeof(int), 1, outputBIN);          // off 1
    fwrite(&(pRegHeader->proxRRN), sizeof(int), 1, outputBIN);       // off 5
    fwrite(&(pRegHeader->nEstacoes), sizeof(int), 1, outputBIN);     // off 9
    fwrite(&(pRegHeader->nParesEstacao), sizeof(int), 1, outputBIN); // off 13
}

void regData_write(FILE *outputBIN, regData *pregData){
    fwrite(&(pregData->removido), sizeof(char), 1, outputBIN);                         // off 0
    fwrite(&(pregData->proxRRN), sizeof(int), 1, outputBIN);                           // off 1
    fwrite(&(pregData->codEstacao), sizeof(int), 1, outputBIN);                        // off 5
    fwrite(&(pregData->codLinha), sizeof(int), 1, outputBIN);                          // off 9
    fwrite(&(pregData->codProxEstacao), sizeof(int), 1, outputBIN);                    // off 13
    fwrite(&(pregData->distProxEstacao), sizeof(int), 1, outputBIN);                   // off 17
    fwrite(&(pregData->codLinhaIntegra), sizeof(int), 1, outputBIN);                   // off 21
    fwrite(&(pregData->codEstIntegra), sizeof(int), 1, outputBIN);                     // off 25
    fwrite(&(pregData->tamNomeEstacao), sizeof(int), 1, outputBIN);                    // off 29
    fwrite(pregData->nomeEstacao, sizeof(char), pregData->tamNomeEstacao, outputBIN); // varzea daqui para baixo
    fwrite(&(pregData->tamNomeLinha), sizeof(int), 1, outputBIN);
    fwrite(pregData->nomeLinha, sizeof(char), pregData->tamNomeLinha, outputBIN);

    // Preencher com lixo
    const char trashChar = '$';
    int total_size = 0;
    total_size = sizeof(int) * 9 + sizeof(char) + pregData->tamNomeLinha + pregData->tamNomeEstacao;
    for (int i = 0; i < 80 - total_size; i++)
    {
        fwrite(&trashChar, sizeof(char), 1, outputBIN);
    }
}

void REGCab_ler(FILE *BIN, regHeader *pRegHeader){
    fseek(BIN, 0, SEEK_SET);
    fread(&(pRegHeader->status), sizeof(char), 1, BIN);
    fread(&(pRegHeader->topo), sizeof(int), 1, BIN);
    fread(&(pRegHeader->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegHeader->nEstacoes), sizeof(int), 1, BIN);
    fread(&(pRegHeader->nParesEstacao), sizeof(int), 1, BIN);
}

// Retorna -1 se chegou no fim do arquivo
// Retorna 0 se tentou ler registro removido
// retorna 1 se deu certo
int regData_read(FILE *BIN, regData *pregData){

    // tratar casos de fim de arquivo e registro removido
    char verificarFimDoArquivo = fread(&(pregData->removido), sizeof(char), 1, BIN);
    if (verificarFimDoArquivo != 1)
        return -1;
    if (pregData->removido == 1){
        fseek(BIN, 79, SEEK_CUR); // pulo os 79 bytes restantes do registro removido
        return 0;
    }

    fread(&(pregData->proxRRN), sizeof(int), 1, BIN);
    fread(&(pregData->codEstacao), sizeof(int), 1, BIN);
    fread(&(pregData->codLinha), sizeof(int), 1, BIN);
    fread(&(pregData->codProxEstacao), sizeof(int), 1, BIN);
    fread(&(pregData->distProxEstacao), sizeof(int), 1, BIN);
    fread(&(pregData->codLinhaIntegra), sizeof(int), 1, BIN);
    fread(&(pregData->codEstIntegra), sizeof(int), 1, BIN);

    char bufferNome[128];

    // Nome da estacao
    // Ficou um pouco horroroso pq eu preciso previnir memory leak que aconteceria se eu só reescrevesse o ponteiro em nomeEstacao
    // Sem desalocar a string que ja exisita nele
    fread(&(pregData->tamNomeEstacao), sizeof(int), 1, BIN);
    if (pregData->tamNomeEstacao > 0){
        fread(bufferNome, sizeof(char), pregData->tamNomeEstacao, BIN);
        bufferNome[pregData->tamNomeEstacao] = '\0';
        if (pregData->nomeEstacao != NULL)
            free(pregData->nomeEstacao);
        pregData->nomeEstacao = strdup(bufferNome);
    }
    else{
        if (pregData->nomeEstacao != NULL)
            free(pregData->nomeEstacao);
        pregData->nomeEstacao = strdup("");
    }

    // Nome da linha
    fread(&(pregData->tamNomeLinha), sizeof(int), 1, BIN);
    if (pregData->tamNomeLinha > 0){
        fread(bufferNome, sizeof(char), pregData->tamNomeLinha, BIN);
        bufferNome[pregData->tamNomeLinha] = '\0';
        if (pregData->nomeLinha != NULL)
            free(pregData->nomeLinha);
        pregData->nomeLinha = strdup(bufferNome);
    }
    else{
        if (pregData->nomeLinha != NULL)
            free(pregData->nomeLinha);
        pregData->nomeLinha = strdup("");
    }

    // Compensar o lixo no resto do registro
    int tamanho = 1 + sizeof(int) * 9 + pregData->tamNomeEstacao + pregData->tamNomeLinha;
    if ((80 - tamanho) > 0){
        char descarta[80];
        fread(descarta, sizeof(char), 80 - tamanho, BIN);
    }

    return 1;
}

// funcoes auxiliares para o printDados nao ficar gigante
void regData_printInt(int input){
    char nulo[] = "NULO";
    if (input == -1)
        printf("%s ", nulo);
    else
        printf("%d ", input);
}

void regData_printString(char *string){
    char nulo[] = "NULO";
    if (strcmp(string, "") == 0)
        printf("%s ", nulo);
    else
        printf("%s ", string);
}

void regData_printData(regData inputReg){
    regData_printInt(inputReg.codEstacao);
    regData_printString(inputReg.nomeEstacao);
    regData_printInt(inputReg.codLinha);
    regData_printString(inputReg.nomeLinha);
    regData_printInt(inputReg.codProxEstacao);
    regData_printInt(inputReg.distProxEstacao);
    regData_printInt(inputReg.codLinhaIntegra);
    regData_printInt(inputReg.codEstIntegra);
    printf("\n");
}

// SEGUNDA FUNCIONALIDADE
int regData_printBIN(FILE *BIN){
    regHeader tempHead;
    regData tempData;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;
    REGCab_ler(BIN, &tempHead);

    if (tempHead.status == '0') return -1;

    char readAtLeastOne = 0;
    int flag = 1;
    while (flag != -1){
        flag = regData_read(BIN, &tempData);
        if (flag == 1){
            regData_printData(tempData);
            readAtLeastOne = 1;
        }
        // Se flag == 0 significa que tentou ler um registro removido
        // entao nao faz nada e continua
    }

    // Verificar se leu nenhum registro
    if (readAtLeastOne == 0) return 0;

    free(tempData.nomeEstacao);
    free(tempData.nomeLinha);
}

// Funcao auxiliar para a buscaReg
// 1 se passou, 0 se não passou
// Que funçao feia meu deus
// Valor nulo de string dentro do criteria é "" igual ao valor nulo de string na memoria
char regData_testCriteria(regData registro, criteria teste){

    if (strcmp(teste.nomeCampo, "codEstacao") == 0){
        return registro.codEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "nomeEstacao") == 0){
        return strcmp(registro.nomeEstacao, teste.valorString) == 0;
    }

    if (strcmp(teste.nomeCampo, "codLinha") == 0){
        return registro.codLinha == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "nomeLinha") == 0){
        return strcmp(registro.nomeLinha, teste.valorString) == 0;
    }

    if (strcmp(teste.nomeCampo, "codProxEstacao") == 0){
        return registro.codProxEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "distProxEstacao") == 0){
        return registro.distProxEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "codLinhaIntegra") == 0){
        return registro.codLinhaIntegra == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "codEstIntegra") == 0){
        return registro.codEstIntegra == teste.valorInt;
    }
}

// Procura por um registro que atende todos os criterias e retorna a posicao dele no bin
// e copia o conteudo dele para o outputReg na memoria
// Começa a procurar a partir da posicao atual do ponteiro do arquivo e move ele para frente
// tambem supoe que ja tenha passado pelo cabecalho
// retorna -1 se nao achou nada(fim de arquivo)
int regData_searchReg(FILE *BIN, regData *outputReg, int m, criteria tests[]){
    int foundOne = 0;
    while (foundOne == 0){
        int flag = regData_read(BIN, outputReg);
        switch (flag){
        case -1: // fim de arquivo
            return -1;
        case 0: // Achou um registro removido
            continue;
        case 1: // leu um registro, testar criterias
            char failed = 0;

            for (int i = 0; i < m; i++){
                if (regData_testCriteria(*outputReg, tests[i]) == 0){
                    failed = 1;
                    break;
                }
            }

            if (failed == 0)
                foundOne = 1;
            break;
        }
    }
    return ftell(BIN) - 80; // Eu passei pelo registro que eu queria por causa dos freads, entao tenho que compensar voltando 80 bytes
}

// Essa funcao eu fiz direto com o input do terminal pq eu nao sei como separar a parte do print e da busca sem enlouquecer com alocacao dinamica
void regData_printWithInputCriteria(FILE *BIN){

    regHeader tempHead;

    REGCab_ler(BIN, &tempHead);

    if (tempHead.status == '0'){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    int n = 0;
    scanf("%d", &n);

    // Esse for itera pelas diferentes buscas
    for (int i = 0; i < n; i++){
        fseek(BIN, 17, SEEK_SET); // volto para o comeco depois do cabecalho

        int m;
        scanf("%d", &m); // Quantidade de filtros para busca

        criteria *tests = (criteria *)malloc(m * sizeof(criteria));

        // For para ler os criterias e colocar eles no vetor
        for (int j = 0; j < m; j++){
            scanf("%s", tests[j].nomeCampo);

            if (strcmp(tests[j].nomeCampo, "nomeEstacao") == 0 || strcmp(tests[j].nomeCampo, "nomeLinha") == 0){
                ScanQuoteString(tests[j].valorString);
                if (strcmp(tests[j].valorString, "NULO") == 0)
                    strcpy(tests[j].valorString, "");
            }
            else{
                char temp[128];
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0)
                    tests[j].valorInt = -1;
                else
                    tests[j].valorInt = atoi(temp);
            }
        }

        // A partir daqui eu tenho todos os testes prontos e posso começar a buscar
        regData tempReg;
        tempReg.nomeEstacao = NULL;
        tempReg.nomeLinha = NULL;

        int foundOne = 0;
        int flag;

        while ((flag = regData_read(BIN, &tempReg)) != -1){
            if (flag == 1){ // achou um registro valido
                int passedAllTests = 1;
                for (int k = 0; k < m; k++){
                    if (regData_testCriteria(tempReg, tests[k]) == 0){
                        passedAllTests = 0;
                        break;
                    }
                }

                if (passedAllTests){
                    regData_printData(tempReg);
                    foundOne = 1;
                }
            }
        }

        // Liberar a memoria que esta no tempReg
        if (tempReg.nomeEstacao != NULL)
            free(tempReg.nomeEstacao);
        if (tempReg.nomeLinha != NULL)
            free(tempReg.nomeLinha);

        free(tests);

        if (!foundOne){
            printf("Registro inexistente.\n");
        }
    }

    return;
}