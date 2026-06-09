#include "func_csv.h"
// ----------------------------------------------------------------------------------------------------------------------------------------
//                                                   funcionalidades do trabalho
// ----------------------------------------------------------------------------------------------------------------------------------------

//percorre um csv de linha a linha, transformando as linhas em registros de dados. no fim, cria o registro de cabeçalho
void createBIN(FILE* inputCSV, FILE* outputBIN){
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
        int hash = fnv1a_hash(nomeEstacao);
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
    
    regHeader_write(outputBIN, &tempHeader);
    regHeader_setFileConsistent(outputBIN);
}

// ----------------------------------------------------------------------------------------------------------------------------------------
//                                            funções usadas nas implementações das funcionalidades
// ----------------------------------------------------------------------------------------------------------------------------------------

//a partir de uma string, cria e retorna um registro de dados
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
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

// o7 csv_createHeader