#include "func_reg.h"
#include "fornecidas.h"

//pega um arquivo já aberto e coloca ele como consistente
void regHeader_setFileConsistent(FILE* file){
    fseek(file, 0, SEEK_SET);
    const char consistent = '1';
    fwrite(&consistent, sizeof(char), 1, file);
}

//pega um arquivo já aberto e coloca ele como inconsistente
void regHeader_setFileInconsistent(FILE* file){
    fseek(file, 0, SEEK_SET);
    const char inconsistent = '0';
    fwrite(&inconsistent, sizeof(char), 1, file);
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
    fwrite(&(pregData->proxRemovido), sizeof(int), 1, outputBIN);                           // off 1
    fwrite(&(pregData->codEstacao), sizeof(int), 1, outputBIN);                        // off 5
    fwrite(&(pregData->codLinha), sizeof(int), 1, outputBIN);                          // off 9
    fwrite(&(pregData->codProxEstacao), sizeof(int), 1, outputBIN);                    // off 13
    fwrite(&(pregData->distProxEstacao), sizeof(int), 1, outputBIN);                   // off 17
    fwrite(&(pregData->codLinhaIntegra), sizeof(int), 1, outputBIN);                   // off 21
    fwrite(&(pregData->codEstIntegra), sizeof(int), 1, outputBIN);                     // off 25
    fwrite(&(pregData->tamNomeEstacao), sizeof(int), 1, outputBIN);                    // off 29
    fwrite(pregData->nomeEstacao, sizeof(char), pregData->tamNomeEstacao, outputBIN);  // varzea daqui para baixo
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

void regHeader_read(FILE *BIN, regHeader *pRegHeader){
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
    if (pregData->removido == '1'){
        fread(&(pregData->proxRemovido), sizeof(int), 1, BIN);
        fseek(BIN, 79-sizeof(int), SEEK_CUR); // pulo os bytes restantes do registro removido
        return 0;
    }

    fread(&(pregData->proxRemovido), sizeof(int), 1, BIN);
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
    regHeader_read(BIN, &tempHead);

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
    int RRN = 0;

    while (foundOne == 0){
        int flag = regData_read(BIN, outputReg);
        RRN++;
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
    return (RRN)*80 - 17; // Eu passei pelo registro que eu queria por causa dos freads, entao tenho que compensar voltando 1 RRN
}

// Essa funcao eu fiz direto com o input do terminal pq eu nao sei como separar a parte do print e da busca sem enlouquecer com alocacao dinamica
int regData_printWithInputCriteria(FILE *BIN){

    regHeader tempHead;

    regHeader_read(BIN, &tempHead);

    if (tempHead.status == '0') return -1;


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
        printf("\n");
    }

    return 0;
}

//vai para o registro em RRN e o seta como deletado;
//seta seu proxRemovido para o topo do cabecalho;
//seta o topo do cabecalho para ele
int regData_DeleteRegistry(FILE* BIN, int RRN){
    regHeader tempHead;
    regData tempData;
    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;

    fseek(BIN, 17+80*RRN, SEEK_SET);
    int status = regData_read(BIN, &tempData);
    if(status != 1) return status;

    tempData.removido = '1';
    tempData.proxRemovido = tempHead.topo;
    tempHead.nParesEstacao--;   //Como os pares de estacao sao baseados por id, posso subtrair aqui
                                //Porem o nEstacoes é baseado por nome, entao tenho que recalcular

    fseek(BIN, -80, SEEK_CUR);
    regData_write(BIN, &tempData);
    
    tempHead.topo = RRN;
    long currentPos = ftell(BIN);
    regHeader_write(BIN, &tempHead);    //Essa funcao move o ponteiro entao preciso salvar a posicao atual
    fseek(BIN, currentPos, SEEK_SET);   //nao deveriamos precisar salvar a posição atual pq já temos o seu RRN?

    if (tempData.nomeEstacao != NULL) free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL) free(tempData.nomeLinha);

    return status;
}

//funçao copypastada e editada do searchReg que deleta ao encontrar
int regData_searchAndDeleteReg(FILE *BIN, regData *outputReg, int m, criteria tests[]){
    int foundOne = 0;
    int RRN = -1;
    while (foundOne == 0){
        int flag = regData_read(BIN, outputReg);
        RRN++;
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

            if (failed == 0){
                foundOne = 1;
                //int RRN = ((ftell(BIN) - 17) / 80) - 1;
                regData_DeleteRegistry(BIN, RRN);
            }

            break;
        }
    }
    return 1;
}

