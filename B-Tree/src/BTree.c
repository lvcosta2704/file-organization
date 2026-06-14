#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "BTree.h"
#include "BTree_utils.h"

// === ARVORE-B ===

// ======= ABRIR E FECHA NÓ =========
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

// ======= IN OUT NÓ E CABEÇALHO =========

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
// ======= IMPRESSAO E DEGUB =========

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

// ======= UTILS DA ARVORE =========


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
  if (node == NULL) return 0;

  for (int i = 0; i < ORDEM; i++) {
    if (node->ponteiroNo[i] != -1) {
      return 0;
    }
  }
  return 1;
}



// ======= BUSCA =========

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

      if(FOUND_RRN != NULL) {
        *FOUND_RRN = RRN; // Salva o RRN da pagina, se pedido
      }
      return buscaArvoreB(btreeindex, proxRRN, chave, FOUND_RRN, FOUND_POS, FOUND_OFFSET);
    }
  }
}

// ======== INSERCAO =========

void inserirOrdenado(BTreeNode *node, int chave, int offset, int r_child) {
    // Começamos do fim do nó (última chave válida)
    int i = node->nroChaves - 1;

    // Desloca as chaves, offsets e ponteiros para a direita
    // enquanto encontrar chaves maiores que a chave que queremos inserir
    while (i >= 0 && node->indice[i].codEstacao > chave) {
        // Desloca o índice (chave e offset juntos)
        node->indice[i+1] = node->indice[i];
        
        // Desloca o ponteiro da direita correspondente a essa chave
        node->ponteiroNo[i+2] = node->ponteiroNo[i + 1];
        
        i--;
    }

    // Insere a nova chave e seu offset na posição vaga encontrada
    node->indice[i+1].codEstacao = chave;
    node->indice[i+1].offset = offset;
    
    // O ponteiro do filho direito da nova chave vai para a posição i + 2
    node->ponteiroNo[i + 2] = r_child;

    // Incrementa o contador de chaves do nó
    node->nroChaves++;
}

