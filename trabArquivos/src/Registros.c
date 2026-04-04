#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Registros.h"
#include "fornecidas.h"

// Definicao de macros
#define MAX_REGISTROS 250
#define TAM_CABECALHO 17
#define TAM_REGISTRO 80

// struct que ajuda a contar os pares de estacoes diferentes
struct parestacao{
    int origem;
    int destino;
};

// struct para armazenar os campos de busca e compara-los
struct busca{
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    char nomeEstacao[256];
    char nomeLinha[256];
};

// struct para armazenar os dados do cabeçalho 
struct cabecalho{
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
};

// struct para armazenar os dados do cabeçalho de no maximo 80 bytes 
struct registro{
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
};

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
    char nomesVistos[MAX_REGISTROS][256];
    ParEstacao paresVistos[MAX_REGISTROS];

    // Dados estaticos para o cabeçalho
    Cabecalho cab = iniciarCabecalho();

    // Escrevendo o cabeçalho inconsistente inicialmente
    escreverCabecalho(fileBin, cab);

    // lendo a primeira linha do arquivo csv (cabeçalho) e descartando
    char buffer[512];
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
        escreverRegistro(fileBin, reg);

        cab.proxRRN++;
    }

    // --- ATUALIZAÇÃO DO CABEÇALHO ---
    cab.status = '1'; // Finalizou, arquivo está consistente
    escreverCabecalho(fileBin, cab); // Atualiza o cabecalho

    // Fechamento de arquivos obrigatório antes do binarioNaTela
    fclose(fileCsv);
    fclose(fileBin);

    BinarioNaTela(binName);
}

