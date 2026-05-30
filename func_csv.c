#include "func_csv.h"

// Funcao auxiliar para selecionar qual campo eu quero de uma line
// retorna ponteiro para primeira posicao do campo que eu quero depois da virgula
// Campo vai de 1 a 8
char *CSV_pointerToField(char *line, int field){
    if (field == 1)
        return line;

    int foundCommas = 0;
    char *pChar = line;

    // Vai varrendo e contando as virgulas para achar qual campo eu quero
    while (*pChar != '\0'){
        if (*pChar == ','){
            foundCommas++;
            if (foundCommas == field - 1)
                return pChar + 1; // Retorna o ponteiro para o caractere após a vírgula
        }
        pChar++;
    }

    return NULL; // Campo vazio
}

// Escreve no outputVetor
// Se estiver vazio outputVetor = "";
void CSV_readStringField(char outputVetor[], char *line, int fieldNumber){

    if (CSV_pointerToField(line, fieldNumber) == NULL){ // vazio -> ""
        strcpy(outputVetor, "");
        return;
    }

    strcpy(outputVetor, CSV_pointerToField(line, fieldNumber)); // supondo que eu quero o campo 2, "16,Ana Rosa,300,..." vira "Ana Rosa,300..." com o CSV_pointerToField
    char *pChar = outputVetor;
    while (*pChar != '\0')
    {
        if (*pChar == ',' || *pChar == '\n' || *pChar == '\r')
            *pChar = '\0'; // Com essa parte aqui "Ana Rosa,300..." vira "Ana Rosa\0300..."
        
        pChar++;
    }
    return;
}

// Retorna o valor int de um campo em uma string
int CSV_readIntField(char *line, int fieldNumber){
    char buffer[256];
    CSV_readStringField(buffer, line, fieldNumber);
    if (strcmp(buffer, "") == 0)
        return -1; // significa campo nulo
    else
        return atoi(buffer); // faz a string descerevendo um numero virar int

}

// Funçao auxiliar que retorna um registro que vem de uma line do csv
regData CSV_registerPerLine(char *line){
    regData regOutput;

    // Dados faceis
    regOutput.removido = '0';
    regOutput.proxRemovido = -1;
    regOutput.codEstacao = CSV_readIntField(line, 1);
    regOutput.codLinha = CSV_readIntField(line, 3);
    regOutput.codProxEstacao = CSV_readIntField(line, 5);
    regOutput.distProxEstacao = CSV_readIntField(line, 6);
    regOutput.codLinhaIntegra = CSV_readIntField(line, 7);
    regOutput.codEstIntegra = CSV_readIntField(line, 8);

    // Agora conseguir as strings
    // NA MEMORIA vou deixar as strings com final \0, para ter acesso as funcoes da string.h, só vou tirar o \0 na hora de escrever no disco
    char stringBuffer[256];

    CSV_readStringField(stringBuffer, line, 2);
    regOutput.nomeEstacao = strdup(stringBuffer); // strdup me faz o trabalho de alocar dinamicamente isso aqui

    CSV_readStringField(stringBuffer, line, 4);
    regOutput.nomeLinha = strdup(stringBuffer);

    // Tamanho das strings sem o \0
    regOutput.tamNomeEstacao = strlen(regOutput.nomeEstacao);
    regOutput.tamNomeLinha = strlen(regOutput.nomeLinha);

    return regOutput;
}

