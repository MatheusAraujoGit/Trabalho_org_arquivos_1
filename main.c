#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Cabeçalho
typedef struct {
    char status;    // 0 inconsistente, 1 consistente
    int topo;       // RRN do topo da pilha de removidos
    int proxRRN;    // proximo rrn disponivel
    int nEstacoes;
    int nParesEstacao;
} regCabecalho;

//Dados
typedef struct {
    char removido;
    int proximoRRNRemovido; // proximo registro removido para o funcionamento da pilha de removidos
    int codEstacao;         
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;     
    char* nomeEstacao;   
    int tamNomeLinha;       
    char* nomeLinha;     
} regDados;

//Funcao auxiliar para selecionar qual campo eu quero de uma linha
//retorna ponteiro para primeira posicao do campo que eu quero depois da virgula
//Campo vai de 1 a 8
char* ponteiroCampo(char* linha, int campo) {
    if (campo == 1) return linha;

    int virgulasEncontradas = 0;
    char* pCaracter = linha;

    //Vai varrendo e contando as virgulas para achar qual campo eu quero
    while (*pCaracter != '\0') {
        if (*pCaracter == ',') {
            virgulasEncontradas++;
            if (virgulasEncontradas == campo - 1) {
                return pCaracter + 1; // Retorna o ponteiro para o caractere após a vírgula
            }
        }
        pCaracter++;
    }

    return NULL; //Campo vazio
}

//Escreve no campo &campo[256]
//Se estiver vazio campo[256] = "";
void lerStringCampo(char outputVetor[], char* linha, int nCampo){

    if(ponteiroCampo(linha, nCampo) == NULL){
        strcpy(outputVetor, "");
        return;
    }

    strcpy(outputVetor, ponteiroCampo(linha, nCampo));
    char* pCaracter= outputVetor;
    while(*pCaracter != '\0'){
        if(*pCaracter == ','){
            *pCaracter = '\0';
        }
        pCaracter++;
    }
    return;
}

//Retorna o valor int de um campo em uma string
int lerIntCampo(char outputVetor[], char* linha, int nCampo){
    lerStringCampo(outputVetor, linha, nCampo);
    if(strcmp(outputVetor, "") == 0) return -1;    //significa campo nulo
    else{
        return atoi(outputVetor);
    }
}

void cabecalho_Criar(FILE* inputCSV, regCabecalho* inputCab){
    //Inicializando
    fseek(inputCSV, 0, SEEK_SET);
    inputCab->status = '0';
    inputCab->topo = -1;
    inputCab->proxRRN = 0;
    inputCab->nEstacoes = 0;
    inputCab->nParesEstacao = 0;

    char linha[512];
    char campo[256];
    char* ptr;

    fgets(linha, sizeof(linha), inputCSV);    //para pular a primeira linha

    //Contar numero de estacoes
    int nVirgulas = 0;
    char flag = 'a';
    while(flag != EOF){
        flag = fgetc(inputCSV);
        nVirgulas += (flag == ',');
    }
    inputCab->nEstacoes = nVirgulas/7;      //Cada registro tem 7 virgulas, assim o nVirgulas/7 é o numero de registros
    
    //Agora contar numero de pares
    //estou considerando que, para (estacao, proxima_estacao), (A, B) é diferente de (B, A), pois a palavra proximo indica direcao,
    //e uma estacao nao é necessariamente bidirecional com a sua posterior
    fseek(inputCSV, 0, SEEK_SET);
    fgets(linha, sizeof(linha), inputCSV);  //pular primeira linha de novo

    //Contando
    int acum = 0;
    for(int i = 0; i<inputCab->nEstacoes; i++){
        fgets(linha, sizeof(linha), inputCSV);
        int valor = lerIntCampo(campo, linha, 5);
        if(valor != -1) acum += 1;
    }
    inputCab->nParesEstacao = acum;

}

int main() {
    FILE* inputCSV = fopen("estacoes.csv", "r");
    FILE* outputBIN;

    regCabecalho cab;
    cabecalho_Criar(inputCSV, &cab);
    printf("%d\n", cab.nEstacoes);
    printf("%d\n", cab.nParesEstacao);

    return 0;
}