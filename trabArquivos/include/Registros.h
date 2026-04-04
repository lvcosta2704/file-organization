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
Cabecalho iniciarCabecalho();
void lerCabecalho(FILE *fileBin, Cabecalho *cab);
void escreverCabecalho(FILE *fileBin, Cabecalho cab);
void lerRegistro(FILE *fileBin, Registro *reg);
void escreverRegistro(FILE* fileBin, Registro reg);
void apagarRegistro(FILE *fileBin, Registro *reg, Cabecalho *cab, int RRN);
Registro criarRegistro();
Busca resetarFiltro();
Busca filtrarRegistro();
int comparaFiltro(Busca filtro, Registro reg);
Busca inputAtualizacoes();
void atualizar(Registro *reg, Busca atualizacoes);
void contarEstacoesEPares(FILE *fileBin, Cabecalho *cab);

#endif
