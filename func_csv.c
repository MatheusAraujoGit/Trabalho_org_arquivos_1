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

void CSV_createHeader(FILE *inputCSV, regHeader *inputHeader){
    //inicializando como inconsistente
    fseek(inputCSV, 0, SEEK_SET);
    inputHeader->status = '0';
    inputHeader->topo = -1;
    inputHeader->proxRRN = 0;
    inputHeader->nEstacoes = 0;
    inputHeader->nParesEstacao = 0;

    char line[512];
    char field[128];

    fgets(line, sizeof(line), inputCSV); // para pular a primeira linha

    // Obter numero de estacoes (usando hash table)
    int capacity = 10;
    int* nomesUnicos = malloc(capacity*sizeof(int));

    // Colocar nomes unicos hasheados no array
    while (fgets(line, sizeof(line), inputCSV) != NULL){
        CSV_readStringField(field, line, 2); // Ler

        // Verificar se ja existe
        int hash = CSV_fnv1a_hash(field);
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

    // Agora contar numero de pares
    // No aulao disseram que o grafo das estacoes era nao direcional ( (A,B) = (B,A)) e baseado em ids

    // Volto para o inicio do arquivo(eu tecnicamente poderia fazer a contagem de pares de estacoes ao mesmo tempo ja que uma usa ids e a outra os nomes...
    // Mas isso ia ficar MUITO feio)
    fseek(inputCSV, 0, SEEK_SET);

    fgets(line, sizeof(line), inputCSV); // pular primeira linha de novo

    // Vou pegar o maior id para criar a matriz de adjacencia e o numero de RRNs
    
    int proxRRN = 0;
    int maxID = 0;
    while (fgets(line, sizeof(line), inputCSV) != NULL){
        proxRRN++;
        int ID = CSV_readIntField(line, 1);
        if(ID>maxID) maxID = ID;
    }

    // crio matrix de adjancencia
    char matAdj[maxID + 1][maxID + 1]; // estou usando maxID+1 para poder ir de 1 a maxID ao inves de 0 a maxID-1

    // Inicializo a matriz de adjacencia
    for (int i = 0; i < maxID + 1; i++){
        for (int j = 0; j < maxID + 1; j++)
        {
            matAdj[i][j] = 0;
        }
    }

    // Agora eu coloco 1 em matAdj[A][B] e matAdj[B][A] se eles sao pares
    // Preciso tratar os casos em que A = B tambem
    fseek(inputCSV, 0, SEEK_SET); // Ultima vez que eu volto no começo do arquivo
    fgets(line, sizeof(line), inputCSV);

    while (fgets(line, sizeof(line), inputCSV) != NULL){
        int ID, Prox;
        ID = CSV_readIntField(line, 1);
        Prox = CSV_readIntField(line, 5);
        if (ID != -1 && Prox != -1){

            if(ID == Prox)
                matAdj[ID][Prox] = 2;
            
            else {
            matAdj[ID][Prox] = 1;
            matAdj[Prox][ID] = 1;
            }

        }
    }

    // Agora eu conto o numero total de 1s na matAdj e divido por 2 para obter o numero de pares
    int acum = 0;
    for (int i = 0; i < maxID + 1; i++){
        for (int j = 0; j < maxID + 1; j++){
            acum += matAdj[i][j];
        }
    }
    inputHeader->nParesEstacao = acum / 2;
    inputHeader->proxRRN = proxRRN;
}

//Vai de linha a linha lendo no csv escrevendo os registros no BIN
void CSV_createBIN(FILE* inputCSV, FILE* outputBIN){
    regData tempReg;
    regHeader tempHeader;

    char lineBuffer[512];

    //header já é inicializado como inconsistente
    CSV_createHeader(inputCSV, &tempHeader);
    fseek(inputCSV, 0, SEEK_SET);
    fseek(outputBIN, 0, SEEK_SET);
    regHeader_write(outputBIN, &tempHeader);

    fgets(lineBuffer, sizeof(lineBuffer), inputCSV);
    while(fgets(lineBuffer, sizeof(lineBuffer), inputCSV) != NULL){
        tempReg = CSV_registerPerLine(lineBuffer);
        regData_write(outputBIN, &tempReg);
    }
    regHeader_setFileConsistent(outputBIN);
}