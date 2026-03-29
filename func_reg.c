#include "func_reg.h"

// Eu uso fseek aqui pq de qualquer forma o cabecalho vai estar no comeco no arquivo
void REGCab_escrever(FILE *outputBIN, regCabecalho *pRegCab)
{
    fseek(outputBIN, 0, SEEK_SET);
    fwrite(&(pRegCab->status), sizeof(char), 1, outputBIN);       // off 0
    fwrite(&(pRegCab->topo), sizeof(int), 1, outputBIN);          // off 1
    fwrite(&(pRegCab->proxRRN), sizeof(int), 1, outputBIN);       // off 5
    fwrite(&(pRegCab->nEstacoes), sizeof(int), 1, outputBIN);     // off 9
    fwrite(&(pRegCab->nParesEstacao), sizeof(int), 1, outputBIN); // off 13
}

void REGDados_escrever(FILE *outputBIN, regDados *pRegDados)
{
    fwrite(&(pRegDados->removido), sizeof(char), 1, outputBIN);                         // off 0
    fwrite(&(pRegDados->proxRRN), sizeof(int), 1, outputBIN);                           // off 1
    fwrite(&(pRegDados->codEstacao), sizeof(int), 1, outputBIN);                        // off 5
    fwrite(&(pRegDados->codLinha), sizeof(int), 1, outputBIN);                          // off 9
    fwrite(&(pRegDados->codProxEstacao), sizeof(int), 1, outputBIN);                    // off 13
    fwrite(&(pRegDados->distProxEstacao), sizeof(int), 1, outputBIN);                   // off 17
    fwrite(&(pRegDados->codLinhaIntegra), sizeof(int), 1, outputBIN);                   // off 21
    fwrite(&(pRegDados->codEstIntegra), sizeof(int), 1, outputBIN);                     // off 25
    fwrite(&(pRegDados->tamNomeEstacao), sizeof(int), 1, outputBIN);                    // off 29
    fwrite(pRegDados->nomeEstacao, sizeof(char), pRegDados->tamNomeEstacao, outputBIN); // varzea daqui para baixo
    fwrite(&(pRegDados->tamNomeLinha), sizeof(int), 1, outputBIN);
    fwrite(pRegDados->nomeLinha, sizeof(char), pRegDados->tamNomeLinha, outputBIN);

    // Preencher com lixo
    const char caracterLixo = '$';
    int tamanho_total = 0;
    tamanho_total = sizeof(int) * 9 + sizeof(char) + pRegDados->tamNomeLinha + pRegDados->tamNomeEstacao;
    for (int i = 0; i < 80 - tamanho_total; i++)
    {
        fwrite(&caracterLixo, sizeof(char), 1, outputBIN);
    }
}

void REGCab_ler(FILE *BIN, regCabecalho *pRegCab)
{
    fseek(BIN, 0, SEEK_SET);
    fread(&(pRegCab->status), sizeof(char), 1, BIN);
    fread(&(pRegCab->topo), sizeof(int), 1, BIN);
    fread(&(pRegCab->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegCab->nEstacoes), sizeof(int), 1, BIN);
    fread(&(pRegCab->nParesEstacao), sizeof(int), 1, BIN);
}

