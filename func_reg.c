#include "func_reg.h"

//Eu uso fseek aqui pq de qualquer forma o cabecalho vai estar no comeco no arquivo
void REGCab_escrever(FILE* outputBIN, regCabecalho* pRegCab){
    fseek(outputBIN, 0, SEEK_SET);
    fwrite(&(pRegCab->status), sizeof(pRegCab->status), 1, outputBIN);  //off 0
    fwrite(&(pRegCab->topo), sizeof(pRegCab->topo), 1, outputBIN);      //off 1
    fwrite(&(pRegCab->proxRRN), sizeof(pRegCab->proxRRN), 1, outputBIN);    //off 5
    fwrite(&(pRegCab->nEstacoes), sizeof(pRegCab->nEstacoes), 1, outputBIN);    //off 9
    fwrite(&(pRegCab->nParesEstacao), sizeof(pRegCab->nParesEstacao), 1, outputBIN);    //off 13
}


void REGDados_escrever(FILE* outputBIN, regDados* pRegDados){
    fwrite(&(pRegDados->removido), sizeof(pRegDados->removido), 1, outputBIN); //off 0
    fwrite(&(pRegDados->proxRRN), sizeof(pRegDados->proxRRN), 1, outputBIN);    //off 1
    fwrite(&(pRegDados->codEstacao), sizeof(pRegDados->codEstacao), 1, outputBIN);  //off 5
    fwrite(&(pRegDados->codLinha), sizeof(pRegDados->codLinha), 1, outputBIN);      //off 9
    fwrite(&(pRegDados->codProxEstacao), sizeof(pRegDados->codProxEstacao), 1, outputBIN);  //off 13
    fwrite(&(pRegDados->distProxEstacao), sizeof(pRegDados->distProxEstacao), 1, outputBIN);    //off 17
    fwrite(&(pRegDados->codLinhaIntegra), sizeof(pRegDados->codLinhaIntegra), 1, outputBIN);    //off 21
    fwrite(&(pRegDados->codEstIntegra), sizeof(pRegDados->codEstIntegra), 1, outputBIN);        //off 25
    fwrite(&(pRegDados->tamNomeEstacao), sizeof(pRegDados->tamNomeEstacao), 1, outputBIN);      //off 29
    fwrite(pRegDados->nomeEstacao, sizeof(char), pRegDados->tamNomeEstacao, outputBIN);         //varzea daqui para baixo
    fwrite(&(pRegDados->tamNomeLinha), sizeof(pRegDados->tamNomeLinha), 1, outputBIN);
    fwrite(pRegDados->nomeLinha, sizeof(char), pRegDados->tamNomeLinha, outputBIN);


    //Preencher com lixo
    const char caracterLixo = '$';
    int tamanho_total = 0;
    tamanho_total = sizeof(int) * 9 + sizeof(char) + pRegDados->tamNomeLinha + pRegDados->tamNomeEstacao;
    for(int i=0; i<80-tamanho_total; i++){
        fwrite(&caracterLixo, sizeof(char), 1, outputBIN);
    }

}

//eu estou usando ponteiro para cabecalho para parear o cabecalho no arquivo e na memoria
void REG_criarBIN(FILE* outputBIN, regCabecalho* pRegCab, regDados** vetorRegDados, int TamanhoVetor){
    REGCab_escrever(outputBIN, pRegCab);
    for(int i = 0; i<TamanhoVetor; i++){
        REGDados_escrever(outputBIN, vetorRegDados[i]);
    }
    //agora que eu terminei de escrever eu ajusto o status
    fseek(outputBIN, 0, SEEK_SET);
    const int status = 1;
    fwrite(&status, sizeof(char), 1, outputBIN);
    pRegCab->status = 1;
}