void split(BTreeNode *nodeAtual, int chaveInserida, int offsetInserido, int ptrInserido, BTreeNode *novoNo, int *chavePromovida, int *offsetPromovido) {
    
    // Criação dos arrays temporários que suportam o overflow (tamanho + 1)
    int tempChaves[MAX_CHAVES + 1];
    int tempOffsets[MAX_CHAVES + 1];
    int tempPonteiros[MAX_CHAVES + 2]; // Ponteiros são sempre chaves + 1
    
    // 1. Copia todos os dados do nó cheio para as arrays temporárias
    int i;
    for (i = 0; i < MAX_CHAVES; i++) {
        tempChaves[i] = nodeAtual->indice[i].codEstacao;
        tempOffsets[i] = nodeAtual->indice[i].offset;
        tempPonteiros[i] = nodeAtual->ponteiroNo[i];
    }
    tempPonteiros[MAX_CHAVES] = nodeAtual->ponteiroNo[MAX_CHAVES];
    
    // 2. Insere a nova chave/offset/ponteiro de forma ordenada na estrutura temporária
    int pos = MAX_CHAVES - 1;
    while (pos >= 0 && tempChaves[pos] > chaveInserida) {
        tempChaves[pos + 1] = tempChaves[pos];
        tempOffsets[pos + 1] = tempOffsets[pos];
        tempPonteiros[pos + 2] = tempPonteiros[pos + 1];
        pos--;
    }
    tempChaves[pos + 1] = chaveInserida;
    tempOffsets[pos + 1] = offsetInserido;
    tempPonteiros[pos + 2] = ptrInserido;
    
    // 3. Encontra o ponto de divisão (meio) e define quem será promovido
    int mid = (MAX_CHAVES + 1) / 2; 
    *chavePromovida = tempChaves[mid];
    *offsetPromovido = tempOffsets[mid];
    
    // 4. Atualiza a quantidade de chaves de cada nó pós-divisão
    nodeAtual->nroChaves = mid;
    novoNo->nroChaves = MAX_CHAVES - mid;
    
    // 5. Devolve a primeira metade (elementos menores que o 'mid') para o nodeAtual
    for (i = 0; i < mid; i++) {
        nodeAtual->indice[i].codEstacao = tempChaves[i];
        nodeAtual->indice[i].offset = tempOffsets[i];
        nodeAtual->ponteiroNo[i] = tempPonteiros[i];
    }
    nodeAtual->ponteiroNo[mid] = tempPonteiros[mid];
    
    // Limpa o restante das posições do nodeAtual que agora estão vazias
    for (i = mid; i < MAX_CHAVES; i++) {
        nodeAtual->indice[i].codEstacao = -1;
        nodeAtual->indice[i].offset = -1;
        nodeAtual->ponteiroNo[i + 1] = -1;
    }
    
    // 6. Envia a segunda metade (elementos maiores que o 'mid') para o novoNo
    for (i = mid + 1; i <= MAX_CHAVES; i++) {
        int novoIdx = i - (mid + 1);
        novoNo->indice[novoIdx].codEstacao = tempChaves[i];
        novoNo->indice[novoIdx].offset = tempOffsets[i];
        novoNo->ponteiroNo[novoIdx] = tempPonteiros[i];
    }
    novoNo->ponteiroNo[novoNo->nroChaves] = tempPonteiros[MAX_CHAVES + 1];
    
    // Garante que o resto do novoNo fique limpo
    for (i = novoNo->nroChaves; i < MAX_CHAVES; i++) {
        novoNo->indice[i].codEstacao = -1;
        novoNo->indice[i].offset = -1;
        novoNo->ponteiroNo[i + 1] = -1;
    }
    
    // 7. Configura os metadados estruturais do novo nó
    novoNo->tipoNo = nodeAtual->tipoNo; // Se o atual era folha, o novo também será.
    novoNo->removido = '0';
    novoNo->proximo = -1;
}

int insercaoArvoreB(FILE *btreeindex, int RRNAtual, int chave, int offset, int *PROMO_R_CHILD, int *PROMO_KEY, int *PROMO_OFFSET) {
  // Se passou de um nó folha
  if(RRNAtual == -1) {
    *PROMO_KEY = chave;
    *PROMO_OFFSET = offset;
    *PROMO_R_CHILD = -1;
    return PROMOCAO; // Retorna promocao para a chamada recursiva (insere no nó folha)
  }
  else { // Se nao
    // Realiza uma busca:
    // Lê a página atual na memoria principal
    BTreeNode *node = criarNo();
    lerNo(btreeindex, RRNAtual, node);

    int pos;
 
     // SUB-ROTINA: criar uma array de chaves
    int array[MAX_CHAVES];
    arrayDeChaves(array, node); // OBS: a array sempre está ordenada
    
    //imprimeArray(array);
                                
    // SUB-ROTINA: realizar busca binaria na array de chaves
    if(binarySearch(chave, array, 0, MAX_CHAVES, &pos) != -1) {
      printf("Nao é possivel inserir chaves duplicadas.\n");
      liberaNo(&node);
      return ERRO;
    }

    // Chama a funcao recursivamente descobrindo se haverá promocao
    int RETORNO = insercaoArvoreB(btreeindex, node->ponteiroNo[pos], chave, offset, PROMO_R_CHILD, PROMO_KEY, PROMO_OFFSET);

    if(RETORNO == SEM_PROMOCAO || RETORNO == ERRO) { // Se nao há promocao ou há erro, retorna
      liberaNo(&node);
      return RETORNO;
    }
    else if(node->nroChaves < MAX_CHAVES) { // Se há promocao e tem espaco no nó
      // Insere a chave no nó de forma ordenada
      inserirOrdenado(node, *PROMO_KEY, *PROMO_OFFSET, *PROMO_R_CHILD);
      escreverNo(btreeindex, RRNAtual, node);
      liberaNo(&node);
      return SEM_PROMOCAO;
    }
    else { // Se há promocao e o nó está cheio
      // Cria um novo nó
      BTreeNode *novoNo = criarNo();
      int novaChavePromovida;
      int novoOffsetPromovido;
      int novoPonteiroPromovido;

      // Realiza o split
      split(node, *PROMO_KEY, *PROMO_OFFSET, *PROMO_R_CHILD, novoNo, &novaChavePromovida, &novoOffsetPromovido);

      // Define o RRN no novo nó
      BTreeHeader *header = malloc(sizeof(BTreeHeader));
      lerCabecalhoArvoreB(btreeindex, header);

      novoPonteiroPromovido = header->proxRRN;

      (header->proxRRN)++;
      escreverCabecalhoArvoreB(btreeindex, header);

      free(header);

      // Salva ambos os nós no disco
      escreverNo(btreeindex, RRNAtual, node);
      escreverNo(btreeindex, novoPonteiroPromovido, novoNo);

      *PROMO_KEY = novaChavePromovida;
      *PROMO_OFFSET = novoOffsetPromovido;
      *PROMO_R_CHILD = novoPonteiroPromovido;

      liberaNo(&node);
      liberaNo(&novoNo);

      return PROMOCAO;
    }
  }
}

