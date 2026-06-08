#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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
  // Verifica se o arquivo nao é NULL, por questoes de seguranca
  if(!btreeindex) {
    printf("O arquivo Arvore-B é NULL.\n");
    return;
  }

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

// Pega as chaves de um nó e transforma em uma array
void arrayDeChaves(int *array, const BTreeNode *node) {
  for(int i = 0; i < MAX_CHAVES; i++) {
    // Se nao existir a chave
    if(node->indice[i].codEstacao == -1)
      array[i] = INT_MAX; // Preenche a array com INT_MAX
    else {
      array[i] = node->indice[i].codEstacao;
    }
  }
}

// Retorna 1 se encontrou e 0 se nao encontrou
int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int *FOUND_RRN, int *FOUND_POS) {
  // Se o RRN é -1, para
  if(RRN == -1) {
    return NAO_ENCONTROU;
  }
  else {
    // Lê o nó de RRN especificado na memoria primaria
    BTreeNode *node;
    lerNo(btreeindex, RRN, node);

    // Procura pela chave no nó e armazena em uma variável a posicao em que ocorre
    int pos; 
    int *prox;

    // SUB-ROTINA: criar uma array de chaves
    int array[MAX_CHAVES];
    arrayDeChaves(array, node); // OBS: a array sempre está ordenada
                                
    // SUB-ROTINA: realizar busca binaria na array de chaves
    binarySearch(chave, array, 0, MAX_CHAVES, prox);

    // Se a chave foi encontrada
    if(pos != -1) {
      *FOUND_RRN = RRN; // Salva o RRN
      *FOUND_POS = pos; // Salva a posicao
      return ENCONTROU;
    }
    // Se a chave nao foi encontrada
    else {
      // Realiza a busca no próximo no, recursivamente
      return buscaArvoreB(btreeindex, node->ponteiroNo[*prox], chave, FOUND_RRN, FOUND_POS);
    }
  }
}
