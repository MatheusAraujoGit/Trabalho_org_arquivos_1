#include "func_csv.h"

// Funcao auxiliar para selecionar qual campo eu quero de uma linha
// retorna ponteiro para primeira posicao do campo que eu quero depois da virgula
// Campo vai de 1 a 8
char *CSV_ponteiroParaCampo(char *linha, int campo)
{
    if (campo == 1)
        return linha;

    int virgulasEncontradas = 0;
    char *pCaracter = linha;

    // Vai varrendo e contando as virgulas para achar qual campo eu quero
    while (*pCaracter != '\0')
    {
        if (*pCaracter == ',')
        {
            virgulasEncontradas++;
            if (virgulasEncontradas == campo - 1)
            {
                return pCaracter + 1; // Retorna o ponteiro para o caractere após a vírgula
            }
        }
        pCaracter++;
    }

    return NULL; // Campo vazio
}

// Escreve no outputVetor
// Se estiver vazio outputVetor = "";
void CSV_lerStringCampo(char outputVetor[], char *linha, int nCampo)
{

    if (CSV_ponteiroParaCampo(linha, nCampo) == NULL)
    { // vazio -> ""
        strcpy(outputVetor, "");
        return;
    }

    strcpy(outputVetor, CSV_ponteiroParaCampo(linha, nCampo)); // supondo que eu quero o campo 2, "16,Ana Rosa,300,..." vira "Ana Rosa,300..." com o CSV_ponteiroParaCampo
    char *pCaracter = outputVetor;
    while (*pCaracter != '\0')
    {
        if (*pCaracter == ',' || *pCaracter == '\n')
        {
            *pCaracter = '\0'; // Com essa parte aqui "Ana Rosa,300..." vira "Ana Rosa\0300..."
        }
        pCaracter++;
    }
    return;
}

// Retorna o valor int de um campo em uma string
int CSV_lerIntCampo(char *linha, int nCampo)
{
    char vetorTemporario[256];
    CSV_lerStringCampo(vetorTemporario, linha, nCampo);
    if (strcmp(vetorTemporario, "") == 0)
        return -1; // significa campo nulo
    else
    {
        return atoi(vetorTemporario); // faz a string descerevendo um numero virar int
    }
}

// Funçao auxiliar que retorna ponteiro para um registro de dados alocado dinamicamente
// dados desse registro vem de uma linha do csv
regDados *CSV_registroPorLinha(char *linha)
{
    regDados *regOutput = (regDados *)malloc(sizeof(regDados));

    // Dados faceis
    regOutput->removido = 0;
    regOutput->proxRRN = -1;
    regOutput->codEstacao = CSV_lerIntCampo(linha, 1);
    regOutput->codLinha = CSV_lerIntCampo(linha, 3);
    regOutput->codProxEstacao = CSV_lerIntCampo(linha, 5);
    regOutput->distProxEstacao = CSV_lerIntCampo(linha, 6);
    regOutput->codLinhaIntegra = CSV_lerIntCampo(linha, 7);
    regOutput->codEstIntegra = CSV_lerIntCampo(linha, 8);

    // Agora conseguir as strings
    // NA MEMORIA vou deixar as strings com final \0, para ter acesso as funcoes da string.h, só vou tirar o \0 na hora de escrever no disco
    char bufferParaString[256];

    CSV_lerStringCampo(bufferParaString, linha, 2);
    regOutput->nomeEstacao = strdup(bufferParaString); // strdup me faz o trabalho de alocar dinamicamente isso aqui

    CSV_lerStringCampo(bufferParaString, linha, 4);
    regOutput->nomeLinha = strdup(bufferParaString);

    // Tamanho das strings sem o \0
    regOutput->tamNomeEstacao = strlen(regOutput->nomeEstacao);
    regOutput->tamNomeLinha = strlen(regOutput->nomeLinha);

    return regOutput;
}

