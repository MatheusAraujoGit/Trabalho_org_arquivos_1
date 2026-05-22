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

//vai para o começo do arquivo e escreve um registro de header em pRegheader
void regHeader_write(FILE *outputBIN, regHeader *pRegHeader){
    fseek(outputBIN, 0, SEEK_SET);
    fwrite(&(pRegHeader->status), sizeof(char), 1, outputBIN);       // off 0
    fwrite(&(pRegHeader->topo), sizeof(int), 1, outputBIN);          // off 1
    fwrite(&(pRegHeader->proxRRN), sizeof(int), 1, outputBIN);       // off 5
    fwrite(&(pRegHeader->nEstacoes), sizeof(int), 1, outputBIN);     // off 9
    fwrite(&(pRegHeader->nParesEstacao), sizeof(int), 1, outputBIN); // off 13
}

//escreve um registro de dado em pregData na posição atual do arquivo
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

//lê o registro de header e o guarda em pregHeader
void regHeader_read(FILE *BIN, regHeader *pRegHeader){
    fseek(BIN, 0, SEEK_SET);
    fread(&(pRegHeader->status), sizeof(char), 1, BIN);
    fread(&(pRegHeader->topo), sizeof(int), 1, BIN);
    fread(&(pRegHeader->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegHeader->nEstacoes), sizeof(int), 1, BIN);
    fread(&(pRegHeader->nParesEstacao), sizeof(int), 1, BIN);
}

//lê um registro de dado na posição atual do arquivo e o guarda em pregData
//retorno: -1 se falhou, 0 se estava deletado, 1 se conseguiu
int regData_read(FILE *BIN, regData *pregData){

    char check_EOF = fread(&(pregData->removido), sizeof(char), 1, BIN);
    if (check_EOF != 1)
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

// funcoes auxiliares do printData -----
void regData_printInt(int input){
    char nulo[] = "NULO";
    if (input == -1)
        printf("%s ", nulo);
    else
        printf("%d ", input);
}

// funcoes auxiliares do printData -----
void regData_printString(char *string){
    char nulo[] = "NULO";
    if (strcmp(string, "") == 0)
        printf("%s ", nulo);
    else
        printf("%s ", string);
}

//printa os dados de um registro
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

//printa todos os dados de um binario
//-1 é erro, 0 é êxito
int regData_printBIN(FILE *BIN){
    regHeader tempHead;
    regData tempData;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;
    regHeader_read(BIN, &tempHead);

    if (tempHead.status == '0') return -1;

    char readOne = 0;
    int flag = 1;
    while (flag != -1){
        flag = regData_read(BIN, &tempData);
        if (flag == 1){
            regData_printData(tempData);
            readOne = 1;
        }
    }

    if (readOne == 0) printf("Registro inexistente.\n");

    if (tempData.nomeEstacao != NULL)
        free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL)
        free(tempData.nomeLinha);

    return 0;
}

int print_with_criteria(FILE* BIN){
    regData tempData;
    regHeader tempHead;
    regCriteria criteria;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;

    criteria = createCriteriaRegister();

    bool found_one = false;
    while(regData_read(BIN, &tempData) != -1){
        if(do_they_match(criteria,tempData)){
            regData_printData(tempData);
            found_one = true;
        }
    }
    if(!found_one) printf("Registro inexistente.\n");
    printf("\n");

    if (tempData.nomeEstacao != NULL)
        free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL)
        free(tempData.nomeLinha);
    return 0;
}

//seta o registro em RRN como deletado;
//seta seu proxRemovido para o topo do cabecalho;
//seta o topo do cabecalho para ele
void regData_DeleteRegistry(FILE* BIN, int RRN){
    regHeader tempHead;
    regData tempData;
    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    fseek(BIN, -80, SEEK_CUR);
    int status = regData_read(BIN, &tempData);
    if(status != 1) return;

    regHeader_read(BIN, &tempHead);

    tempData.removido = '1';
    tempData.proxRemovido = tempHead.topo;
    if(tempData.codProxEstacao != -1) tempHead.nParesEstacao--;

    tempHead.topo = RRN;
    regHeader_write(BIN, &tempHead);

    fseek(BIN, 17+80*RRN, SEEK_SET);
    regData_write(BIN, &tempData);

    if (tempData.nomeEstacao != NULL) free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL) free(tempData.nomeLinha);

    return;
}