// ======== REMOCAO ==========


int liberarPaginaBTree(FILE *btreeindex, BTreeHeader *header, int rrnLiberado) {
  BTreeNode *node = criarNo();
  lerNo(btreeindex, rrnLiberado, node); // Le o nó com o determinado RRN

  node->removido = '1'; // Seta como removido logicamente
  node->proximo = header->topo; // Push na pilha

  header->topo = rrnLiberado; // Ultima página liberada
  // Procedimento de escrita do cabeçalho e no nó
  escreverCabecalhoArvoreB(btreeindex, header);
  escreverNo(btreeindex, rrnLiberado, node);

  liberaNo(&node);
  return 1;
}


int buscarSucessorImediato(FILE *btreeindex, int rrnSubarvore, int *rrnFolha, int *posNaFolha) {
  if (rrnSubarvore == -1) return 0; // Se nao ha subarvore, retorna 0
  // Procediemento de leitura 
  BTreeNode *node = criarNo();
  lerNo(btreeindex, rrnSubarvore, node);

  int rrnAtual = rrnSubarvore;

  // Desce sempre pela subárvore da esquerda até chegar numa folha
  while (!ehFolha(node)) {
    rrnAtual = node->ponteiroNo[0];
    liberaNo(&node);
    node = criarNo();
    lerNo(btreeindex, rrnAtual, node);
  }

  // Encontrou a folha, a sucessora é a primeira chave dela
  if (rrnFolha) *rrnFolha = rrnAtual;
  if (posNaFolha) *posNaFolha = 0;

  liberaNo(&node);
  return 1;
}