void CSV_cabecalhoCriar(FILE *inputCSV, regCabecalho *inputCab)
{
    // Inicializando
    fseek(inputCSV, 0, SEEK_SET);
    inputCab->status = '0';
    inputCab->topo = -1;
    inputCab->proxRRN = 0;
    inputCab->nEstacoes = 0;
    inputCab->nParesEstacao = 0;

    char linha[512];
    char campo[128];

    fgets(linha, sizeof(linha), inputCSV); // para pular a primeira linha

    // Obter numero de estacoes

    // Vou inicializar o array onde vou colocar os nomes
    // Solucao meio nuclear eu alocar tantos bytes para isso mas funciona
    char *nomesUnicos[512];
    for (int i = 0; i < 512; i++)
    {
        nomesUnicos[i] = NULL;
    }

    // Colocar nomes unicos em no array
    while (fgets(linha, sizeof(linha), inputCSV) != NULL)
    {
        CSV_lerStringCampo(campo, linha, 2); // Ler

        // Verificar se ja existe
        int i = 0;
        char jaExiste = 0;
        while (nomesUnicos[i] != NULL)
        {
            if (strcmp(campo, nomesUnicos[i]) == 0)
            {
                jaExiste = 1;
                break;
            }
            i++;
        }

        // Se nao existe escrevo no array
        if (jaExiste == 0)
        {
            nomesUnicos[i] = strdup(campo); // strdup aloca dinamicamente a string
        }
    }

    // agora eu conto quantos nomes existem em nomesUnicos[]
    int i = 0;
    while (nomesUnicos[i] != NULL)
    {
        inputCab->nEstacoes += 1;
        i++;
    }

    // Liberar memoria
    i = 0;
    while (nomesUnicos[i] != NULL)
    {
        free(nomesUnicos[i]);
        i++;
    }

    // Agora contar numero de pares
    // No aulao disseram que o grafo das estacoes era nao direcional ( (A,B) = (B,A)) e baseado em ids

    // Volto para o inicio do arquivo(eu tecnicamente poderia fazer a contagem de pares e de estacoes ao mesmo tempo ja que uma usa ids e a outra os nomes...
    // Mas isso ia ficar MUITO feio)
    fseek(inputCSV, 0, SEEK_SET);

    fgets(linha, sizeof(linha), inputCSV); // pular primeira linha de novo

    // Para fazer essa contagem estou supondo que os ids são únicos, ou seja, há um id por linha
    int totalID = 0;
    while (fgets(linha, sizeof(linha), inputCSV) != NULL)
    {
        totalID += 1;
    }

    // crio matrix de adjancencia
    char matAdj[totalID + 1][totalID + 1]; // estou usando totalID+1 para poder ir de 1 a totalID ao inves de 0 a totalID-1

    // Inicializo a matriz de adjacencia
    for (int i = 0; i < totalID + 1; i++)
    {
        for (int j = 0; j < totalID + 1; j++)
        {
            matAdj[i][j] = 0;
        }
    }

    // Agora eu coloca 1 em matAdj[A][B] e matAdj[B][A] se eles sao pares
    fseek(inputCSV, 0, SEEK_SET); // Ultima vez que eu volto no começo do arquivo
    fgets(linha, sizeof(linha), inputCSV);

    while (fgets(linha, sizeof(linha), inputCSV) != NULL)
    {
        int ID, Prox;
        ID = CSV_lerIntCampo(linha, 1);
        Prox = CSV_lerIntCampo(linha, 5);
        if (ID != -1 && Prox != -1)
        {
            matAdj[ID][Prox] = 1;
            matAdj[Prox][ID] = 1;
        }
    }

    // Agora eu conto o numero total de 1s na matAdj e divido por 2 para obter o numero de pares
    int acum = 0;
    for (int i = 0; i < totalID + 1; i++)
    {
        for (int j = 0; j < totalID + 1; j++)
        {
            acum += matAdj[i][j];
        }
    }
    inputCab->nParesEstacao = acum / 2;
}

// Vou retornar um vetor dinâmico de ponteiros para regDados e escrever o tamanho dele no TamanhoVetor
regDados **CSV_criarVetorRegDados(FILE *inputCSV, int *TamanhoVetor)
{
    *TamanhoVetor = 0;
    char linha[512];
    fseek(inputCSV, 0, SEEK_SET);
    fgets(linha, sizeof(linha), inputCSV); // Pular primeira linha

    // Vou contar o tamanho do vetor pelo numero de registros necessarios
    while (fgets(linha, sizeof(linha), inputCSV) != NULL)
    {
        *TamanhoVetor += 1;
    }

    // Aloco o vetor de registros de dados
    regDados **vetorOutput = (regDados **)malloc((*TamanhoVetor) * sizeof(regDados *));

    // Volto para o começo do arquivo para agora popular o vetor com os registros
    fseek(inputCSV, 0, SEEK_SET);
    fgets(linha, sizeof(linha), inputCSV);

    for (int i = 0; i < (*TamanhoVetor); i++)
    {
        fgets(linha, sizeof(linha), inputCSV);
        vetorOutput[i] = CSV_registroPorLinha(linha);
    }

    return vetorOutput;
}

// Desalocar toda a memoria do vetorRegDados
void CSV_apagarVetorRegDados(regDados **VetorDados, int TamanhoVetor)
{
    // Preciso desalocar as strings tambem, pq eu aloquei elas dinamicamente
    for (int i = 0; i < TamanhoVetor; i++)
    {
        free(VetorDados[i]->nomeEstacao);
        free(VetorDados[i]->nomeLinha);
        free(VetorDados[i]);
    }
    free(VetorDados);
}
