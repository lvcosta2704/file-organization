#ifndef BTREE_H
#define BTREE_H

// === MACROS ===

#define ORDEM 4
#define MAX_CHAVES ORDEM - 1
#define TAM_CABECALHO_BTREE 17
#define TAM_NO 13 + 8 * MAX_CHAVES + 4 * ORDEM

// === STRUCTS ===

// Struct para armazenar a chave e seu respectivo RRN
struct Indice {
  int codEstacao;
  int RRN;
};

// Struct para armazenar os dados do cabecalho do arquivo de indice Arvore-B
typedef struct {
  char status;
  int noRaiz;
  int topo;
  int proxRRN;
  int nroNos;
} BTreeHeader;

typedef struct {
  char removido;
  int proximo;
  int tipoNo;
  int nroChaves;
  struct Indice *indice; // C1, Pr1 a Cn, Prn
  int *ponteiroNo;       // P1 a Pn
} BTreeNode;

// === FUNCOES ===
BTreeNode *criarNo();
void lerNo(FILE *btreeindex, int RRN, BTreeNode *node);
int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int FOUND_RRN, int FOUND_POS);

#endif
