#include "auxiliares.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//converter uma string pra um int usando o hash FNV-1a
int fnv1a_hash(const char* str) {
    int hash = 2166136261; // FNV offset basis
    while (*str) {
        hash ^= *str++;
        hash *= 16777619; // FNV prime
    }
    return hash;
}

//converte um rrn pra byte offset da arvore B, e vice-versa
int BTree_RRN2BYTE(int RRN){
    return 17+53*RRN;
}
int BTree_BYTE2RRN(int BYTE){
    return (BYTE-17)/53;
}

//converte um rrn pra byte offset do arquivo de dados, e vice-versa
int Data_RRN2BYTE(int RRN){
    return 17+80*RRN;
}
int Data_BYTE2RRN(int BYTE){
    return (BYTE-17)/80;
}


//fornecida
void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

//fornecida
void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}
