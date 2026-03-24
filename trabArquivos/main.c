#include <stdio.h>
#include <string.h>

int main() {
    FILE *fileBin; 
    FILE *fileCsv; 

    fileBin = fopen("saida.bin", "wb");
    fileCsv = fopen("estacoes.csv", "r");

    // dados estaticos para o cabeçalho
    char status[1] = '0';
    int topo = -1;
    int proxRRN = 0;
    int nroEstacoes = 0;
    int nroParesEstacao = 0;

    // dados estaticos do registro
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;

    // dados dinamicos (buffers)
    char nomeEstacao[256];
    char nomeLinha[256];

    // dados estaticos para removidos e indicadores de tamanho
    int tamNomeEstacao;
    int tamNomeLinha;
    char removido = '0';
    int proximo = -1;

    // escrevendo em binario os dados estaticos do cabeçalho    
    fwrite(&status, sizeof(char), 1, fileBin);
    fwrite(&topo, sizeof(int), 1, fileBin);
    fwrite(&proxRRN, sizeof(int), 1, fileBin);
    fwrite(&nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&nroParesEstacao, sizeof(int), 1, fileBin);

    //lendo a primeira linha do arquivo csv
    char buffer[1024];
    fgets(buffer, sizeof(buffer), fileCsv);

    while(fgets(buffer, sizeof(buffer), fileCsv) != NULL) {
        // consome os \r e \n para nao dar problema no final da leitura da linha
        buffer[strcspn(buffer, "\r\n")] = '\0';
        // linhaPtr vai funcionar como uma copia de buffer para ler as linhas do .csv
        char *linhaPtr = buffer;
        // o campo funciona para ler cada valor entre as virgulas
        char *campo;
        // coluna 1
        campo = strsep(&linhaPtr, ",");
        codEstacao = atoi(campo);
        // coluna 2
        campo = strsep(&linhaPtr, ",");
        strcpy(nomeEstacao, campo);
        tamNomeEstacao = strlen(nomeEstacao);
        // coluna 3
        campo = strsep(&linhaPtr, ",");
        codLinha = (campo && *campo) ? atoi(campo) : -1;
        // coluna 4
        campo = strsep(&linhaPtr, ",");
        if (campo && *campo) {
            strcpy(nomeLinha, campo);
            tamNomeLinha = strlen(nomeLinha);
        } else {
            tamNomeLinha = 0;
        }
        // coluna 5
        campo = strsep(&linhaPtr, ",");
        codProxEstacao = (campo && *campo) ? atoi(campo) : -1;
        // coluna 6
        campo = strsep(&linhaPtr, ",");
        distProxEstacao = (campo && *campo) ? atoi(campo) : -1;
        // coluna 7
        campo = strsep(&linhaPtr, ",");
        codLinhaIntegra = (campo && *campo) ? atoi(campo) : -1;
        // coluna 8
        campo = strsep(&linhaPtr, ",");
        codEstIntegra = (campo && *campo) ? atoi(campo) : -1;
    }

}