#include "func_csv.h"

//Funcao auxiliar para selecionar qual campo eu quero de uma linha
//retorna ponteiro para primeira posicao do campo que eu quero depois da virgula
//Campo vai de 1 a 8
char* CSV_ponteiroCampo(char* linha, int campo) {
    if (campo == 1) return linha;

    int virgulasEncontradas = 0;
    char* pCaracter = linha;

    //Vai varrendo e contando as virgulas para achar qual campo eu quero
    while (*pCaracter != '\0') {
        if (*pCaracter == ',') {
            virgulasEncontradas++;
            if (virgulasEncontradas == campo - 1) {
                return pCaracter + 1;   // Retorna o ponteiro para o caractere após a vírgula
            }
        }
        pCaracter++;
    }

    return NULL;    //Campo vazio
}

//Escreve no outputVetor
//Se estiver vazio outputVetor = "";
void CSV_lerStringCampo(char outputVetor[], char* linha, int nCampo){

    if(CSV_ponteiroCampo(linha, nCampo) == NULL){ // vazio -> ""
        strcpy(outputVetor, "");
        return;
    }

    strcpy(outputVetor, CSV_ponteiroCampo(linha, nCampo));// supondo que eu quero o campo 2, "16,Ana Rosa,300,..." vira "Ana Rosa,300..." com o CSV_ponteiroCampo
    char* pCaracter= outputVetor;
    while(*pCaracter != '\0'){ 
        if(*pCaracter == ','){
            *pCaracter = '\0';                            //Com essa parte aqui "Ana Rosa,300..." vira "Ana Rosa\0300..."
        }
        pCaracter++;
    }
    return;
}

//Retorna o valor int de um campo em uma string
int CSV_lerIntCampo(char* linha, int nCampo){
    char vetorTemporario[256];
    CSV_lerStringCampo(vetorTemporario, linha, nCampo);
    if(strcmp(vetorTemporario, "") == 0) return -1; //significa campo nulo
    else{
        return atoi(vetorTemporario);               //faz a string descerevendo um numero virar int
    }
}

void CSV_cabecalhoCriar(FILE* inputCSV, regCabecalho* inputCab){
    //Inicializando
    fseek(inputCSV, 0, SEEK_SET);
    inputCab->status = '0';
    inputCab->topo = -1;
    inputCab->proxRRN = 0;
    inputCab->nEstacoes = 0;
    inputCab->nParesEstacao = 0;

    char linha[512];
    char campo[256];

    fgets(linha, sizeof(linha), inputCSV);      //para pular a primeira linha

    //Obter numero de estacoes
    
    //Vou inicializar o array onde vou colocar os nomes
    char* nomesUnicos[200];
    for(int i = 0; i<200; i++){
        nomesUnicos[i] = NULL;
    }

    while( fgets(linha, sizeof(linha), inputCSV) != NULL ){
        CSV_lerStringCampo(campo, linha, 2);    //Ler

        //Verificar se ja existe
        int i = 0;
        char jaExiste = 0;
        while(nomesUnicos[i] != NULL){
            if(strcmp(campo, nomesUnicos[i]) == 0){
                jaExiste = 1;
                break;
            }
            i++;
        }

        //Se nao existe escrevo no array
        if(jaExiste == 0){
            nomesUnicos[i] = strdup(campo); // strdup aloca dinamicamente a string
        }
    }

    //agora eu conto quantos nomes existem em nomesUnicos[]
    int i = 0;
    while(nomesUnicos[i] != NULL){
        inputCab->nEstacoes += 1;
        i++;
    }

    //Liberar memoria
    i = 0;
    while(nomesUnicos[i] != NULL){
        free(nomesUnicos[i]);
        i++;
    }
    
    //Agora contar numero de pares
    //No aulao disseram que o grafo das estacoes era nao direcional ( (A,B) = (B,A))

    //Volto para o inicio do arquivo(eu tecnicamente poderia fazer a contagem de pares e de estacoes ao mesmo tempo ja que uma usa ids e a outra os nomes...
    //Mas isso ia ficar MUITO feio)
    fseek(inputCSV, 0, SEEK_SET);
    
    fgets(linha, sizeof(linha), inputCSV);  //pular primeira linha de novo

    //Para fazer essa contagem estou supondo que os ids são únicos, ou seja, há um id por linha
    int totalID = 0;
    while(fgets(linha, sizeof(linha), inputCSV) != NULL){
        totalID +=1;
    }

    //crio matrix de adjancencia
    char matAdj[totalID+1][totalID+1]; //estou usando totalID+1 para poder ir de 1 a totalID ao inves de 0 a totalID-1

    //Inicializo a matriz de adjacencia
    for(int i = 0; i<totalID+1; i++){
        for(int j = 0; j<totalID+1; j++){
            matAdj[i][j] = 0;
        }
    }

    //Agora eu coloca 1 em matAdj[A][B] e matAdj[B][A] se eles sao pares
    fseek(inputCSV, 0, SEEK_SET); // Ultima vez que eu volto no começo do arquivo
    while(fgets(linha, sizeof(linha), inputCSV) != NULL){
        int ID, Prox;
        ID = CSV_lerIntCampo(linha, 1);
        Prox = CSV_lerIntCampo(linha, 5);
        if(ID != -1 && Prox != -1){
            matAdj[ID][Prox] = 1;
            matAdj[Prox][ID] = 1;
        }
    }


    //Agora eu conto o numero total de 1s na matAdj e divido por 2 para obter o numero de pares
    int acum = 0;
    for(int i = 0; i<totalID+1; i++){
        for(int j = 0; j<totalID+1; j++){
            acum += matAdj[i][j];
        }
    }
    inputCab->nParesEstacao = acum/2;

}