//aux: converter uma string pra um int usando o hash FNV-1a (pra melhorar comparação de strings depois)
int CSV_fnv1a_hash(const char* str) {
    int hash = 2166136261; // FNV offset basis
    while (*str) {
        hash ^= *str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}


//Vai de linha a linha lendo no csv escrevendo os registros no BIN
//Corrigi para que o header fosse escrito no final e integrei o csv_createHeader para nao precisar fazer malabarismo com o csv
void CSV_createBIN(FILE* inputCSV, FILE* outputBIN){
    regData tempReg;
    
    //Inicializa o cabecalho zerado
    regHeader tempHeader;
    tempHeader.status = '0';
    tempHeader.topo = -1;
    tempHeader.proxRRN = 0;
    tempHeader.nEstacoes = 0;
    tempHeader.nParesEstacao = 0;
    
    char lineBuffer[512];
    
    //variaveis para a parte do cabecalho
    char nomeEstacao[128];
    int capacity = 10;
    int* nomesUnicos = malloc(capacity*sizeof(int));
    
    fseek(inputCSV, 0, SEEK_SET);
    fseek(outputBIN, 17, SEEK_SET); // 17 para escrever depois do cabecalho
    
    fgets(lineBuffer, sizeof(lineBuffer), inputCSV);
    while(fgets(lineBuffer, sizeof(lineBuffer), inputCSV) != NULL){
        //Parte de dados
        tempReg = CSV_registerPerLine(lineBuffer);
        regData_write(outputBIN, &tempReg);
        
        //Parte do cabeçalho
        tempHeader.proxRRN++;
        
        //Pares
        if(tempReg.codProxEstacao != -1) tempHeader.nParesEstacao++;
        
        //Calcular numero de estacoes por nomes unicos
        CSV_readStringField(nomeEstacao, lineBuffer, 2);
        
        // Verificar se ja existe
        int hash = CSV_fnv1a_hash(nomeEstacao);
        char jaExiste = 0;
        
        for(int i = 0; i< tempHeader.nEstacoes; i++){
            if (nomesUnicos[i] == hash){
                jaExiste = 1;
                break;
            }
        }
        
        // Se nao existe, aloco mais memoria pro array (se necessário) e conto uma estação a mais
        if (jaExiste == 0){
            if (tempHeader.nEstacoes >= capacity) {
                capacity *= 2;
                nomesUnicos = realloc(nomesUnicos, capacity * sizeof(int));
            }
            
            nomesUnicos[tempHeader.nEstacoes] = hash;
            tempHeader.nEstacoes += 1;
        }

    if (tempReg.nomeEstacao != NULL)
        free(tempReg.nomeEstacao);
    if (tempReg.nomeLinha != NULL)
        free(tempReg.nomeLinha);
    }
    
    free(nomesUnicos);
    
    //CSV_createHeader(inputCSV, &tempHeader); //RIP createHeader
    regHeader_write(outputBIN, &tempHeader);
    regHeader_setFileConsistent(outputBIN);
}

//Essa parte do código virou saudade
/*
void CSV_createHeader(FILE *inputCSV, regHeader *inputHeader){
    //inicializando como inconsistente
    fseek(inputCSV, 0, SEEK_SET);
    inputHeader->status = '0';
    inputHeader->topo = -1;
    inputHeader->proxRRN = 0;
    inputHeader->nEstacoes = 0;
    inputHeader->nParesEstacao = 0;

    char line[512];
    char nomeEstacao[128];

    fgets(line, sizeof(line), inputCSV); // para pular a primeira linha

    // Obter numero de estacoes (usando hash table)
    int capacity = 10;
    int* nomesUnicos = malloc(capacity*sizeof(int));
    
    //loop para conseguir numero de estacoes e de pares
    while (fgets(line, sizeof(line), inputCSV) != NULL){
        inputHeader->proxRRN++;

        if(CSV_readIntField(line, 5) != -1) inputHeader->nParesEstacao++;

        //Parte do numero de estacoes
        CSV_readStringField(nomeEstacao, line, 2);

        // Verificar se ja existe
        int hash = CSV_fnv1a_hash(nomeEstacao);
        char jaExiste = 0;

        for(int i = 0; i< inputHeader->nEstacoes; i++){
            if (nomesUnicos[i] == hash){
                jaExiste = 1;
                break;
            }
        }

        // Se nao existe, aloco mais memoria pro array (se necessário) e conto uma estação a mais
        if (jaExiste == 0){
            if (inputHeader->nEstacoes >= capacity) {
            capacity *= 2;
            nomesUnicos = realloc(nomesUnicos, capacity * sizeof(int));
            }

            nomesUnicos[inputHeader->nEstacoes] = hash;
            inputHeader->nEstacoes += 1;
        }
    }
    free(nomesUnicos);
}
*/