// Funcao que ajuda a achar os irmaos de um nó, primeiramente achando o pai
int buscarPai(FILE *btreeindex, int rrnAtual, int rrnFilho, int *rrnPai, int *posFilhoNoPai) {
  if (rrnAtual == -1) return 0; // Se chegou em -1 não tem nó nesse RRN
  // Procedimento de leitura (dados na RAM)
  BTreeNode *node = criarNo();
  lerNo(btreeindex, rrnAtual, node);
  // Loop para checar se o rrnFilho é igual a algum ponteiroNo do pai
  for (int i = 0; i < ORDEM; i++) {
    // Checa todos os filhos do Nó e ve se algum bate com o RRNFilho
    if (node->ponteiroNo[i] == rrnFilho) {
      if (rrnPai) {
        *rrnPai = rrnAtual;
      }
      if (posFilhoNoPai) {
        *posFilhoNoPai = i;
      } 
      liberaNo(&node);
      return 1;
    }
  }
  // Chama recursivamente para procurar em profundidade buscando o pai
  for (int i = 0; i < ORDEM; i++) {
    if (node->ponteiroNo[i] != -1) {
      if (buscarPai(btreeindex, node->ponteiroNo[i], rrnFilho, rrnPai, posFilhoNoPai)) {
        liberaNo(&node);
        return 1;
      }
    }
  }

  liberaNo(&node);
  return 0;
}
// Funcao que desempenha o emprestimo pela direita de uma chave para outro nó
int emprestarDireita(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  // Procedimento de leitura (dados na RAM)
  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);
  // Se posFIlho foi maior ou igual nao faz sentido calcular, já que o pai nao possui essa qtd de chaves
  if (posFilho >= pai->nroChaves) {
    liberaNo(&pai);
    return 0;
  }
  // Se nao houver irmao a direita nao faz sentido emprestar da Direita
  int rrnDir = pai->ponteiroNo[posFilho + 1];
  if (rrnDir == -1) {
    liberaNo(&pai);
    return 0;
  }

  // Procedimento de leitura (dados na RAM)
  BTreeNode *filho = criarNo();
  BTreeNode *dir = criarNo();
  lerNo(btreeindex, rrnFilho, filho);
  lerNo(btreeindex, rrnDir, dir);

  // Se o nó da direita nao tiver a quantidade minima de chaves entao nao tem como emprestar
  if (dir->nroChaves <= MIN_CHAVES) {
    liberaNo(&pai);
    liberaNo(&filho);
    liberaNo(&dir);
    return 0;
  }

  // Puxa chaves do irmão da direita enquanto ele tiver MAIS chaves que o filho
  while (filho->nroChaves < dir->nroChaves) {
    // Desce a chave do pai para o filho
    filho->indice[filho->nroChaves] = pai->indice[posFilho];

    // Passa o ponteiro mais à esquerda da dir para ser o da direita do filho
    if (!ehFolha(filho)) {
      filho->ponteiroNo[filho->nroChaves + 1] = dir->ponteiroNo[0];
    }

    // Sobe a primeira chave da dir para o pai
    pai->indice[posFilho] = dir->indice[0];

    // Puxa tudo na dir uma casa para a esquerda
    for (int i = 0; i < dir->nroChaves - 1; i++) {
      dir->indice[i] = dir->indice[i + 1];
    }
    dir->indice[dir->nroChaves - 1].codEstacao = -1;
    dir->indice[dir->nroChaves - 1].offset = -1;

    // Reajuste de ponteiros se nao for folha
    if (!ehFolha(dir)) {
      // Move os ponteiros para esquerda
      for (int i = 0; i < dir->nroChaves; i++) {
        dir->ponteiroNo[i] = dir->ponteiroNo[i + 1];
      }
      // Ultima posicao fica sem nada, por isso aponta para -1
      dir->ponteiroNo[dir->nroChaves] = -1;
    }

    // Atualiza os contadores para a próxima iteração
    dir->nroChaves--;
    filho->nroChaves++;
  }

  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnFilho, filho);
  escreverNo(btreeindex, rrnDir, dir);
  escreverNo(btreeindex, rrnPai, pai);
  // Liberação na RAM
  liberaNo(&pai);
  liberaNo(&filho);
  liberaNo(&dir);
  return 1;
}
// Funcao que desempenha o emprestimo de uma chave para outro nó pela esquerda
int emprestarEsquerda(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  if (posFilho == 0) return 0; // Ou seja, nao possui irmao a direita
  // Procediemento de escrito (dados na RAM)
  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);
  // Salva no rrnEsq o nó a esquerda do nó atual
  int rrnEsq = pai->ponteiroNo[posFilho - 1];
  // Se for nulo retorna
  if (rrnEsq == -1) {
    liberaNo(&pai);
    return 0;
  }

  // Processo de leitura (dados na RAM)
  BTreeNode *filho = criarNo();
  BTreeNode *esq = criarNo();
  lerNo(btreeindex, rrnFilho, filho);
  lerNo(btreeindex, rrnEsq, esq);

  // Se o nó esquerdo nao tem o minimo nao pode emprestar
  if (esq->nroChaves <= MIN_CHAVES) {
    liberaNo(&pai);
    liberaNo(&filho);
    liberaNo(&esq);
    return 0;
  }
  // Move as chaves do filho
  for (int i = filho->nroChaves; i > 0; i--) {
    filho->indice[i] = filho->indice[i - 1];
  }

  // Uma das chaves vai para o pai, outra para o filho
  filho->indice[0] = pai->indice[posFilho - 1];
  pai->indice[posFilho - 1] = esq->indice[esq->nroChaves - 1];

  // Reajuste de ponteiros se nao for nó folha
  if (!ehFolha(filho)) {
    for (int i = filho->nroChaves + 1; i > 0; i--) {
      // Empurra uma casa para direita para abrir espaço no indice 0
      filho->ponteiroNo[i] = filho->ponteiroNo[i - 1];
    }
    // Posicao mais a esquerda do nó filho recebe o ponteiro mais a direita do nó irmao esquerdo
    // Com a intuicao de manter a organizacao da arvoreB
    filho->ponteiroNo[0] = esq->ponteiroNo[esq->nroChaves];
    // Ponteiro mais a direita do nó esquerdo aponta para NULL
    esq->ponteiroNo[esq->nroChaves] = -1;
  }

  // Limpeza da chave doada do irmao da esquerda
  esq->indice[esq->nroChaves - 1].codEstacao = -1;
  esq->indice[esq->nroChaves - 1].offset = -1;
  // Atualiza os contadores
  // Irmao da esquerda perdeu uma chave e o "filho" ganhou uma 
  esq->nroChaves--;
  filho->nroChaves++;
  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnEsq, esq);
  escreverNo(btreeindex, rrnFilho, filho);
  escreverNo(btreeindex, rrnPai, pai);
  // Liberacao na RAM
  liberaNo(&pai);
  liberaNo(&filho);
  liberaNo(&esq);
  return 1;
}

