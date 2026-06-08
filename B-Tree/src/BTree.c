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

// Lê o nó de determinado RRN do arquivo de índice Arvore-B
// e armazena na memoria primaria
void lerNo(FILE *btreeindex, int RRN, BTreeNode *node) {
  // Posiciona a referência para o arquivo no RRN correto
  fseek(btreeindex, TAM_CABECALHO_BTREE + RRN * TAM_NO, SEEK_SET);

  // Lê cada um dos campos
  fread(&node->removido, sizeof(char), 1, btreeindex);
  fread(&node->proximo, sizeof(int), 1, btreeindex);
  fread(&node->tipoNo, sizeof(int), 1, btreeindex);
  fread(&node->nroChaves, sizeof(int), 1, btreeindex);

  for(int i = 0; i < MAX_CHAVES; i++) {
    fread(&node->indice[i].codEstacao, sizeof(int), 1, btreeindex);
    fread(&node->indice[i].RRN, sizeof(int), 1, btreeindex);
  }

  for(int i = 0; i < ORDEM; i++) {
    fread(&node->ponteiroNo[i], sizeof(int), 1, btreeindex);
  }
}

int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int FOUND_RRN, int FOUND_POS) {
  // Se o RRN é -1, para
  if(RRN == -1) {
    return -1;
  }
  else {
    BTreeNode *node;
    lerNo(btreeindex, RRN, node);
  }
}
