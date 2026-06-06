#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BTree.h"
#include "query.h"
#include "utils.h"

// === ARVORE-B ===

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
    node->indice[i].codEstacao = -1;
    node->indice[i].RRN = -1;
  }

  // Inicializa os indices para os outros nós
  // como -1
  for (int i = 0; i < ORDEM; i++) {
    node->ponteiroNo[i] = -1;
  }

  return node;
}