// listarRegistros (SELECT FROM -)
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

    // Lê o cabecalho do arquivo e verifica se está consistente
    lerCabecalho(fileBin, &cab);
    if(cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    int registrosExibidos = 0;
    
    // declarando os campos do registro valido
    Registro reg;

    while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1) {
        if (reg.removido == '1') {
            fseek(fileBin, 79, SEEK_CUR); // pula todos os bytes do registro
            continue; // pula esse registro
        }
        // chama a funcao lerRegistro que ira fazer todos os fread necessarios 
        // alem de pular o lixo que tem no registro
        lerRegistro(fileBin, &reg);

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

// buscarRegistros (SELECT FROM - WHERE -)
// recebe: arquivo binario e N = numeros de buscas
// retorno: void
// funcionalidade: buscar e mostrar registros filtrados
void buscarRegistros(char *binName, int N) {
    // Abre o arquivo no modo leitura e verifica se ocorreu bem
    FILE *fileBin = fopen(binName, "rb");
    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;
    
    // Lê o cabecalho do arquivo e verifica se está consistente
    lerCabecalho(fileBin, &cab);
    if(cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    for (int i = 0; i < N; i++){ // Executa N vezes
        Busca filtro = filtrarRegistro(); // Cria um filtro para a busca (Simula o WHERE)

        fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Coloca a referência para o arquivo no primeiro registro de dados

        int encontrouAlgum = 0;
        Registro reg;

        // Passa por cada registro de dados sequencialmente
        // verificando se ele está removido a principio
        while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
            // Se estiver removido pula para o proximo
            if (reg.removido == '1') {
                fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
                continue;
            }
            // --- LEITURA DOS REGISTROS ---
            // Lê o restante do registro para comparar
            // Lê o registro com os freads e tambem pula o lixo do registro no final
            
            lerRegistro(fileBin, &reg);

            int coincide = comparaFiltro(filtro, reg);

            if (coincide) {
            // Imprime usando a mesma lógica da listarRegistros, porem se tiver -1 imprime NULO
                // 1. codEstacao
                if (reg.codEstacao != -1) printf("%d ", reg.codEstacao); 
                else printf("NULO ");
                
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
                
                // 8. codEstIntegra (Último campo tem a quebra de linha \n)
                if (reg.codEstIntegra != -1) printf("%d\n", reg.codEstIntegra); 
                else printf("NULO\n");

                encontrouAlgum = 1;
            }

        }
        if (!encontrouAlgum) {
            printf("Registro inexistente.\n");
        }

        printf("\n");
    }
    fclose(fileBin);
}

// removerRegistros (DELETE FROM - WHERE -)
// recebe: arquivo binario e N = numeros de remocoes
// retorno: void
// funcionalidade: remover registros filtrados baseado na abordagem dinamica
void removerRegistros(char *binName, int N) {
    // Abre o arquivo no modo leitura e verifica se ocorreu bem
    FILE *fileBin = fopen(binName, "rb+");
    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;

    // Lê o cabecalho do arquivo e verifica se está consistente
    lerCabecalho(fileBin, &cab);
    if(cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    // Marca o arquivo como inconsistente
    cab.status = '0';
    escreverCabecalho(fileBin, cab);

    for (int i = 0; i < N; i++) { // Executa N vezes
        Busca filtro = filtrarRegistro(); // Cria um filtro para a busca (Simula o WHERE)

        fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Coloca o fseek no primeiro registro de dados

        int encontrouAlgum = 0;
        Registro reg;

        int RRN = 0; // Inicializa o RRN como zero

        // Passa por cada registro de dados sequencialmente
        // verificando se ele está removido a principio
        while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
            // Se estiver removido pula para o proximo
            if (reg.removido == '1') {
                fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
                RRN++; // Atualiza o valor do RRN
                continue;
            }
            // --- LEITURA DOS REGISTROS ---
            // Lê o restante do registro para comparar
            // Lê o registro com os freads e tambem pula o lixo do registro no final
            
            lerRegistro(fileBin, &reg);

            int coincide = comparaFiltro(filtro, reg);

            if(coincide) {
                encontrouAlgum = 1;

                apagarRegistro(fileBin, &reg, &cab, RRN);

                // Poe o fseek no próximo RRN
                fseek(fileBin, TAM_CABECALHO + (RRN+1)*TAM_REGISTRO, SEEK_SET);
            }

            RRN++; // Atualiza o valor do RRN
        }

        if (!encontrouAlgum) {
            printf("Registro inexistente.\n");
        }
    }

    // Coloca no cabecalho o novo numero
    // de estacoes e de pares de estacao
    contarEstacoesEPares(fileBin, &cab);

    // Marca o arquivo como consistente
    cab.status = '1';
    escreverCabecalho(fileBin, cab);

    fclose(fileBin);

    BinarioNaTela(binName);
}

// inserirRegistros (INSERT INTO)
// recebe: arquivo binario e N = numeros de insercoes
// retorno: void
// funcionalidade: inserir novos registros baseado na abordagem dinamica
void inserirRegistros(char *binName, int N) {
    // Abre o arquivo no modo leitura e verifica se ocorreu bem
    FILE *fileBin = fopen(binName, "rb+");
    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;

    // Lê o cabecalho do arquivo e verifica se está consistente
    lerCabecalho(fileBin, &cab);
    if(cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    // Marca o arquivo como inconsistente
    cab.status = '0';
    escreverCabecalho(fileBin, cab);

    for(int i = 0; i < N; i++) {
        Registro reg = criarRegistro();

        // Se houver algum registro logicamente removido: 
        if(cab.topo != -1) {
            // Inserimos no RRN correspondente ao topo da pilha
            int RRN = cab.topo;

            // Coloca a próxima referencia da lista no topo da pilha
            fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO + 1, SEEK_SET);
            fread(&cab.topo, sizeof(int), 1, fileBin);

            // Vai pro RRN do topo da pilha e escreve o registro
            fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO, SEEK_SET);
            escreverRegistro(fileBin, reg);
        }
        else { // Caso contrário
            // Inserimos no próximo RRN (fim do arquivo)
            int RRN = cab.proxRRN;

            // Vai pro final do arquivo e escreve o registro
            fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO, SEEK_SET);
            escreverRegistro(fileBin, reg);

            // Atualiza o proximo RRN
            cab.proxRRN++;
        }
    }

    // Coloca no cabecalho o novo numero
    // de estacoes e de pares de estacao
    contarEstacoesEPares(fileBin, &cab);

    // Marca o arquivo como consistente novamente
    cab.status = '1';
    escreverCabecalho(fileBin, cab);

    fclose(fileBin);

    BinarioNaTela(binName);
}

