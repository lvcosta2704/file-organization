#ifndef REGISTROSIO_H
#define REGISTROSIO_H

// === MACROS ===

#define MAX_REGISTROS 250
#define TAM_CABECALHO 17
#define TAM_REGISTRO 80

// === STRUCTS ===

// Struct que ajuda a contar os pares de estacoes diferentes
typedef struct {
  int origem;
  int destino;
} ParEstacao;

// Struct para armazenar os campos de busca do arquivo binario e compara-los
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

// Struct para armazenar os dados do cabecalho do arquivo binario
typedef struct {
  char status;
  int topo;
  int proxRRN;
  int nroEstacoes;
  int nroParesEstacao;
} Cabecalho;

// Struct para armazenar os dados dos registros do arquivo binário
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

// === FUNCOES ===

Cabecalho iniciarCabecalho();
void lerCabecalho(FILE *fileBin, Cabecalho *cab);
void escreverCabecalho(FILE *fileBin, Cabecalho cab);
void lerRegistro(FILE *fileBin, Registro *reg);
void escreverRegistro(FILE *fileBin, Registro reg);
void apagarRegistro(FILE *fileBin, Registro *reg, Cabecalho *cab, int RRN);
Registro inputRegistro();
Busca resetarFiltro();
Busca inputFiltro();
int comparaFiltro(Busca filtro, Registro reg);
Busca inputAtualizacoes();
void atualizar(Registro *reg, Busca atualizacoes);
void contarEstacoesEPares(FILE *fileBin, Cabecalho *cab);

#endif