// Retorna -1 se chegou no fim do arquivo
// Retorna 0 se tentou ler registro removido
// retorna 1 se deu certo
int REGDados_ler(FILE *BIN, regDados *pRegDados)
{

    // tratar casos de fim de arquivo e registro removido
    char verificarFimDoArquivo = fread(&(pRegDados->removido), sizeof(char), 1, BIN);
    if (verificarFimDoArquivo != 1)
        return -1;
    if (pRegDados->removido == 1)
    {
        fseek(BIN, 79, SEEK_CUR); // pulo os 79 bytes restantes do registro removido
        return 0;
    }

    fread(&(pRegDados->proxRRN), sizeof(int), 1, BIN);
    fread(&(pRegDados->codEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->codLinha), sizeof(int), 1, BIN);
    fread(&(pRegDados->codProxEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->distProxEstacao), sizeof(int), 1, BIN);
    fread(&(pRegDados->codLinhaIntegra), sizeof(int), 1, BIN);
    fread(&(pRegDados->codEstIntegra), sizeof(int), 1, BIN);

    char bufferNome[128];

    // Nome da estacao
    // Ficou um pouco horroroso pq eu preciso previnir memory leak que aconteceria se eu só reescrevesse o ponteiro em nomeEstacao
    // Sem desalocar a string que ja exisita nele
    fread(&(pRegDados->tamNomeEstacao), sizeof(int), 1, BIN);
    if (pRegDados->tamNomeEstacao > 0)
    {
        fread(bufferNome, sizeof(char), pRegDados->tamNomeEstacao, BIN);
        bufferNome[pRegDados->tamNomeEstacao] = '\0';
        if (pRegDados->nomeEstacao != NULL)
            free(pRegDados->nomeEstacao);
        pRegDados->nomeEstacao = strdup(bufferNome);
    }
    else
    {
        if (pRegDados->nomeEstacao != NULL)
            free(pRegDados->nomeEstacao);
        pRegDados->nomeEstacao = strdup("");
    }

    // Nome da linha
    fread(&(pRegDados->tamNomeLinha), sizeof(int), 1, BIN);
    if (pRegDados->tamNomeLinha > 0)
    {
        fread(bufferNome, sizeof(char), pRegDados->tamNomeLinha, BIN);
        bufferNome[pRegDados->tamNomeLinha] = '\0';
        if (pRegDados->nomeLinha != NULL)
            free(pRegDados->nomeLinha);
        pRegDados->nomeLinha = strdup(bufferNome);
    }
    else
    {
        if (pRegDados->nomeLinha != NULL)
            free(pRegDados->nomeLinha);
        pRegDados->nomeLinha = strdup("");
    }

    // Compensar o lixo no resto do registro
    int tamanho = 1 + sizeof(int) * 9 + pRegDados->tamNomeEstacao + pRegDados->tamNomeLinha;
    if ((80 - tamanho) > 0)
    {
        char descarta[80];
        fread(descarta, sizeof(char), 80 - tamanho, BIN);
    }

    return 1;
}

// PRIMEIRA FUNCIONALIDADE
// eu estou usando ponteiro para cabecalho para parear o cabecalho no arquivo e na memoria
void REG_criarBIN(FILE *outputBIN, regCabecalho *pRegCab, regDados **vetorRegDados, int TamanhoVetor)
{
    REGCab_escrever(outputBIN, pRegCab);
    for (int i = 0; i < TamanhoVetor; i++)
    {
        REGDados_escrever(outputBIN, vetorRegDados[i]);
    }
    // agora que eu terminei de escrever eu ajusto o status
    fseek(outputBIN, 0, SEEK_SET);
    const char status = 1;
    fwrite(&status, sizeof(char), 1, outputBIN);

    pRegCab->status = 1; // parear cabecalho memoria e no disco
}

// funcoes auxiliares para o printDados nao ficar gigante
void REGDados_printInt(int input)
{
    char nulo[] = "NULO";
    if (input == -1)
        printf("%s ", nulo);
    else
        printf("%d ", input);
}

void REGDados_printString(char *string)
{
    char nulo[] = "NULO";
    if (strcmp(string, "") == 0)
        printf("%s ", nulo);
    else
        printf("%s ", string);
}

void REGDados_printDados(regDados registro)
{
    REGDados_printInt(registro.codEstacao);
    REGDados_printString(registro.nomeEstacao);
    REGDados_printInt(registro.codLinha);
    REGDados_printString(registro.nomeLinha);
    REGDados_printInt(registro.codProxEstacao);
    REGDados_printInt(registro.distProxEstacao);
    REGDados_printInt(registro.codLinhaIntegra);
    REGDados_printInt(registro.codEstIntegra);
    printf("\n");
}

// SEGUNDA FUNCIONALIDADE
void REGDados_printBIN(FILE *BIN)
{
    regCabecalho tempCab;
    regDados tempDados;

    tempDados.nomeEstacao = NULL;
    tempDados.nomeLinha = NULL;
    REGCab_ler(BIN, &tempCab);

    if (tempCab.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    char leuUm = 0;
    int flag = 1;
    while (flag != -1)
    {
        flag = REGDados_ler(BIN, &tempDados);
        if (flag == 1)
        {
            REGDados_printDados(tempDados);
            leuUm = 1;
        }
        // Se flag == 0 significa que tentou ler um registro removido
        // entao nao faz nada e continua
    }

    // Verificar se leu nenhum registro
    if (leuUm == 0)
    {
        printf("Registro inexistente.\n");
    }

    free(tempDados.nomeEstacao);
    free(tempDados.nomeLinha);
}

// Funcao auxiliar para a buscaReg
// 1 se passou, 0 se não passou
// Que funçao feia meu deus
// Valor nulo de string dentro do criterio é "" igual ao valor nulo de string na memoria
char REGDados_testeCriterio(regDados registro, criterio teste)
{

    if (strcmp(teste.nomeCampo, "codEstacao") == 0)
    {
        return registro.codEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "nomeEstacao") == 0)
    {
        return strcmp(registro.nomeEstacao, teste.valorString) == 0;
    }

    if (strcmp(teste.nomeCampo, "codLinha") == 0)
    {
        return registro.codLinha == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "nomeLinha") == 0)
    {
        return strcmp(registro.nomeLinha, teste.valorString) == 0;
    }

    if (strcmp(teste.nomeCampo, "codProxEstacao") == 0)
    {
        return registro.codProxEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "distProxEstacao") == 0)
    {
        return registro.distProxEstacao == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "codLinhaIntegra") == 0)
    {
        return registro.codLinhaIntegra == teste.valorInt;
    }

    if (strcmp(teste.nomeCampo, "codEstIntegra") == 0)
    {
        return registro.codEstIntegra == teste.valorInt;
    }
}