// atualizarRegistros (UPDATE)
// recebe: arquivo binario e N = numeros de atualizacoes
// retorno: void
// funcionalidade: buscar e atualiza os campos de um registro
void atualizarRegistros(char *binName, int N) {
    // Abre o arquivo no modo leitura e verifica se ocorreu bem
    FILE *fileBin = fopen(binName, "rb+");
    if (fileBin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;

    // Lê o cabecalho do arquivo e verifica se está consistente
    lerCabecalho(fileBin, &cab);
    if(cab.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(fileBin);
        return;
    }

    // Marca o arquivo como inconsistente
    cab.status = '0';
    escreverCabecalho(fileBin, cab);

    for (int i = 0; i < N; i++) { // Executa N vezes
        Busca filtro = filtrarRegistro(); // Cria um filtro para a busca (Simula o WHERE)
        Busca atualizacoes = inputAtualizacoes(); // Cria um filtro de atualizacoes

        fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Coloca o fseek no primeiro registro de dados

        int encontrouAlgum = 0;
        Registro reg;

        int RRN = 0; // Inicializa o RRN como zero

        // Passa por cada registro de dados sequencialmente
        // verificando se ele está removido a principio
        while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
            // Se estiver removido pula para o proximo
            if (reg.removido == '1') {
                fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
                RRN++; // Atualiza o valor do RRN
                continue;
            }
            // --- LEITURA DOS REGISTROS ---
            // Lê o restante do registro para comparar
            // Lê o registro com os freads e tambem pula o lixo do registro no final
            
            lerRegistro(fileBin, &reg);

            int coincide = comparaFiltro(filtro, reg);

            if(coincide) {
                // Atualiza o registro
                atualizar(&reg, atualizacoes);

                // Garante que o fseek está no inicio do registro
                fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO, SEEK_SET);

                // Escreve o novo registro no arquivo
                escreverRegistro(fileBin, reg);

                // Poe o fseek no próximo RRN
                fseek(fileBin, TAM_CABECALHO + (RRN+1)*TAM_REGISTRO, SEEK_SET);
            }

            RRN++; // Atualiza o valor do RRN
        }
    }

    // Coloca no cabecalho o novo numero
    // de estacoes e de pares de estacao
    contarEstacoesEPares(fileBin, &cab);

    // Marca o arquivo como consistente
    cab.status = '1';
    escreverCabecalho(fileBin, cab);

    fclose(fileBin);

    BinarioNaTela(binName);
}




// ------- MÓDULOS -------
Cabecalho iniciarCabecalho() {
    Cabecalho cab;
    cab.status = '0';
    cab.topo = -1;
    cab.proxRRN = 0;
    cab.nroEstacoes = 0;
    cab.nroParesEstacao = 0;

    return cab;
}

void lerCabecalho(FILE *fileBin, Cabecalho *cab) {
    fseek(fileBin, 0, SEEK_SET); // Garante que lemos o inicio do binario

    // Lê o cabecalho do arquivo e coloca na struct
    fread(&cab->status, sizeof(char), 1, fileBin);
    fread(&cab->topo, sizeof(int), 1, fileBin);
    fread(&cab->proxRRN, sizeof(int), 1, fileBin);
    fread(&cab->nroEstacoes, sizeof(int), 1, fileBin);
    fread(&cab->nroParesEstacao, sizeof(int), 1, fileBin);
}

