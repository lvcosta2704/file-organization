#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void binarioNaTela(char *nomeArquivoBinario);

// struct que ajuda a contar os pares de estacoes diferentes
typedef struct {
    int origem;
    int destino;
} ParEstacao;


// -------------- FUNCOES ---------------

// criarBin (CREATE TABLE)
// recebe: arquivo csv e arquivo binario
// retorno: void
// funcionalidade: criar um binario a partir de 100 linhas de um arquivo csv
void criarBin(char* csvName, char* binName) {
    FILE *fileCsv = fopen(csvName, "r");  // Abre o CSV apenas para leitura
    FILE *fileBin = fopen(binName, "wb"); // Cria/Abre o Binário para escrita

    if (fileCsv == NULL || fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Vetores para checar duplicatas e atualizar o cabeçalho corretamente
    char nomesVistos[150][256];
    ParEstacao paresVistos[150];

    // Dados estaticos para o cabeçalho
    char status = '0';
    int topo = -1;
    int proxRRN = 0;
    int nroEstacoes = 0;
    int nroParesEstacao = 0;

    // Escrevendo o cabeçalho inconsistente inicialmente
    fwrite(&status, sizeof(char), 1, fileBin);
    fwrite(&topo, sizeof(int), 1, fileBin);
    fwrite(&proxRRN, sizeof(int), 1, fileBin);
    fwrite(&nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&nroParesEstacao, sizeof(int), 1, fileBin);

    // lendo a primeira linha do arquivo csv (cabeçalho) e descartando
    char buffer[1024];
    fgets(buffer, sizeof(buffer), fileCsv);

    // while para capturar os campos do arquivo csv
    // captura feita com a funcao STRSEP
    while(fgets(buffer, sizeof(buffer), fileCsv) != NULL) {
        // consome os \r e \n para nao dar problema no final da leitura da linha
        buffer[strcspn(buffer, "\r\n")] = '\0';
        char *linhaPtr = buffer;
        char *campo;

        // coluna 1
        campo = strsep(&linhaPtr, ",");
        int codEstacao = atoi(campo);

        // coluna 2
        campo = strsep(&linhaPtr, ",");
        char nomeEstacao[256];
        strcpy(nomeEstacao, campo);
        int tamNomeEstacao = strlen(nomeEstacao);

        // coluna 3
        campo = strsep(&linhaPtr, ",");
        int codLinha = (campo && *campo) ? atoi(campo) : -1;

        // coluna 4
        campo = strsep(&linhaPtr, ",");
        char nomeLinha[256] = "";
        int tamNomeLinha = 0;
        if (campo && *campo) {
            strcpy(nomeLinha, campo);
            tamNomeLinha = strlen(nomeLinha);
        }

        // coluna 5
        campo = strsep(&linhaPtr, ",");
        int codProxEstacao = (campo && *campo) ? atoi(campo) : -1;

        // coluna 6
        campo = strsep(&linhaPtr, ",");
        int distProxEstacao = (campo && *campo) ? atoi(campo) : -1;

        // coluna 7
        campo = strsep(&linhaPtr, ",");
        int codLinhaIntegra = (campo && *campo) ? atoi(campo) : -1;

        // coluna 8
        campo = strsep(&linhaPtr, ",");
        int codEstIntegra = (campo && *campo) ? atoi(campo) : -1;


        // --- LOGICA DE CONTAGEM ---
        // Checa se o NOME da estacao ja foi visto
        int estacaoRepetida = 0;
        for (int i = 0; i < nroEstacoes; i++) {
            if (strcmp(nomesVistos[i], nomeEstacao) == 0) {
                estacaoRepetida = 1;
                break;
            }
        }
        if (!estacaoRepetida) {
            strcpy(nomesVistos[nroEstacoes], nomeEstacao);
            nroEstacoes++;
        }

        // Checa se o PAR de estacoes já foi visto
        if (codProxEstacao != -1) {
            int parRepetido = 0;
            for (int i = 0; i < nroParesEstacao; i++) {
                if (paresVistos[i].origem == codEstacao && paresVistos[i].destino == codProxEstacao) {
                    parRepetido = 1;
                    break;
                }
            }
            if (!parRepetido) {
                paresVistos[nroParesEstacao].origem = codEstacao;
                paresVistos[nroParesEstacao].destino = codProxEstacao;
                nroParesEstacao++;
            }
        }

        // --- ESCRITA NO BINÁRIO ---
        // 12 campos no binario para escrever
        char removido = '0';
        int proximo = -1;

        // Campos Fixos
        fwrite(&removido, sizeof(char), 1, fileBin);
        fwrite(&proximo, sizeof(int), 1, fileBin);
        fwrite(&codEstacao, sizeof(int), 1, fileBin);
        fwrite(&codLinha, sizeof(int), 1, fileBin);
        fwrite(&codProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&distProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&codLinhaIntegra, sizeof(int), 1, fileBin);
        fwrite(&codEstIntegra, sizeof(int), 1, fileBin);

        // Campos Variáveis
        fwrite(&tamNomeEstacao, sizeof(int), 1, fileBin);
        if (tamNomeEstacao > 0) {
            fwrite(nomeEstacao, sizeof(char), tamNomeEstacao, fileBin);
        }
        
        fwrite(&tamNomeLinha, sizeof(int), 1, fileBin);
        if (tamNomeLinha > 0) {
            fwrite(nomeLinha, sizeof(char), tamNomeLinha, fileBin); 
        }
        
        // Preenchimento com lixo '$' 
        int bytesEscritos = 1 + (9 * 4) + tamNomeEstacao + tamNomeLinha; // 1 char, 9 ints, 2 variaveis

        char lixo = '$';
        for (int i = bytesEscritos; i < 80; i++) {
            fwrite(&lixo, sizeof(char), 1, fileBin);
        }

        proxRRN++;
    }

    // --- ATUALIZAÇÃO DO CABEÇALHO ---
    status = '1'; // Finalizou, arquivo está consistente
    fseek(fileBin, 0, SEEK_SET); // Volta pro começo do arquivo

    // Reescreve os dados atualizados
    fwrite(&status, sizeof(char), 1, fileBin);
    fwrite(&topo, sizeof(int), 1, fileBin);
    fwrite(&proxRRN, sizeof(int), 1, fileBin);
    fwrite(&nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&nroParesEstacao, sizeof(int), 1, fileBin);

    // Fechamento de arquivos obrigatório antes do binarioNaTela
    fclose(fileCsv);
    fclose(fileBin);

    binarioNaTela(binName);
}
// listarRegistros (SELECT)
// recebe: arquivo binario
// retorno: void
// funcionalidade: buscar e mostrar todos os registros do binario 
void listarRegistros(char *binName) {
    FILE *fileBin = fopen(binName, "rb");

    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo\n");
        return;
    }

    char status;
    fread(&status, sizeof(char), 1, fileBin);
    if (status == '0') {
        printf("Arquivo inconsistente");
        fclose(fileBin);
        return;
    }

    // pula o cabecalho que agora possui 16 bytes, visto que ja lemos 1
    fseek(fileBin, 16, SEEK_CUR);

    int registrosExibidos = 0;
    char removido;

    while (fread(&removido, sizeof(char), 1, fileBin) != NULL)
    {
        if (removido == '1') {
            fseek(fileBin, 79, SEEK_CUR); // pula todos os bytes do registro
            continue; // pula esse registro
        }

        // declarando os campos do registro valido
        int proximo, codEstacao, codLinha, codProxEstacao;
        int distProxEstacao, codLinhaIntegra, codEstIntegra;
        int tamNomeEstacao, tamNomeLinha;
        char nomeEstacao[256] = "";
        char nomeLinha[256] = "";

        // lendo com fread os campos fixos do registro
        fread(&proximo, sizeof(int), 1, fileBin);
        fread(&codEstacao, sizeof(int), 1, fileBin);
        fread(&codLinha, sizeof(int), 1, fileBin);
        fread(&codProxEstacao, sizeof(int), 1, fileBin);
        fread(&distProxEstacao, sizeof(int), 1, fileBin);
        fread(&codLinhaIntegra, sizeof(int), 1, fileBin);
        fread(&codEstIntegra, sizeof(int), 1, fileBin);

        fread(&tamNomeEstacao, sizeof(int), 1, fileBin);
        if (tamNomeEstacao > 0) {
            fread(nomeEstacao, sizeof(char), tamNomeEstacao, fileBin);
            nomeEstacao[tamNomeEstacao] = '\0'; // Finaliza a string para o printf
        }

        fread(&tamNomeLinha, sizeof(int), 1, fileBin);
        if (tamNomeLinha > 0) {
            fread(&nomeLinha, sizeof(char), tamNomeLinha, fileBin);
            nomeLinha[tamNomeLinha] = '\0';
        }

        int bytesLidos = 1 + (9 * 4) + tamNomeEstacao + tamNomeLinha;
        fseek(fileBin, 80 - bytesLidos, SEEK_CUR); // pula o lixo e vai para o prox registro

        // --- PRINTS ---
        // 1. codEstacao
        printf("%d ", codEstacao);

        // 2. nomeEstacao
        if (tamNomeEstacao > 0) printf("%s ", nomeEstacao);
        else printf("NULO ");

        // 3. codLinha
        if (codLinha != -1) printf("%d ", codLinha);
        else printf("NULO ");

        // 4. nomeLinha
        if (tamNomeLinha > 0) printf("%s ", nomeLinha);
        else printf("NULO ");

        // 5. codProxEstacao
        if (codProxEstacao != -1) printf("%d ", codProxEstacao);
        else printf("NULO ");

        // 6. distProxEstacao
        if (distProxEstacao != -1) printf("%d ", distProxEstacao);
        else printf("NULO ");

        // 7. codLinhaIntegra
        if (codLinhaIntegra != -1) printf("%d ", codLinhaIntegra);
        else printf("NULO ");

        // 8. codEstIntegra
        if (codEstIntegra != -1) printf("%d\n", codEstIntegra); // O último campo tem o \n
        else printf("NULO\n");

        registrosExibidos++;
    }

    if (registrosExibidos == 0) {
        printf("registro inexistente");
    }

    fclose(fileBin);

}

void buscarRegistros(char *binName, int N) {
    FILE *fileBin = fopen(binName, "rb");

    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo");
        return;
    }

    for (int i = 0; i < N; i++)
    {
        int m;
        scanf("%d", &m);
        
    }
    
}
int main () {
    int funcionalidade;
    char inputfile[256];
    char outputfile[256];

    if (scanf("%d", &funcionalidade) != 1) return 0;

    switch (funcionalidade) {
        case 1:
            scanf("%s %s", inputfile, outputfile);
            criarBin(inputfile, outputfile);
            break; 
        case 2:
            scanf("%s", inputfile);
            listarRegistros(inputfile);
            break;
        case 3:
            int n;
            scanf("%s %d", inputfile, n);
            buscarRegistros(inputfile, n);
            break;
        default:
            break;
    }
    
    return 0;
}