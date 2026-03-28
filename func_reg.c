#include "func_reg.h"

//Eu uso fseek aqui pq de qualquer forma o cabecalho vai estar no comeco no arquivo
void REGCab_escrever(FILE* outputBIN, regCabecalho* pRegCab){
    fseek(outputBIN, 0, SEEK_SET);
    fwrite(&(pRegCab->status), sizeof(char), 1, outputBIN);  //off 0
    fwrite(&(pRegCab->topo), sizeof(int), 1, outputBIN);      //off 1
    fwrite(&(pRegCab->proxRRN), sizeof(int), 1, outputBIN);    //off 5
    fwrite(&(pRegCab->nEstacoes), sizeof(int), 1, outputBIN);    //off 9
    fwrite(&(pRegCab->nParesEstacao), sizeof(int), 1, outputBIN);    //off 13
}


void REGDados_escrever(FILE* outputBIN, regDados* pRegDados){
    fwrite(&(pRegDados->removido), sizeof(char), 1, outputBIN); //off 0
    fwrite(&(pRegDados->proxRRN), sizeof(int), 1, outputBIN);    //off 1
    fwrite(&(pRegDados->codEstacao), sizeof(int), 1, outputBIN);  //off 5
    fwrite(&(pRegDados->codLinha), sizeof(int), 1, outputBIN);      //off 9
    fwrite(&(pRegDados->codProxEstacao), sizeof(int), 1, outputBIN);  //off 13
    fwrite(&(pRegDados->distProxEstacao), sizeof(int), 1, outputBIN);    //off 17
    fwrite(&(pRegDados->codLinhaIntegra), sizeof(int), 1, outputBIN);    //off 21
    fwrite(&(pRegDados->codEstIntegra), sizeof(int), 1, outputBIN);        //off 25
    fwrite(&(pRegDados->tamNomeEstacao), sizeof(int), 1, outputBIN);      //off 29
    fwrite(pRegDados->nomeEstacao, sizeof(char), pRegDados->tamNomeEstacao, outputBIN);         //varzea daqui para baixo
    fwrite(&(pRegDados->tamNomeLinha), sizeof(int), 1, outputBIN);
    fwrite(pRegDados->nomeLinha, sizeof(char), pRegDados->tamNomeLinha, outputBIN);


    //Preencher com lixo
    const char caracterLixo = '$';
    int tamanho_total = 0;
    tamanho_total = sizeof(int) * 9 + sizeof(char) + pRegDados->tamNomeLinha + pRegDados->tamNomeEstacao;
    for(int i=0; i<80-tamanho_total; i++){
        fwrite(&caracterLixo, sizeof(char), 1, outputBIN);
    }

}

void REGCab_ler(FILE* BIN, regCabecalho* pRegCab){
    fseek(BIN, 0, SEEK_SET);
    fread(&(pRegCab->status), sizeof(char), 1, BIN);
    fread(&(pRegCab->topo), sizeof(int), 1, BIN);
    fread(&(pRegCab->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegCab->nEstacoes), sizeof(int), 1, BIN);
    fread(&(pRegCab->nParesEstacao), sizeof(int), 1, BIN);
}

//Sem tratamento do caso de registro removido
//para nao precisar ficar usando fseek (essa funcao vai aparecer dentro e loop), os freads ja movem o ponteiro
//Retorna -1 se chegou no fim do arquivo
int REGDados_lerBASE(FILE* BIN, regDados* pRegDados){

    char verificarFimDoArquivo = fread(&(pRegDados->removido), sizeof(char), 1, BIN);
    if(verificarFimDoArquivo != 1) return -1;

    fread(&(pRegDados->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegDados->codEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->codLinha), sizeof(int), 1, BIN);
    fread(&(pRegDados->codProxEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->distProxEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->codLinhaIntegra), sizeof(int), 1, BIN);
    fread(&(pRegDados->codEstIntegra), sizeof(int), 1, BIN);
    
    char bufferNome[128];
    
    //Nome da estacao
    //Ficou um pouco horroroso pq eu preciso previnir memory leak se eu só reescrevesse o ponteiro em nomeEstacao
    //Sem desalocar a string que ja exisita nele
    fread(&(pRegDados->tamNomeEstacao), sizeof(int), 1, BIN);
    if (pRegDados->tamNomeEstacao > 0) {
        fread(bufferNome, sizeof(char), pRegDados->tamNomeEstacao, BIN);
        bufferNome[pRegDados->tamNomeEstacao] = '\0';
        if (pRegDados->nomeEstacao != NULL) free(pRegDados->nomeEstacao);
        pRegDados->nomeEstacao = strdup(bufferNome);
    } else {
        if (pRegDados->nomeEstacao != NULL) free(pRegDados->nomeEstacao);
        pRegDados->nomeEstacao = strdup("");
    }

    //Nome da linha
    fread(&(pRegDados->tamNomeLinha), sizeof(int), 1, BIN);
    if (pRegDados->tamNomeLinha > 0) {
        fread(bufferNome, sizeof(char), pRegDados->tamNomeLinha, BIN);
        bufferNome[pRegDados->tamNomeLinha] = '\0';
        if (pRegDados->nomeLinha != NULL) free(pRegDados->nomeLinha);
        pRegDados->nomeLinha = strdup(bufferNome);
    } else {
        if (pRegDados->nomeLinha != NULL) free(pRegDados->nomeLinha);
        pRegDados->nomeLinha = strdup("");
    }

    //Compensar o lixo no resto do registro
    int tamanho = 1 + sizeof(int) * 9 + pRegDados->tamNomeEstacao + pRegDados->tamNomeLinha;
    if ( (80 - tamanho) > 0) {
        char descarta[80];
        fread(descarta, sizeof(char), 80-tamanho, BIN);
    }

    return 1;
}