void escreverCabecalho(FILE *fileBin, Cabecalho cab) {
    fseek(fileBin, 0, SEEK_SET); // Garante que escrevemos no inicio do binario
    
    // Escreve no arquivo
    fwrite(&cab.status, sizeof(char), 1, fileBin);
    fwrite(&cab.topo, sizeof(int), 1, fileBin);
    fwrite(&cab.proxRRN, sizeof(int), 1, fileBin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, fileBin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, fileBin);
}

// lerRegistro
// recebe: ponteiro para o arquivo binario e ponteiro para o registro
// funcionalidade: lê todos os campos de um registro (exceto o removido) e pula o lixo
// Lê os campos de tamanho fixo
void lerRegistro(FILE *fileBin, Registro *reg) {
    
    fread(&reg->proximo, sizeof(int), 1, fileBin);
    fread(&reg->codEstacao, sizeof(int), 1, fileBin);
    fread(&reg->codLinha, sizeof(int), 1, fileBin);
    fread(&reg->codProxEstacao, sizeof(int), 1, fileBin);
    fread(&reg->distProxEstacao, sizeof(int), 1, fileBin);
    fread(&reg->codLinhaIntegra, sizeof(int), 1, fileBin);
    fread(&reg->codEstIntegra, sizeof(int), 1, fileBin);

    // Lê os campos de tamanho variável
    fread(&reg->tamNomeEstacao, sizeof(int), 1, fileBin);
    if (reg->tamNomeEstacao > 0) {
        fread(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, fileBin);
        reg->nomeEstacao[reg->tamNomeEstacao] = '\0';
    } else {
        strcpy(reg->nomeEstacao, "");
    }

    fread(&reg->tamNomeLinha, sizeof(int), 1, fileBin);
    if (reg->tamNomeLinha > 0) {
        fread(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, fileBin);
        reg->nomeLinha[reg->tamNomeLinha] = '\0';
    } else {
        strcpy(reg->nomeLinha, "");
    }

    // Pula os bytes de lixo ('$') para alinhar os 80 bytes
    // Obs: são 1 char (removido) + 9 ints (proximo, tamNomes e codigos) = 37 bytes fixos
    int bytesLidos = 1 + (9 * 4) + reg->tamNomeEstacao + reg->tamNomeLinha;
    fseek(fileBin, 80 - bytesLidos, SEEK_CUR);
}

void escreverRegistro(FILE* fileBin, Registro reg) {
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
}

void apagarRegistro(FILE *fileBin, Registro *reg, Cabecalho *cab, int RRN) {
    // Posiciona o fseek no inicio do registro
    fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO, SEEK_SET);

    reg->removido = '1'; // Marca o registro como logicamente removido
    reg->proximo = cab->topo; // Marca o valor do próximo RRN removido

    // Escreve no arquivo
    fwrite(&(reg->removido), sizeof(char), 1, fileBin);
    fwrite(&(reg->proximo), sizeof(int), 1, fileBin);

    cab->topo = RRN;

    escreverCabecalho(fileBin, *cab);
}

Registro criarRegistro() {
    Registro reg;

    reg.removido = '0';
    reg.proximo = -1;

    scanf("%d", &reg.codEstacao);
    ScanQuoteString(reg.nomeEstacao);
    scanf("%d", &reg.codLinha);
    ScanQuoteString(reg.nomeLinha);

    reg.tamNomeEstacao = strlen(reg.nomeEstacao);
    reg.tamNomeLinha = strlen(reg.nomeLinha);

    char digitos[64];
    scanf("%s", digitos); // Eu sei que tem um bof aqui :(
    if(!strcmp(digitos, "NULO")) {
        reg.codProxEstacao = -1;
    }
    else {
        reg.codProxEstacao = atoi(digitos);
    }

    scanf("%s", digitos); // Eu sei que tem um bof aqui :(
    if(!strcmp(digitos, "NULO")) {
        reg.distProxEstacao = -1;
    }
    else {
        reg.distProxEstacao = atoi(digitos);
    }

    scanf("%s", digitos); // Eu sei que tem um bof aqui :(
    if(!strcmp(digitos, "NULO")) {
        reg.codLinhaIntegra = -1;
    }
    else {
        reg.codLinhaIntegra = atoi(digitos);
    }

    scanf("%s", digitos); // Eu sei que tem um bof aqui :(
    if(!strcmp(digitos, "NULO")) {
        reg.codEstIntegra = -1;
    }
    else {
        reg.codEstIntegra = atoi(digitos);
    }

    return reg;
}

