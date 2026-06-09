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
    node->indice[i].offset = -1;
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

void liberaNo(BTreeNode **node) {
  if(!node || !(*node)) {
    printf("No NULL.\n");
    return;
  }

  free((*node)->indice);
  free((*node)->ponteiroNo);

  free(*node);
}

// Funcao que imprime um nó, util para debuggar
void imprimeNo(const BTreeNode node) {
  printf("No atual: ");

  printf("[%c][%d][%d][%d]--", node.removido, node.proximo, node.tipoNo, node.nroChaves);

  for(int i = 0; i < MAX_CHAVES; i++) {
    printf("[%d][%u]", node.indice[i].codEstacao, node.indice[i].offset);
  }

  printf("--");

  for(int i = 0; i < ORDEM; i++) {
    printf("[%d]", node.ponteiroNo[i]);
  }

  printf("\n");
}

void imprimeArray(int *array) {
    printf("Array de chaves: ");
    for(int i = 0; i < MAX_CHAVES; i++) {
        printf("[%d]", array[i]);
    }
    printf("\n");
}


void escreverNo(FILE *btreeindex, int RRN, const BTreeNode *node) {
  // Verifica se o arquivo nao é NULL, por questoes de seguranca
  if (!btreeindex || !node) return;

  // Posiciona a referência para o arquivo no RRN correto
  fseek(btreeindex, TAM_CABECALHO_BTREE + RRN * TAM_NO, SEEK_SET);

  // Lê cada um dos campos
  fwrite(&node->removido, sizeof(char), 1, btreeindex);
  fwrite(&node->proximo, sizeof(int), 1, btreeindex);
  fwrite(&node->tipoNo, sizeof(int), 1, btreeindex);
  fwrite(&node->nroChaves, sizeof(int), 1, btreeindex);

  for(int i = 0; i < MAX_CHAVES; i++) {
    fwrite(&node->indice[i].codEstacao, sizeof(int), 1, btreeindex);
    fwrite(&node->indice[i].offset, sizeof(int), 1, btreeindex);
  }

  for(int i = 0; i < ORDEM; i++) {
    fwrite(&node->ponteiroNo[i], sizeof(int), 1, btreeindex);
  }

  //imprimeNo(*node);
}


// Lê o nó de determinado RRN do arquivo de índice Arvore-B
// e armazena na memoria primaria
void lerNo(FILE *btreeindex, int RRN, BTreeNode *node) {
  // Verifica se o arquivo nao é NULL, por questoes de seguranca
  if(!btreeindex) {
    printf("O arquivo Arvore-B é NULL.\n");
    return;
  }
  if(!node) {
    printf("O nó é NULL.\n");
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
    fread(&node->indice[i].offset, sizeof(int), 1, btreeindex);
  }

  for(int i = 0; i < ORDEM; i++) {
    fread(&node->ponteiroNo[i], sizeof(int), 1, btreeindex);
  }

  //imprimeNo(*node);
}


void escreverCabecalhoArvoreB(FILE *btreeindex, BTreeHeader *header) {
  fseek(btreeindex, 0, SEEK_SET);

  fwrite(&header->status, sizeof(char), 1, btreeindex);
  fwrite(&header->noRaiz, sizeof(int), 1, btreeindex);
  fwrite(&header->topo, sizeof(int), 1, btreeindex);
  fwrite(&header->proxRRN, sizeof(int), 1, btreeindex);
  fwrite(&header->nroNos, sizeof(int), 1, btreeindex);
}

void lerCabecalhoArvoreB(FILE *btreeindex, BTreeHeader *header) {
  fseek(btreeindex, 0, SEEK_SET);

  fread(&header->status, sizeof(char), 1, btreeindex);
  fread(&header->noRaiz, sizeof(int), 1, btreeindex);
  fread(&header->topo, sizeof(int), 1, btreeindex);
  fread(&header->proxRRN, sizeof(int), 1, btreeindex);
  fread(&header->nroNos, sizeof(int), 1, btreeindex);
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

int removerChaveNo(BTreeNode *node, int pos) {
  for (int i = pos; i < node->nroChaves - 1; i++)
  {
    node->indice[i] = node->indice[i+1];
  }

  node->indice[node->nroChaves - 1].codEstacao = -1;
  node->indice[node->nroChaves - 1].offset = -1;
  node->nroChaves--;

  return 1;
}

int ehFolha(const BTreeNode *node) {
  for (int i = 0; i < ORDEM; i++)
  {
    if (node->ponteiroNo[i] != -1) return 0;
  }
  return 1;
}

// Retorna 1 se encontrou e 0 se nao encontrou
int buscaArvoreB(FILE *btreeindex, int RRN, int chave, int *FOUND_RRN, int *FOUND_POS, int *FOUND_OFFSET) {
  // Se o RRN é -1, para
  if(RRN == -1) {
    return NAO_ENCONTROU;
  }
  else {
    // Lê o nó de RRN especificado na memoria primaria
    BTreeNode *node = criarNo();
    lerNo(btreeindex, RRN, node);

    // Procura pela chave no nó e armazena em uma variável a posicao em que ocorre
    int pos; 
    int prox;

    // SUB-ROTINA: criar uma array de chaves
    int array[MAX_CHAVES];
    arrayDeChaves(array, node); // OBS: a array sempre está ordenada
    
    //imprimeArray(array);
                                
    // SUB-ROTINA: realizar busca binaria na array de chaves
    pos = binarySearch(chave, array, 0, MAX_CHAVES, &prox);

    //printf("pos: %d\n", pos);

    // Se a chave foi encontrada
    if(pos != -1) { 
      if(FOUND_RRN != NULL) {
        *FOUND_RRN = RRN; // Salva o RRN da pagina, se pedido
      }
      if(FOUND_POS != NULL) {
        *FOUND_POS = pos; // Salva a posicao, se pedido
      }
      if(FOUND_OFFSET != NULL) {
        *FOUND_OFFSET = node->indice[pos].offset; // Salva o offset no arquivo de dados, se pedido
      }

      liberaNo(&node);
      
      return ENCONTROU;
    }
    // Se a chave nao foi encontrada
    else {
      // Realiza a busca no próximo no, recursivamente
      int proxRRN = node->ponteiroNo[prox];
      liberaNo(&node);
      return buscaArvoreB(btreeindex, proxRRN, chave, FOUND_RRN, FOUND_POS, FOUND_OFFSET);
    }
  }
}

int removerArvoreB(FILE *btreeindex, BTreeHeader *header, int rrnAtual, int posChave, int chave) {
  BTreeNode *node = criarNo();
  lerNo(btreeindex, rrnAtual, node);

  if (ehFolha(node)) {
    removerChaveNo(node, posChave);

    escreverNo(btreeindex, rrnAtual, node);

    
  }
}