int fundirComIrmaoEsq(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  if (posFilho == 0) return 0;

  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);

  int rrnEsq = pai->ponteiroNo[posFilho - 1];
  BTreeNode *esq = criarNo();
  BTreeNode *filho = criarNo();
  lerNo(btreeindex, rrnEsq, esq);
  lerNo(btreeindex, rrnFilho, filho);

  esq->indice[esq->nroChaves] = pai->indice[posFilho - 1];

  for (int i = 0; i < filho->nroChaves; i++) {
    esq->indice[esq->nroChaves + 1 + i] = filho->indice[i];
  }

  if (!ehFolha(esq)) {
    for (int i = 0; i <= filho->nroChaves; i++) {
      esq->ponteiroNo[esq->nroChaves + 1 + i] = filho->ponteiroNo[i];
    }
  }

  esq->nroChaves += 1 + filho->nroChaves;

  for (int i = posFilho - 1; i < pai->nroChaves - 1; i++) {
    pai->indice[i] = pai->indice[i + 1];
  }

  for (int i = posFilho; i < ORDEM - 1; i++) {
    pai->ponteiroNo[i] = pai->ponteiroNo[i + 1];
  }

  pai->ponteiroNo[ORDEM - 1] = -1;
  pai->indice[pai->nroChaves - 1].codEstacao = -1;
  pai->indice[pai->nroChaves - 1].offset = -1;
  pai->nroChaves--;

  escreverNo(btreeindex, rrnEsq, esq);
  escreverNo(btreeindex, rrnPai, pai);

  liberarPaginaBTree(btreeindex, header, rrnFilho);
  header->nroNos--;

  liberaNo(&pai);
  liberaNo(&esq);
  liberaNo(&filho);
  return 1;
}