Busca resetarFiltro() {
    Busca filtro;
    filtro.codEstacao = -2;
    filtro.codLinha = -2;
    filtro.codProxEstacao = -2;
    filtro.distProxEstacao = -2;
    filtro.codLinhaIntegra = -2;
    filtro.codEstIntegra = -2;
    strcpy(filtro.nomeEstacao, "");
    strcpy(filtro.nomeLinha, "");

    return filtro;
}

Busca filtrarRegistro() { 
    Busca filtro = resetarFiltro();

    int m;
    scanf("%d", &m);

    for (int j = 0; j < m; j++){
        char nomeCampo[50];
        scanf("%s", nomeCampo);

        if (strcmp(nomeCampo, "codEstacao") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.codEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "codLinha") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.codLinha = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.codProxEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.distProxEstacao = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.codLinhaIntegra = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
            char valor[50];
            scanf("%s", valor);
            filtro.codEstIntegra = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);

        } else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
            ScanQuoteString(filtro.nomeEstacao);

        } else if (strcmp(nomeCampo, "nomeLinha") == 0) {
            ScanQuoteString(filtro.nomeLinha);
        }
    }

    return filtro;
}

// LOGICA DE COMPARAÇÃO (AND): Se o filtro for != -2, o registro deve bater
// Testa se o filtro existe e depois faz um AND para ver se o valor BATE
// IMPORTANTE: Só entra no IF se o filtro existir e estiver ERRADO
// Pois se estiver errado basta descartar e partir para o prox reg
int comparaFiltro(Busca filtro, Registro reg) {
    if (filtro.codEstacao != -2 && reg.codEstacao != filtro.codEstacao) return 0;
    if (filtro.codLinha != -2 && reg.codLinha != filtro.codLinha) return 0;
    if (filtro.codProxEstacao != -2 && reg.codProxEstacao != filtro.codProxEstacao) return 0;
    if (filtro.distProxEstacao != -2 && reg.distProxEstacao != filtro.distProxEstacao) return 0;
    if (filtro.codLinhaIntegra != -2 && reg.codLinhaIntegra != filtro.codLinhaIntegra) return 0;
    if (filtro.codEstIntegra != -2 && reg.codEstIntegra != filtro.codEstIntegra) return 0;
    if (strlen(filtro.nomeEstacao) > 0 && strcmp(reg.nomeEstacao, filtro.nomeEstacao) != 0) return 0;
    if (strlen(filtro.nomeLinha) > 0 && strcmp(reg.nomeLinha, filtro.nomeLinha) != 0) return 0;

    return 1;
}

Busca inputAtualizacoes() {
    Busca reg = resetarFiltro();

    int p;
    scanf("%d", &p);

    while(p--) {
        char campo[64];
        scanf("%s", campo);

        if(!strcmp(campo, "codEstacao")) {
            scanf("%d", &reg.codEstacao);
        }
        else if(!strcmp(campo, "codLinha")) {
            scanf("%d", &reg.codLinha);
        }
        else if(!strcmp(campo, "codProxEstacao")) {
            scanf("%d", &reg.codProxEstacao);
        }
        else if(!strcmp(campo, "distProxEstacao")) {
            scanf("%d", &reg.distProxEstacao);
        }
        else if(!strcmp(campo, "codLinhaIntegra")) {
            scanf("%d", &reg.codLinhaIntegra);
        }
        else if(!strcmp(campo, "codEstacaoIntegra")) {
            scanf("%d", &reg.codEstIntegra);
        }
        else if(!strcmp(campo, "nomeEstacao")) {
            ScanQuoteString(reg.nomeEstacao);
        }
        else if(!strcmp(campo, "nomeLinha")) {
            ScanQuoteString(reg.nomeLinha);
        }
    }

    return reg;
}

