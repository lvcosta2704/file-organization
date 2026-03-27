#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        char campo[256];
        int pos = 0;
        int i;


        // coluna 1
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        codEstacao = atoi(campo);
        pos++;


        // coluna 2
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        strcpy(nomeEstacao, campo);
        tamNomeEstacao = strlen(campo);
        pos++;


        // coluna 3
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            codLinha = atoi(campo);
        } else {
            codLinha = -1;
        }
        pos++;


        // coluna 4
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            strcpy(nomeLinha, campo);
            tamNomeLinha = strlen(campo);
        } else {
            tamNomeLinha = 0;
        }
        pos++;


        // coluna 5
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            codProxEstacao = atoi(campo);
        } else {
            codProxEstacao = -1;
        }
        pos++;


        // coluna 6
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            distProxEstacao = atoi(campo);
        } else {
            distProxEstacao = -1;
        }
        pos++;


        // coluna 7
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            codLinhaIntegra = atoi(campo);
        } else {
            codLinhaIntegra = -1;
        }
        pos++;


        // coluna 8
        i = 0;
        while (buffer[pos] != ',' && buffer[pos] != '\0')
        {
            campo[i++] = buffer[pos++];
        }
        campo[i] = '\0';
        if (strlen(campo) > 0) {
            codEstIntegra = atoi(campo);
        } else {
            codEstIntegra = -1;
        }
        pos++;

        fwrite(&removido, sizeof(char), 1, fileBin);
        fwrite(&proximo, sizeof(int), 1, fileBin);

        fwrite(&codEstacao, sizeof(int), 1, fileBin);
        fwrite(&codLinha, sizeof(int), 1, fileBin);
        fwrite(&codProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&distProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&codLinhaIntegra, sizeof(int), 1, fileBin);
        fwrite(&codEstIntegra, sizeof(int), 1, fileBin);

        fwrite(&tamNomeEstacao, sizeof(int), 1, fileBin);
        if (tamNomeEstacao > 0) {
            fwrite(nomeEstacao, sizeof(char), tamNomeEstacao, fileBin);
        }
        
        fwrite(&tamNomeLinha, sizeof(int), 1, fileBin);
        if (tamNomeLinha > 0) {
            fwrite(nomeLinha, sizeof(char), tamNomeLinha, fileBin); 
        }
        
        int bytesEscritos = 1 + 4 + (6 * 4) + 4 + tamNomeEstacao + 4 + tamNomeLinha;

        char lixo = '$';
        for (int i = bytesEscritos; i < 80; i++) {
            fwrite(&lixo, sizeof(char), 1, fileBin);
        }

        proxRRN++;
    }

}