int fundirComIrmaoDir(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);

  if (posFilho >= pai->nroChaves) {
    liberaNo(&pai);
    return 0;
  }

  int rrnDir = pai->ponteiroNo[posFilho + 1];
  BTreeNode *filho = criarNo();
  BTreeNode *dir = criarNo();
  lerNo(btreeindex, rrnFilho, filho);
  lerNo(btreeindex, rrnDir, dir);

  filho->indice[filho->nroChaves] = pai->indice[posFilho];

  for (int i = 0; i < dir->nroChaves; i++) {
    filho->indice[filho->nroChaves + 1 + i] = dir->indice[i];
  }

  if (!ehFolha(filho)) {
    for (int i = 0; i <= dir->nroChaves; i++) {
      filho->ponteiroNo[filho->nroChaves + 1 + i] = dir->ponteiroNo[i];
    }
  }

  filho->nroChaves += 1 + dir->nroChaves;

  for (int i = posFilho; i < pai->nroChaves - 1; i++) {
    pai->indice[i] = pai->indice[i + 1];
  }

  for (int i = posFilho + 1; i < ORDEM - 1; i++) {
    pai->ponteiroNo[i] = pai->ponteiroNo[i + 1];
  }

  pai->ponteiroNo[ORDEM - 1] = -1;
  pai->indice[pai->nroChaves - 1].codEstacao = -1;
  pai->indice[pai->nroChaves - 1].offset = -1;
  pai->nroChaves--;

  escreverNo(btreeindex, rrnFilho, filho);
  escreverNo(btreeindex, rrnPai, pai);

  liberarPaginaBTree(btreeindex, header, rrnDir);
  header->nroNos--;

  liberaNo(&pai);
  liberaNo(&filho);
  liberaNo(&dir);
  return 1;
}

int tratarUnderflow(FILE *btreeindex, BTreeHeader *header, int rrnAtual) {
  if (rrnAtual == header->noRaiz) return 1;

  int rrnPai = -1;
  int posFilho = -1;

  if (!buscarPai(btreeindex, header->noRaiz, rrnAtual, &rrnPai, &posFilho)) {
    return 0;
  }

  if (emprestarDireita(btreeindex, header, rrnPai, posFilho, rrnAtual)) return 1;
  if (emprestarEsquerda(btreeindex, header, rrnPai, posFilho, rrnAtual)) return 1;

  if (posFilho > 0) {
    // Salva o rrnEsq antes de fundir
    BTreeNode *pai = criarNo();
    lerNo(btreeindex, rrnPai, pai);
    int rrnEsq = pai->ponteiroNo[posFilho - 1];
    liberaNo(&pai);

    int resultado = fundirComIrmaoEsq(btreeindex, header, rrnPai, posFilho, rrnAtual);

    if (resultado) {
      // Verifica se o pai necessita de tratamento de underflow
      BTreeNode *paiAtualizado = criarNo();
      lerNo(btreeindex, rrnPai, paiAtualizado);
      int nroChavesPai = paiAtualizado->nroChaves;
      liberaNo(&paiAtualizado);

      if (rrnPai == header->noRaiz && nroChavesPai == 0) {
        header->noRaiz = rrnEsq;

        BTreeNode *novaRaiz = criarNo();
        lerNo(btreeindex, rrnEsq, novaRaiz);
        if (!ehFolha(novaRaiz)) {
            novaRaiz->tipoNo = 0; // Atualiza de intermediário (1) para raiz (0)
            escreverNo(btreeindex, rrnEsq, novaRaiz);
        }
        liberaNo(&novaRaiz);

        header->nroNos--;
        liberarPaginaBTree(btreeindex, header, rrnPai);

      }
      else if (nroChavesPai < MIN_CHAVES) {
        return tratarUnderflow(btreeindex, header, rrnPai);
      }
    }
    return resultado;
  }

  // Agora se chegou ate aqui quer dizer que posFilho == 0, ou seja, nao possui irmao a esquerda

  int resultado = fundirComIrmaoDir(btreeindex, header, rrnPai, posFilho, rrnAtual);

    if (resultado) {
      // Verifica se o pai necessita de tratamento de underflow
      BTreeNode *paiAtualizado = criarNo();
      lerNo(btreeindex, rrnPai, paiAtualizado);
      int nroChavesPai = paiAtualizado->nroChaves;
      liberaNo(&paiAtualizado);

      if (rrnPai == header->noRaiz && nroChavesPai == 0) {
        header->noRaiz = rrnAtual;

        BTreeNode *novaRaiz = criarNo();
        lerNo(btreeindex, rrnAtual, novaRaiz);
        if (!ehFolha(novaRaiz)) {
            novaRaiz->tipoNo = 0; // Atualiza de intermediário (1) para raiz (0)
            escreverNo(btreeindex, rrnAtual, novaRaiz);
        }
        liberaNo(&novaRaiz);

        header->nroNos--;
        liberarPaginaBTree(btreeindex, header, rrnPai);

      }
      else if (nroChavesPai < MIN_CHAVES) {
        return tratarUnderflow(btreeindex, header, rrnPai);
      }
    }
  return resultado;
}