void atualizar(Registro *reg, Busca atualizacoes) {
    if(atualizacoes.codEstacao != 2){
        reg->codEstacao = atualizacoes.codEstacao;
    }
    if(atualizacoes.codLinha != 2){
        reg->codLinha = atualizacoes.codLinha;
    }
    if(atualizacoes.codProxEstacao != 2){
        reg->codProxEstacao = atualizacoes.codProxEstacao;
    }
    if(atualizacoes.distProxEstacao != 2){
        reg->distProxEstacao = atualizacoes.distProxEstacao;
    }
    if(atualizacoes.codLinhaIntegra != 2){
        reg->codLinhaIntegra = atualizacoes.codLinhaIntegra;
    }
    if(atualizacoes.codEstIntegra != 2){
        reg->codEstIntegra = atualizacoes.codEstIntegra;
    }
    if(strcmp(atualizacoes.nomeEstacao, "")){
        strcpy(reg->nomeEstacao, atualizacoes.nomeEstacao);
        reg->tamNomeEstacao = strlen(reg->nomeEstacao);
    }
    if(strcmp(atualizacoes.nomeLinha, "")){
        strcpy(reg->nomeLinha, atualizacoes.nomeLinha);
        reg->tamNomeLinha = strlen(reg->nomeLinha);
    }
}

// Conta a quantidade de estacoes e de pares de estacao
// e coloca no Cabecalho
void contarEstacoesEPares(FILE *fileBin, Cabecalho *cab) {
    if(fileBin == NULL)
        return;

    int RRN = 0;
    
    // Reseta a contagem de estacoes e
    // de pares do cabecalho
    cab->nroEstacoes = 0;
    cab->nroParesEstacao = 0;

    char nomesVistos[MAX_REGISTROS][256];
    ParEstacao paresVistos[MAX_REGISTROS];

    // Posiciona o fseek pro primeiro registro
    fseek(fileBin, TAM_CABECALHO, SEEK_SET);

    Registro reg;

    while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
        // Se estiver removido pula para o proximo
        if (reg.removido == '1') {
            fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
            RRN++; // Atualiza o valor do RRN
            continue;
        }

        lerRegistro(fileBin, &reg);
 
        // --- LOGICA DE CONTAGEM ---
        // Checa se a estação já foi vista pelo NOME (regra do PDF)
        int estacaoRepetida = 0;
        for (int i = 0; i < cab->nroEstacoes; i++) {
            if (strcmp(nomesVistos[i], reg.nomeEstacao) == 0) {
                estacaoRepetida = 1;
                break;
            }
        }
        if (!estacaoRepetida) {
            strcpy(nomesVistos[cab->nroEstacoes], reg.nomeEstacao);
            cab->nroEstacoes++;
        }

        // Checa se o par já foi visto
        if (reg.codProxEstacao != -1) {
            int parRepetido = 0;
            for (int i = 0; i < cab->nroParesEstacao; i++) {
                if (paresVistos[i].origem == reg.codEstacao && paresVistos[i].destino == reg.codProxEstacao) {
                    parRepetido = 1;
                    break;
                }
            }
            if (!parRepetido) {
                paresVistos[cab->nroParesEstacao].origem = reg.codEstacao;
                paresVistos[cab->nroParesEstacao].destino = reg.codProxEstacao;
                cab->nroParesEstacao++;
            }
        }

        RRN++;
        fseek(fileBin, TAM_CABECALHO + RRN*TAM_REGISTRO, SEEK_SET);
    }
}
