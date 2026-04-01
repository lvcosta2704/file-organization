#ifndef REGISTROS_H
#define REGISTROS_H

typedef struct parestacao ParEstacao;
typedef struct busca Busca;
typedef struct cabecalho Cabecalho;
typedef struct registro Registro;

// ------ PROTOTIPOS DAS FUNCOES PRINCIPAIS ------
void criarBin(char* csvName, char* binName);
void listarRegistros(char *binName);
void buscarRegistros(char *binName, int N);
void removerRegistros(char *binName, int N);
void inserirRegistros(char *binName, int N);
void atualizarRegistros(char *binName, int N);

// ----------- PROTOTIPOS DOS MODULOS -----------
void BinarioNaTela(char *arquivo); // Fornecido
void ScanQuoteString(char *str); // Fornecido
Cabecalho iniciarCabecalho();
Busca resetarFiltro();
void escreverCabecalho(FILE *fileBin, Cabecalho cab);
void lerCabecalho(FILE *fileBin, Cabecalho *cab);

#endif
