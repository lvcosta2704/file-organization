#include "BTree.h"
#include "Registros.h"
#include "fornecidas.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definicao de macros
#define ORDEM 4
#define MAX_CHAVES ORDEM - 1
#define TAM_CABECALHO 17
#define TAM_NO 13 + 8 * MAX_CHAVES + 4 * ORDEM

// Struct para armazenar a chave e seu respectivo índice
struct chaveIndice {
  int codEstacao;
  int RRN;
};

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
  struct chaveIndice *chave; // C1, Pr1 a Cn, Prn, MAX_CHAVES
  int *indiceNo;             // P1 a Pn, ORDEM
} BTreeNode;

// Cria um nó vazio
// Funcionamento:
// Aloca memória para a criacao do no e inicializa seus campos
// conforme especificado
BTreeNode *criarNo() {
  // Aloca memoria para nó
  BTreeNode *node = (BTreeNode *)malloc(TAM_NO);
  if (!node) {
    printf("Erro ao alocar memoria.\n");
    return NULL;
  }

  // Inicializa os campos do nó
  node->removido = '0';
  node->proximo = -1;
  node->tipoNo = -1; // Inicializa o nó como raiz
  node->nroChaves = 0;

  // Inicializa as chaves como -1
  for (int i = 0; i < MAX_CHAVES; i++) {
    node->chave[i].codEstacao = -1;
    node->chave[i].RRN = -1;
  }

  // Inicializa os indices para os outros nós
  // como -1
  for (int i = 0; i < ORDEM; i++) {
    node->indiceNo[i] = -1;
  }

  return node;
}