//Retorna 1 se deu certo
//Retorna 0 se tentou ler um reg removido
//Retorna -1 se chegou no fim do arquivo
int REGDados_ler(FILE* BIN, regDados* pRegDados){
    regDados temp;
    temp.nomeEstacao = NULL;
    temp.nomeLinha = NULL;

    //Se fim for -1 significa que chegou no fim do arquivo
    char fim = REGDados_lerBASE(BIN, &temp);
    if(fim == -1) return -1;

    //Se o reg ser do tipo removido eu desaloco as strings e retorno 0
    if(temp.removido == 1){
        if(temp.nomeEstacao !=NULL) free(temp.nomeEstacao);
        if(temp.nomeLinha != NULL) free(temp.nomeLinha);
        return 0;
    }

    // fazer pRegDados receber temp
    if(pRegDados->nomeEstacao != NULL) free(pRegDados->nomeEstacao);
    if(pRegDados->nomeLinha != NULL) free(pRegDados->nomeLinha);
    pRegDados->removido = temp.removido;
    pRegDados->proxRRN = temp.proxRRN;
    pRegDados->codEstacao = temp.codEstacao;
    pRegDados->codLinha = temp.codLinha;
    pRegDados->codProxEstacao = temp.codProxEstacao;
    pRegDados->distProxEstacao = temp.distProxEstacao;
    pRegDados->codLinhaIntegra = temp.codLinhaIntegra;
    pRegDados->codEstIntegra = temp.codEstIntegra;
    pRegDados->tamNomeEstacao = temp.tamNomeEstacao;
    pRegDados->tamNomeLinha = temp.tamNomeLinha;

    //Pegar strings
    if(temp.nomeEstacao !=NULL){
        pRegDados->nomeEstacao = strdup(temp.nomeEstacao);
        free(temp.nomeEstacao);
    } else {
        pRegDados->nomeEstacao = strdup("");
    }

    if(temp.nomeLinha !=NULL){
        pRegDados->nomeLinha = strdup(temp.nomeLinha);
        free(temp.nomeLinha);
    } else {
        pRegDados->nomeLinha = strdup("");
    }

    return 1;

}

//eu estou usando ponteiro para cabecalho para parear o cabecalho no arquivo e na memoria
void REG_criarBIN(FILE* outputBIN, regCabecalho* pRegCab, regDados** vetorRegDados, int TamanhoVetor){
    REGCab_escrever(outputBIN, pRegCab);
    for(int i = 0; i<TamanhoVetor; i++){
        REGDados_escrever(outputBIN, vetorRegDados[i]);
    }
    //agora que eu terminei de escrever eu ajusto o status
    fseek(outputBIN, 0, SEEK_SET);
    const char status = 1;
    fwrite(&status, sizeof(char), 1, outputBIN);

    pRegCab->status = 1; // parear cabecalho memoria e no disco
}

//funcoes auxiliares para o printDados nao ficar gigante
void REGDados_printInt(int input){
    char nulo[] = "NULO";
    if(input == -1) printf("%s ", nulo);
    else printf("%d ", input);
}

void REGDados_printString(char* string){
    char nulo[] = "NULO";
    if(strcmp(string, "") == 0) printf("%s ", nulo);
    else printf("%s ", string);
}

void REGDados_printDados(regDados registro){
    REGDados_printInt(registro.codEstacao);
    REGDados_printString(registro.nomeEstacao);
    REGDados_printInt(registro.codLinha);
    REGDados_printString(registro.nomeLinha);
    REGDados_printInt(registro.codProxEstacao);
    REGDados_printInt(registro.distProxEstacao);
    REGDados_printInt(registro.codLinhaIntegra);
    //Ultimo nao pode ter espaco eu acho
    char nulo[] = "NULO";
    if(registro.codEstIntegra == -1) printf("%s\n", nulo);
    else printf("%d\n", registro.codEstIntegra);
}

void REGDados_printBIN(FILE* BIN){
    regCabecalho tempCab;
    regDados tempDados;

    REGCab_ler(BIN, &tempCab);

    if (tempCab.status == '0') {
        printf("Registro inexistente.\n");
        return;
    }

    tempDados.nomeEstacao = NULL;
    tempDados.nomeLinha = NULL;

    int flag = 1;
    while (flag != -1) {
        flag = REGDados_ler(BIN, &tempDados);
        if (flag == 1) REGDados_printDados(tempDados);
        //Se flag == 0 significa que tentou ler um registro removido
        //entao nao faz nada e continua
    }

    free(tempDados.nomeEstacao);
    free(tempDados.nomeLinha);
}