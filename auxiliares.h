// nesse header:
// funções auxiliares, como as fornecidas pelos monitores, a função de hash, e quaisquer outras que fomos criar que não se encaixam em outros lugares
//------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef AUX_H
#define AUX_H

//converte uma string pra um int usando o hash FNV-1a
int fnv1a_hash(const char* str);

//fornecidas pelos monitores
void BinarioNaTela(char *arquivo);
void ScanQuoteString(char *str);

#endif