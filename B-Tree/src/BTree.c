#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BTree.h"
#include "BTree_utils.h"

// === ARVORE-B ===

// Cria um nó vazio
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

  // Aloca memoria para a array de indices
  node->indice = malloc(sizeof(struct Indice) * MAX_CHAVES);
  if(!(node->indice)) {
    printf("Erro ao alocar memoria.\n");
    return NULL;
  }

  // Inicializa os indices como -1
  for (int i = 0; i < MAX_CHAVES; i++) {
    node->indice[i].codEstacao = -1;
    node->indice[i].RRN = -1;
  }

  // Aloca memoria para a referencia para os outros nós
  node->ponteiroNo = malloc(sizeof(int) * ORDEM);
  if(!(node->ponteiroNo)) {
    printf("Erro ao alocar memoria.\n");
    return NULL;
  }

  // Inicializa os indices para os outros nós como -1
  for (int i = 0; i < ORDEM; i++) {
    node->ponteiroNo[i] = -1;
  }

  return node;
}

void buscaArvoreB() {
  
}
