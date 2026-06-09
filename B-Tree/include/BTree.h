#ifndef BTREE_H
#define BTREE_H

// === MACROS ===

#define ORDEM 4
#define MAX_CHAVES (ORDEM - 1)
#define TAM_CABECALHO_BTREE 17
#define TAM_NO (13 + 8 * MAX_CHAVES + 4 * ORDEM)

#define ENCONTROU 1
#define NAO_ENCONTROU 0

// === STRUCTS ===

// Struct para armazenar a chave e seu respectivo RRN
struct Indice {
  int codEstacao;
  int offset;
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
void liberaNo(BTreeNode **node);
void lerNo(FILE *btreeindex, int RRN, BTreeNode *node);
void imprimeArray(int *array);
void imprimeNo(const BTreeNode node);
void arrayDeChaves(int *array, const BTreeNode *node);
void lerCabecalhoArvoreB(FILE *btreeindex, BTreeHeader *header);
void escreverCabecalhoArvoreB(FILE *btreeindex, BTreeHeader header);
int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int *FOUND_RRN, int *FOUND_POS, int *FOUND_DATA_RRN);
int removerArvoreB(FILE *btreeindex, BTreeHeader *header, int rrnAtual, int posChave, int chave);

#endif