// Procura por um registro que atende todos os criterios e retorna a posicao dele no bin
// e copia o conteudo dele para o outputReg na memoria
// Começa a procurar a partir da posicao atual do ponteiro do arquivo e move ele para frente
// tambem supoe que ja tenha passado pelo cabecalho
// retorna -1 se nao achou nada(fim de arquivo)
int REGDados_buscaReg(FILE *BIN, regDados *outputReg, int m, criterio testes[])
{
    int achou = 0;
    while (achou == 0)
    {
        int flag = REGDados_ler(BIN, outputReg);
        switch (flag)
        {
        case -1: // fim de arquivo
            return -1;
        case 0: // Achou um registro removido
            continue;
        case 1: // leu um registro, testar criterios
            char falhou = 0;

            for (int i = 0; i < m; i++)
            {
                if (REGDados_testeCriterio(*outputReg, testes[i]) == 0)
                {
                    falhou = 1;
                    break;
                }
            }

            if (falhou == 0)
                achou = 1;
            break;
        }
    }
    return ftell(BIN) - 80; // Eu passei pelo registro que eu queria por causa dos freads, entao tenho que compensar voltando 80 bytes
}

// Essa funcao eu fiz direto com o input do terminal pq eu nao sei como separar a parte do print e da busca sem enlouquecer com alocacao dinamica
void REGDados_printComInputDeCriterios(FILE *BIN)
{

    regCabecalho tempCab;

    REGCab_ler(BIN, &tempCab);

    if (tempCab.status == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    int n = 0;
    scanf("%d", &n);

    // Esse for itera pelas diferentes buscas
    for (int i = 0; i < n; i++)
    {
        fseek(BIN, 17, SEEK_SET); // volto para o comeco depois do cabecalho

        int m;
        scanf("%d", &m); // Quantidade de filtros para busca

        criterio *testes = (criterio *)malloc(m * sizeof(criterio));

        // For para ler os criterios e colocar eles no vetor
        for (int j = 0; j < m; j++)
        {
            scanf("%s", testes[j].nomeCampo);

            if (strcmp(testes[j].nomeCampo, "nomeEstacao") == 0 ||
                strcmp(testes[j].nomeCampo, "nomeLinha") == 0)
            {
                scanf("%s", testes[j].valorString); // TEM QUE TROCAR ESSE SCANF PELO SCANF_QUOTE SLA OQ QUE OS MONITORES VAO PASSAR DEPOIS
                if (strcmp(testes[j].valorString, "NULO") == 0)
                    strcpy(testes[j].valorString, "");
            }
            else
            {
                char temp[128];
                scanf("%s", temp);
                if (strcmp(temp, "NULO") == 0)
                    testes[j].valorInt = -1;
                else
                    testes[j].valorInt = atoi(temp);
            }
        }

        // A partir daqui eu tenho todos os testes prontos e posso começar a buscar
        regDados tempReg;
        tempReg.nomeEstacao = NULL;
        tempReg.nomeLinha = NULL;

        int encontrouAlgum = 0;
        int flag;

        while ((flag = REGDados_ler(BIN, &tempReg)) != -1)
        {
            if (flag == 1) // achou um registro valido
            {
                int passouTodos = 1;
                for (int k = 0; k < m; k++)
                {
                    if (REGDados_testeCriterio(tempReg, testes[k]) == 0)
                    {
                        passouTodos = 0;
                        break;
                    }
                }

                if (passouTodos)
                {
                    REGDados_printDados(tempReg);
                    encontrouAlgum = 1;
                }
            }
        }

        // Liberar a memoria que esta no tempReg
        if (tempReg.nomeEstacao != NULL)
            free(tempReg.nomeEstacao);
        if (tempReg.nomeLinha != NULL)
            free(tempReg.nomeLinha);

        free(testes);

        if (!encontrouAlgum)
        {
            printf("Registro inexistente.\n");
        }
    }

    return;
}