//aux: converter uma string pra um int usando o hash FNV-1a (pra melhorar comparação de strings depois)
int fnv1a_hash(const char* str) {
    int hash = 2166136261; // FNV offset basis
    while (*str) {
        hash ^= *str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}

//Recalculo o nEstacoes, ja que ele é baseado por nome
//Baseado no CSV_createHeader
void regHeader_recalculateNEstacoes(FILE* BIN){
    regHeader tempHead;
    regData tempData;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    regHeader_read(BIN, &tempHead);
    tempHead.nEstacoes = 0;

    int capacity = 10;
    int* nomesUnicos = malloc(capacity*sizeof(int));
    int flag;
    while( (flag = regData_read(BIN, &tempData)) != -1){
        if (flag == 1){    //flag == 1 significa registro valido

        // Verificar se ja existe com hash
        int hash = fnv1a_hash(tempData.nomeEstacao);
        char jaExiste = 0;

        for(int i = 0; i< tempHead.nEstacoes; i++){
            if (nomesUnicos[i] == hash){
                jaExiste = 1;
                break;
            }
        }

        // Se nao existe, aloco mais memoria pro array e conto uma estação a mais
        if (jaExiste == 0){
            if (tempHead.nEstacoes >= capacity) {
            capacity *= 2;
            nomesUnicos = realloc(nomesUnicos, capacity * sizeof(int));
            }

            nomesUnicos[tempHead.nEstacoes] = hash;
            tempHead.nEstacoes += 1;
            }
        }

        if (tempData.nomeEstacao != NULL){
            free(tempData.nomeEstacao); 
            tempData.nomeEstacao = NULL;
        }
        if (tempData.nomeLinha != NULL){
            free(tempData.nomeLinha); 
            tempData.nomeLinha = NULL;
        }

    }

    //Escrevo o novo Header
    regHeader_write(BIN, &tempHead);

    //Desaloco memoria do TempData
    if (tempData.nomeEstacao != NULL)
        free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL)
        free(tempData.nomeLinha);
}

// funçao copiada e ediatada do printWithInputCriteria
int regData_deleteWithInputCriteria(FILE *BIN){

    regHeader tempHead;

    regHeader_read(BIN, &tempHead);

    if (tempHead.status == '0') return -1;

    int n = 0;
    scanf("%d", &n);

    // Esse for itera pelas diferentes buscas
    char removedAtLeastOne = 0;
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
        int RRN = -1;

        while ((flag = regData_read(BIN, &tempReg)) != -1){
            RRN++;
            if (flag == 1){ // achou um registro valido
                int passedAllTests = 1;
                for (int k = 0; k < m; k++){
                    if (regData_testCriteria(tempReg, tests[k]) == 0){
                        passedAllTests = 0;
                        break;
                    }
                }

                if (passedAllTests){
                    // como executou o regData_read, o ponteiro de arquivo esta no proximo registro, entao compenso subtraindo 1
                    //int RRN = ((ftell(BIN)-17)/80 ) - 1;
                    regData_DeleteRegistry(BIN, RRN);
                    foundOne = 1;
                    removedAtLeastOne = 1;
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
    
    //Preciso recalcular o nEstacoes ja que ele é baseado por nome
    if(removedAtLeastOne)
        regHeader_recalculateNEstacoes(BIN);

    return 0;
}

// Funçao auxiliar que retorna um registro que vem de uma linha da funçao 5
regData func5_createRegister(){
    regData regOutput;

    char nomesBuffer[256];
    char intsBuffer[16];
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    
    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.codEstacao = -1;
    else regOutput.codEstacao = atoi(intsBuffer);

    ScanQuoteString(nomesBuffer);
    regOutput.nomeEstacao = strdup(nomesBuffer);

    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.codLinha = -1;
    else regOutput.codLinha = atoi(intsBuffer);

    ScanQuoteString(nomesBuffer);
    regOutput.nomeLinha = strdup(nomesBuffer);

    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.codProxEstacao = -1;
    else regOutput.codProxEstacao = atoi(intsBuffer);

    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.distProxEstacao = -1;
    else regOutput.distProxEstacao = atoi(intsBuffer);

    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.codLinhaIntegra = -1;
    else regOutput.codLinhaIntegra = atoi(intsBuffer);

    scanf("%s ", intsBuffer);
    if(strcmp("NULO",intsBuffer) == 0) regOutput.codEstIntegra = -1;
    else regOutput.codEstIntegra = atoi(intsBuffer);

    regOutput.removido = '0';
    regOutput.proxRemovido = -1;
    regOutput.tamNomeEstacao = strlen(regOutput.nomeEstacao);
    regOutput.tamNomeLinha = strlen(regOutput.nomeLinha);

    return regOutput;
}

int func5_insert(FILE* BIN){
    regData tempData;
    regHeader tempHead;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;
    
    tempData = func5_createRegister();

    if(tempHead.topo != -1){
        fseek(BIN,17 + tempHead.topo*80,SEEK_SET);
        regData curr_top;
        regData_read(BIN, &curr_top);
        int dont_forget = curr_top.proxRemovido;
        fseek(BIN, -80, SEEK_CUR);
        regData_write(BIN, &tempData);
        tempHead.topo = dont_forget;
        tempHead.proxRRN = tempHead.proxRRN+1;
        if(tempData.codProxEstacao != -1) tempHead.nParesEstacao = tempHead.nParesEstacao+1;
        regHeader_write(BIN, &tempHead);                     
    }
    else{
        fseek(BIN,17 + (tempHead.proxRRN)*80,SEEK_SET);
        regData_write(BIN, &tempData);
        tempHead.proxRRN = tempHead.proxRRN+1;
        if(tempData.codProxEstacao != -1) tempHead.nParesEstacao = tempHead.nParesEstacao+1;
        regHeader_write(BIN, &tempHead);
    }

    regHeader_recalculateNEstacoes(BIN);
}