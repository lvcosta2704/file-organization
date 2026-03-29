#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// struct que ajuda a contar os pares de estacoes diferentes
typedef struct {
    int origem;
    int destino;
} ParEstacao;

// struct para armazenar os campos de busca e compara-los
typedef struct {
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    char nomeEstacao[256];
    char nomeLinha[256];
} Busca;

// struct para armazenar os dados do cabeçalho 
typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
} Cabecalho;

// struct para armazenar os dados do cabeçalho de no maximo 80 bytes 
typedef struct {
    char removido;
    int proximo;
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    int tamNomeEstacao;
    char nomeEstacao[256];
    int tamNomeLinha;
    char nomeLinha[256];
} Registro;

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
    Cabecalho cab;
    cab.status = '0';
    cab.topo = -1;
    cab.proxRRN = 0;
    cab.nroEstacoes = 0;
    cab.nroParesEstacao = 0;

    // Escrevendo o cabeçalho inconsistente inicialmente
    fwrite(&cab.status, sizeof(char), 1, fileBin);
    fwrite(&cab.topo, sizeof(int), 1, fileBin);
    fwrite(&cab.proxRRN, sizeof(int), 1, fileBin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, fileBin);

    // lendo a primeira linha do arquivo csv (cabeçalho) e descartando
    char buffer[1024];
    fgets(buffer, sizeof(buffer), fileCsv);

    // while para capturar os campos do arquivo csv
    // captura feita com a funcao STRSEP
    while(fgets(buffer, sizeof(buffer), fileCsv) != NULL) {
        Registro reg;
        
        // consome os \r e \n para nao dar problema no final da leitura da linha
        buffer[strcspn(buffer, "\r\n")] = '\0';
        char *linhaPtr = buffer;
        char *campo;

        // coluna 1
        campo = strsep(&linhaPtr, ",");
        reg.codEstacao = atoi(campo);

        // coluna 2
        campo = strsep(&linhaPtr, ",");
        strcpy(reg.nomeEstacao, campo);
        reg.tamNomeEstacao = strlen(reg.nomeEstacao);

        // coluna 3
        campo = strsep(&linhaPtr, ",");
        reg.codLinha = (campo && *campo) ? atoi(campo) : -1;

        // coluna 4
        campo = strsep(&linhaPtr, ",");
        strcpy(reg.nomeLinha, "");
        reg.tamNomeLinha = 0;
        if (campo && *campo) {
            strcpy(reg.nomeLinha, campo);
            reg.tamNomeLinha = strlen(reg.nomeLinha);
        }

        // coluna 5
        campo = strsep(&linhaPtr, ",");
        reg.codProxEstacao = (campo && *campo) ? atoi(campo) : -1;

        // coluna 6
        campo = strsep(&linhaPtr, ",");
        reg.distProxEstacao = (campo && *campo) ? atoi(campo) : -1;

        // coluna 7
        campo = strsep(&linhaPtr, ",");
        reg.codLinhaIntegra = (campo && *campo) ? atoi(campo) : -1;

        // coluna 8
        campo = strsep(&linhaPtr, ",");
        reg.codEstIntegra = (campo && *campo) ? atoi(campo) : -1;


        // --- LOGICA DE CONTAGEM ---
        // Checa se a estação já foi vista pelo NOME (regra do PDF)
        int estacaoRepetida = 0;
        for (int i = 0; i < cab.nroEstacoes; i++) {
            if (strcmp(nomesVistos[i], reg.nomeEstacao) == 0) {
                estacaoRepetida = 1;
                break;
            }
        }
        if (!estacaoRepetida) {
            strcpy(nomesVistos[cab.nroEstacoes], reg.nomeEstacao);
            cab.nroEstacoes++;
        }

        // Checa se o par já foi visto
        if (reg.codProxEstacao != -1) {
            int parRepetido = 0;
            for (int i = 0; i < cab.nroParesEstacao; i++) {
                if (paresVistos[i].origem == reg.codEstacao && paresVistos[i].destino == reg.codProxEstacao) {
                    parRepetido = 1;
                    break;
                }
            }
            if (!parRepetido) {
                paresVistos[cab.nroParesEstacao].origem = reg.codEstacao;
                paresVistos[cab.nroParesEstacao].destino = reg.codProxEstacao;
                cab.nroParesEstacao++;
            }
        }

        // --- ESCRITA NO BINÁRIO ---
        // 12 campos no binario para escrever
        reg.removido = '0';
        reg.proximo = -1;

        // Campos Fixos
        fwrite(&reg.removido, sizeof(char), 1, fileBin);
        fwrite(&reg.proximo, sizeof(int), 1, fileBin);
        fwrite(&reg.codEstacao, sizeof(int), 1, fileBin);
        fwrite(&reg.codLinha, sizeof(int), 1, fileBin);
        fwrite(&reg.codProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&reg.distProxEstacao, sizeof(int), 1, fileBin);
        fwrite(&reg.codLinhaIntegra, sizeof(int), 1, fileBin);
        fwrite(&reg.codEstIntegra, sizeof(int), 1, fileBin);

        // Campos Variáveis
        fwrite(&reg.tamNomeEstacao, sizeof(int), 1, fileBin);
        if (reg.tamNomeEstacao > 0) {
            fwrite(reg.nomeEstacao, sizeof(char), reg.tamNomeEstacao, fileBin);
        }
        
        fwrite(&reg.tamNomeLinha, sizeof(int), 1, fileBin);
        if (reg.tamNomeLinha > 0) {
            fwrite(reg.nomeLinha, sizeof(char), reg.tamNomeLinha, fileBin); 
        }
        
        // Preenchimento com lixo '$' 
        int bytesEscritos = 1 + (9 * 4) + reg.tamNomeEstacao + reg.tamNomeLinha; // 1 char, 9 ints, 2 variaveis

        char lixo = '$';
        for (int i = bytesEscritos; i < 80; i++) {
            fwrite(&lixo, sizeof(char), 1, fileBin);
        }

        cab.proxRRN++;
    }

    // --- ATUALIZAÇÃO DO CABEÇALHO ---
    cab.status = '1'; // Finalizou, arquivo está consistente
    fseek(fileBin, 0, SEEK_SET); // Volta pro começo do arquivo

    // Reescreve os dados atualizados
    fwrite(&cab.status, sizeof(char), 1, fileBin);
    fwrite(&cab.topo, sizeof(int), 1, fileBin);
    fwrite(&cab.proxRRN, sizeof(int), 1, fileBin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, fileBin);

    // Fechamento de arquivos obrigatório antes do binarioNaTela
    fclose(fileCsv);
    fclose(fileBin);

}