int removerArvoreB(FILE *btreeindex, BTreeHeader *header, int rrnAtual, int posChave, int chave) {
  if (!btreeindex || !header || rrnAtual == -1) return 0;

  BTreeNode *node = criarNo();
  if (!node) return 0;

  lerNo(btreeindex, rrnAtual, node);

  // Proteção básica de índice
  if (posChave < 0 || posChave >= node->nroChaves) {
    liberaNo(&node);
    return 0;
  }

  // === CASO 1: NÓ FOLHA ===
  if (ehFolha(node)) {
    removerChaveNo(node, posChave);
    escreverNo(btreeindex, rrnAtual, node);

    // Caso especial: raiz
    if (rrnAtual == header->noRaiz) {
      if (node->nroChaves == 0) {
        header->noRaiz = -1;

        header->nroNos--;
        liberarPaginaBTree(btreeindex, header, rrnAtual);
      }
      liberaNo(&node);
      return 1;
    }

    // Underflow: para ORDEM=4, acontece quando ficou com 0 chaves
    if (node->nroChaves < MIN_CHAVES) {
      liberaNo(&node);
      return tratarUnderflow(btreeindex, header, rrnAtual);
    }

    liberaNo(&node);
    return 1;
  }

  // === CASO 2: NÓ INTERNO (não é folha) ===
  // Busca a sucessora imediata na subárvore direita
  int rrnSucessora = -1;
  int posSucessora = -1;

  // A subárvore direita da chave está em ponteiroNo[posChave + 1]
  int rrnSubarvore = node->ponteiroNo[posChave + 1];

  if (rrnSubarvore == -1) {
    liberaNo(&node);
    return 0;
  }

  buscarSucessorImediato(btreeindex, rrnSubarvore, &rrnSucessora, &posSucessora);

  // Lê a folha onde a sucessora está
  BTreeNode *folhaSucessora = criarNo();
  lerNo(btreeindex, rrnSucessora, folhaSucessora);

  // Copia a sucessora para a posição da chave a remover
  node->indice[posChave] = folhaSucessora->indice[posSucessora];
  escreverNo(btreeindex, rrnAtual, node);

  int chaveSucessora = folhaSucessora->indice[posSucessora].codEstacao;
  liberaNo(&folhaSucessora);
  liberaNo(&node);

  // Agora remove a sucessora da folha onde ela estava
  // Chama recursivamente removendo na folha
  return removerArvoreB(btreeindex, header, rrnSucessora, posSucessora, chaveSucessora);
}