//cria um registro de criterios, e deleta todos os registros no bin que satisfazem o criterio
int search_and_delete(FILE* BIN){
    regData tempData;
    regHeader tempHead;
    regCriteria criteria;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;

    criteria = createCriteriaRegister();

    int RRN = -1;
    while(regData_read(BIN, &tempData) != -1){
        RRN++;
        if(tempData.removido == '1') continue;
        if(do_they_match(criteria,tempData)) regData_DeleteRegistry(BIN, RRN);
    }

    regHeader_recalculateNEstacoes(BIN);
    if (tempData.nomeEstacao != NULL)
        free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL)
        free(tempData.nomeLinha);
    return 0;
}

//aux: converter uma string pra um int usando o hash FNV-1a (pra melhorar comparação de strings)
int fnv1a_hash(const char* str) {
    int hash = 2166136261; // FNV offset basis
    while (*str) {
        hash ^= *str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}

//recalcular o Nestaçoes de um binário
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

//cria um registro a partir da entrada do usuario
regData createRegister(){
    regData regOutput;

    char nomesBuffer[256];
    char intsBuffer[16];
    
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

//insere um registro de dados, reutilzando espaço de dados removidos se possivel
//se não, o coloca no fim do arquivo
int insert(FILE* BIN){
    regData tempData;
    regHeader tempHead;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;
    
    tempData = createRegister();

    if(tempHead.topo != -1){
        fseek(BIN,17 + tempHead.topo*80,SEEK_SET);
        regData curr_top;
        regData_read(BIN, &curr_top);
        fseek(BIN, -80, SEEK_CUR);
        regData_write(BIN, &tempData);
        tempHead.topo = curr_top.proxRemovido;
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

//diz se um registro de dados satisfaz os critérios de um registro de critério
bool do_they_match(regCriteria criteria, regData data){

    bool all_matched = true;
    if((criteria.codEstacao != -2 && data.codEstacao != criteria.codEstacao)
    ||(criteria.codEstIntegra != -2 && data.codEstIntegra != criteria.codEstIntegra)
    ||(criteria.codLinha != -2 && data.codLinha != criteria.codLinha)
    ||(criteria.codLinhaIntegra != -2 && data.codLinhaIntegra != criteria.codLinhaIntegra)
    ||(criteria.codProxEstacao != -2 && data.codProxEstacao != criteria.codProxEstacao)
    ||(criteria.distProxEstacao != -2 && data.distProxEstacao != criteria.distProxEstacao)
    ||(criteria.tamNomeEstacao != -2 && data.tamNomeEstacao != criteria.tamNomeEstacao)
    ||(criteria.tamNomeLinha != -2 && data.tamNomeLinha != criteria.tamNomeLinha)
    ||(criteria.nomeEstacao != NULL && strcmp(criteria.nomeEstacao,data.nomeEstacao) != 0)
    ||(criteria.nomeLinha != NULL && strcmp(criteria.nomeLinha,data.nomeLinha) != 0)) all_matched = false;
    
    return all_matched;
}

//cria um registro de critério a partir de entradas do usuário
regCriteria createCriteriaRegister(){
    regCriteria regOutput;

    regOutput.codEstacao = -2;
    regOutput.codEstIntegra = -2;
    regOutput.codLinha = -2;
    regOutput.codLinhaIntegra = -2;
    regOutput.codProxEstacao = -2;
    regOutput.distProxEstacao = -2;
    regOutput.nomeEstacao = NULL;
    regOutput.nomeLinha = NULL;
    regOutput.tamNomeEstacao = -2;
    regOutput.tamNomeLinha = -2;

    int criteria_to_go;
    char nomesBuffer[256];
    char intsBuffer[16];
    
    scanf("%d ", &criteria_to_go);

    while(criteria_to_go > 0){
        scanf("%s ", nomesBuffer);
        if(strcmp(nomesBuffer,"codEstacao") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.codEstacao = -1;
            else regOutput.codEstacao = atoi(intsBuffer);
        }
        else if(strcmp(nomesBuffer,"nomeEstacao") == 0){
            ScanQuoteString(nomesBuffer);
            regOutput.nomeEstacao = strdup(nomesBuffer);
        }
        else if(strcmp(nomesBuffer,"codLinha") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.codLinha = -1;
            else regOutput.codLinha = atoi(intsBuffer);
        }
        else if(strcmp(nomesBuffer,"nomeLinha") == 0){
            ScanQuoteString(nomesBuffer);
            regOutput.nomeLinha = strdup(nomesBuffer);
        }
        else if(strcmp(nomesBuffer,"codProxEstacao") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.codProxEstacao = -1;
            else regOutput.codProxEstacao = atoi(intsBuffer);
        }
        else if(strcmp(nomesBuffer,"distProxEstacao") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.distProxEstacao = -1;
            else regOutput.distProxEstacao = atoi(intsBuffer);
        }
        else if(strcmp(nomesBuffer,"codLinhaIntegra") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.codLinhaIntegra = -1;
            else regOutput.codLinhaIntegra = atoi(intsBuffer);
        }
        else if(strcmp(nomesBuffer,"codEstIntegra") == 0){
            scanf("%s ", intsBuffer);
            if(strcmp("NULO",intsBuffer) == 0) regOutput.codEstIntegra = -1;
            else regOutput.codEstIntegra = atoi(intsBuffer);
        }
        criteria_to_go--;
    }

    return regOutput;
}

//a partir de um criterio de updates e um registro de dados, vê tudo o que precisa ser atualizado
//e sobreescreve o registro de dados com a versão atualizada
void regData_updateReg(FILE* BIN, regCriteria updates, regData old){
    regData new = old;

    if(updates.codEstacao != -2 && old.codEstacao != updates.codEstacao) new.codEstacao = updates.codEstacao;
    if(updates.codEstIntegra != -2 && old.codEstIntegra != updates.codEstIntegra) new.codEstIntegra = updates.codEstIntegra;
    if(updates.codLinha != -2 && old.codLinha != updates.codLinha) new.codLinha = updates.codLinha;
    if(updates.codLinhaIntegra != -2 && old.codLinhaIntegra != updates.codLinhaIntegra) new.codLinhaIntegra = updates.codLinhaIntegra;
    if(updates.codProxEstacao != -2 && old.codProxEstacao != updates.codProxEstacao) new.codProxEstacao = updates.codProxEstacao;
    if(updates.distProxEstacao != -2 && old.distProxEstacao != updates.distProxEstacao) new.distProxEstacao = updates.distProxEstacao;
    if(updates.tamNomeEstacao != -2 && old.tamNomeEstacao != updates.tamNomeEstacao) new.tamNomeEstacao = updates.distProxEstacao;
    if(updates.tamNomeLinha != -2 && old.tamNomeLinha != updates.tamNomeLinha) new.tamNomeLinha = updates.tamNomeLinha;
    if(updates.nomeEstacao != NULL && strcmp(updates.nomeEstacao,old.nomeEstacao) != 0) new.nomeEstacao = strdup(updates.nomeEstacao);
    if(updates.nomeLinha != NULL && strcmp(updates.nomeLinha,old.nomeLinha) != 0) new.nomeLinha = strdup(updates.nomeLinha);

    fseek(BIN, -80, SEEK_CUR);
    regData_write(BIN, &new);
}

//cria um registro de criterio e um de atualizaçao, e atualiza todos os
//registros que satisfazerem o de criterio com o de atualizaçao
int update(FILE* BIN){
    regData tempData;
    regHeader tempHead;
    regCriteria criteria;
    regCriteria to_update;

    tempData.nomeEstacao = NULL;
    tempData.nomeLinha = NULL;

    regHeader_read(BIN, &tempHead);
    if (tempHead.status == '0') return -1;

    criteria = createCriteriaRegister();
    to_update = createCriteriaRegister();

    while(regData_read(BIN, &tempData) != -1){
        if(tempData.removido == '1') continue;
        if(do_they_match(criteria,tempData)) regData_updateReg(BIN, to_update, tempData);
    }

    regHeader_recalculateNEstacoes(BIN);
    if (tempData.nomeEstacao != NULL)
        free(tempData.nomeEstacao);
    if (tempData.nomeLinha != NULL)
        free(tempData.nomeLinha);
    return 0;
}