// listarRegistros (SELECT)
// recebe: arquivo binario
// retorno: void
// funcionalidade: buscar e mostrar todos os registros do binario 
void listarRegistros(char *binName) {
    FILE *fileBin = fopen(binName, "rb");

    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;

    fread(&cab.status, sizeof(char), 1, fileBin);
    if (cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    // pula o cabecalho que agora possui 16 bytes, visto que ja lemos 1
    fseek(fileBin, 16, SEEK_CUR);

    int registrosExibidos = 0;
    
    // declarando os campos do registro valido
    Registro reg;

    while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1) {
        if (reg.removido == '1') {
            fseek(fileBin, 79, SEEK_CUR); // pula todos os bytes do registro
            continue; // pula esse registro
        }

        // lendo com fread os campos fixos do registro
        fread(&reg.proximo, sizeof(int), 1, fileBin);
        fread(&reg.codEstacao, sizeof(int), 1, fileBin);
        fread(&reg.codLinha, sizeof(int), 1, fileBin);
        fread(&reg.codProxEstacao, sizeof(int), 1, fileBin);
        fread(&reg.distProxEstacao, sizeof(int), 1, fileBin);
        fread(&reg.codLinhaIntegra, sizeof(int), 1, fileBin);
        fread(&reg.codEstIntegra, sizeof(int), 1, fileBin);

        fread(&reg.tamNomeEstacao, sizeof(int), 1, fileBin);
        if (reg.tamNomeEstacao > 0) {
            fread(reg.nomeEstacao, sizeof(char), reg.tamNomeEstacao, fileBin);
            reg.nomeEstacao[reg.tamNomeEstacao] = '\0'; // Finaliza a string para o printf
        }

        fread(&reg.tamNomeLinha, sizeof(int), 1, fileBin);
        if (reg.tamNomeLinha > 0) {
            fread(reg.nomeLinha, sizeof(char), reg.tamNomeLinha, fileBin);
            reg.nomeLinha[reg.tamNomeLinha] = '\0';
        }

        int bytesLidos = 1 + (9 * 4) + reg.tamNomeEstacao + reg.tamNomeLinha;
        fseek(fileBin, 80 - bytesLidos, SEEK_CUR); // pula o lixo e vai para o prox registro

        // --- PRINTS ---
        // 1. codEstacao
        printf("%d ", reg.codEstacao);

        // 2. nomeEstacao
        if (reg.tamNomeEstacao > 0) printf("%s ", reg.nomeEstacao);
        else printf("NULO ");

        // 3. codLinha
        if (reg.codLinha != -1) printf("%d ", reg.codLinha);
        else printf("NULO ");

        // 4. nomeLinha
        if (reg.tamNomeLinha > 0) printf("%s ", reg.nomeLinha);
        else printf("NULO ");

        // 5. codProxEstacao
        if (reg.codProxEstacao != -1) printf("%d ", reg.codProxEstacao);
        else printf("NULO ");

        // 6. distProxEstacao
        if (reg.distProxEstacao != -1) printf("%d ", reg.distProxEstacao);
        else printf("NULO ");

        // 7. codLinhaIntegra
        if (reg.codLinhaIntegra != -1) printf("%d ", reg.codLinhaIntegra);
        else printf("NULO ");

        // 8. codEstIntegra
        if (reg.codEstIntegra != -1) printf("%d\n", reg.codEstIntegra); // O último campo tem o \n
        else printf("NULO\n");

        registrosExibidos++;
    }

    if (registrosExibidos == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(fileBin);
}

// buscarRegistros (SELECT)
// recebe: arquivo binario e N = numeros de buscas
// retorno: void
// funcionalidade: buscar e mostrar registros filtrados
void buscarRegistros(char *binName, int N) {
    FILE *fileBin = fopen(binName, "rb");
    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    char status;
    fread(&status, sizeof(char), 1, fileBin);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(fileBin);
        return;
    }

    Busca buscando; 

    for (int i = 0; i < N; i++){
        int m;
        scanf("%d", &m);

        for (int j = 0; j < m; j++){
            char nomeCampo[50];
            scanf("%s", nomeCampo);

            if (strcmp(nomeCampo, "codEstacao") == 0) {
                char valorCampo[50];
                scanf("%s", valorCampo);
                if (strcmp(valorCampo, "NULO") == 0) buscando.codEstacao = -1; 

            }
            else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
                char valorCampo[50];
                scanf("%s", valorCampo);
                if (strcmp(valorCampo, "NULO") == 0) strcpy(buscando.nomeEstacao, "NULO");
            }

    }


    

    
    // Lógica da Funcionalidade 3 entrará aqui!
    
    fclose(fileBin);
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
        case 3: {
            int n;
            scanf("%s %d", inputfile, &n);
            buscarRegistros(inputfile, n);
            break;
        }
        default:
            break;
    }
    
    return 0;
}