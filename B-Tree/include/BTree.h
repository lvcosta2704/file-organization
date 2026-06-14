#ifndef BTREE_H
#define BTREE_H

// === MACROS ===

#define ORDEM 4
#define MAX_CHAVES (ORDEM - 1)
#define MIN_CHAVES (ORDEM/2 - 1)
#define TAM_CABECALHO_BTREE 17
#define TAM_NO (13 + 8 * MAX_CHAVES + 4 * ORDEM)

#define ENCONTROU 1
#define NAO_ENCONTROU 0
#define PROMOCAO 1
#define SEM_PROMOCAO 0
#define ERRO -1

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
void escreverNo(FILE *btreeindex, int RRN, const BTreeNode *node);
void imprimeArray(int *array);
void imprimeNo(const BTreeNode node);
void arrayDeChaves(int *array, const BTreeNode *node);
void lerCabecalhoArvoreB(FILE *btreeindex, BTreeHeader *header);
void escreverCabecalhoArvoreB(FILE *btreeindex, BTreeHeader *header);
int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int *FOUND_RRN, int *FOUND_POS, int *FOUND_DATA_RRN);
int insercaoArvoreB(FILE *btreeindex, BTreeHeader *header, int RRNAtual, int chave, int offset, int *PROMO_R_CHILD, int *PROMO_KEY, int *PROMO_OFFSET);
int removerArvoreB(FILE *btreeindex, BTreeHeader *header, int rrnAtual, int posChave, int chave);

void inserirOrdenado(BTreeNode *node, int chave, int offset, int r_child);
void split(BTreeNode *nodeAtual, int chaveInserida, int offsetInserido, int ptrInserido, BTreeNode *novoNo, int *chavePromovida, int *offsetPromovido);

// -------- FUNCOES AUXILIARES DA REMOCAO -------

int tratarUnderflow(FILE *btreeindex, BTreeHeader *header, int rrnAtual);
int removerChaveNo(BTreeNode *node, int pos);
int ehFolha(const BTreeNode *node);
int liberarPaginaBTree(FILE *btreeindex, BTreeHeader *header, int rrnLiberado);
int buscarPai(FILE *btreeindex, int rrnAtual, int rrnFilho, int *rrnPai, int *posFilhoNoPai);
int emprestarDireita(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho);
int emprestarEsquerda(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho);
int fundirComIrmaoEsq(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho);
int fundirComIrmaoDir(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho);
int buscarSucessorImediato(FILE *btreeindex, int rrnSubarvore, int *rrnFolha, int *posNaFolha);

#endif
