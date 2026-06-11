// nesse header:
// funções auxiliares, como as fornecidas pelos monitores, a função de hash, e quaisquer outras que fomos criar que não se encaixam em outros lugares
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef AUX_H
#define AUX_H

//converte uma string pra um int usando o hash FNV-1a
int fnv1a_hash(const char* str);

//converte um rrn pra byte offset da arvore B, e vice-versa
int BTree_RRN2BYTE(int RRN);
int BTree_BYTE2RRN(int BYTE);

//converte um rrn pra byte offset do arquivo de dados, e vice-versa
int Data_RRN2BYTE(int RRN);
int Data_BYTE2RRN(int BYTE);

//fornecidas pelos monitores
void BinarioNaTela(char *arquivo);
void ScanQuoteString(char *str);

#endif