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

// Faz a remocao mais simples na arvore B, somente nó folha
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

// Checa se é folha ou nao
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
    // Comecamos do fim do no
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

    // Insere a nova chave e seu offset na posicao vaga encontrada
    node->indice[i+1].codEstacao = chave;
    node->indice[i+1].offset = offset;
    
    // O ponteiro do filho direito da nova chave vai para a posicao i + 2
    node->ponteiroNo[i+2] = r_child;

    // Incrementa o numero de chaves do no
    node->nroChaves++;
}

void split(BTreeNode *nodeAtual, int chaveInserida, int offsetInserido, int ptrInserido, BTreeNode *novoNo, int *chavePromovida, int *offsetPromovido) { 
    // Criacao dos arrays temporários que suportam o overflow (tamanho + 1)
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
    
    // 3. Encontra o ponto de divisão e define quem será promovido
    int mid = (MAX_CHAVES + 1) / 2; 
    *chavePromovida = tempChaves[mid];
    *offsetPromovido = tempOffsets[mid];
    
    // 4. Atualiza a quantidade de chaves de cada no pos-divisao
    nodeAtual->nroChaves = mid;
    novoNo->nroChaves = MAX_CHAVES - mid;
    
    // 5. Devolve a primeira metade para o nodeAtual
    for (i = 0; i < mid; i++) {
        nodeAtual->indice[i].codEstacao = tempChaves[i];
        nodeAtual->indice[i].offset = tempOffsets[i];
        nodeAtual->ponteiroNo[i] = tempPonteiros[i];
    }
    nodeAtual->ponteiroNo[mid] = tempPonteiros[mid];
    
    // Limpa o restante das posições do nodeAtual que agora estao vazias
    for (i = mid; i < MAX_CHAVES; i++) {
        nodeAtual->indice[i].codEstacao = -1;
        nodeAtual->indice[i].offset = -1;
        nodeAtual->ponteiroNo[i + 1] = -1;
    }
    
    // 6. Envia a segunda metade para o novoNo
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

// Obtem o RRN da pagina usando reaproveitamento
// de registros logicamente removidos
int calcularRRN(FILE *btreeindex, BTreeHeader *header) {
    // verifica se há algum valor na pilha
    if (header->topo != -1) {
        // Há um nó removido
        int RRN = header->topo;
        
        // Lê o no removido e atualiza o topo da pilha
        BTreeNode *noRemovido = criarNo();
        lerNo(btreeindex, RRN, noRemovido);        

        header->topo = noRemovido->proximo; // <- atualiza o topo da pilha
        
        liberaNo(&noRemovido);
        return RRN;
    } else {
        // Não há pagina removida, pega o proxRRN
        int RRN = header->proxRRN;
        (header->proxRRN)++;
        return RRN;
    }
}

int insercaoArvoreB(FILE *btreeindex, BTreeHeader *header, int RRNAtual, int chave, int offset, int *PROMO_R_CHILD, int *PROMO_KEY, int *PROMO_OFFSET) {  
  // Verifica se a arvore está vazia
  if(header->noRaiz == -1) {
    // Se estiver, inicializa o primeiro nó raiz
    BTreeNode *novaRaiz = criarNo();

    novaRaiz->tipoNo = -1; // nó folha = no raiz
    novaRaiz->indice[0].codEstacao = chave;
    novaRaiz->indice[0].offset = offset;
    novaRaiz->nroChaves = 1;
    
    header->noRaiz = header->proxRRN; // A raiz fica no próximo RRN válido
    (header->nroNos)++;
    (header->proxRRN)++;

    escreverNo(btreeindex, header->noRaiz, novaRaiz);
    liberaNo(&novaRaiz);

    return SEM_PROMOCAO; // Insercao finalizada
  }
  // Se passou de uma folha
  if(RRNAtual == -1) {
    *PROMO_KEY = chave;
    *PROMO_OFFSET = offset;
    *PROMO_R_CHILD = -1;
    return PROMOCAO; // retorna promocao (insere na folha)
  } 
  // Caso contrario
  else { 
    // Lê a página atual na memoria principal
    BTreeNode *node = criarNo();
    lerNo(btreeindex, RRNAtual, node);

    // imprimeNo(*node);

    int pos;
    int array[MAX_CHAVES];
    arrayDeChaves(array, node); 
                                
    // Busca binária no array de chaves
    if(binarySearch(chave, array, 0, node->nroChaves, &pos) != -1) {
      // printf("Nao é possivel inserir chaves duplicadas.\n");
      liberaNo(&node);
      return ERRO;
    }

    // Chama a funcao recursivamente descobrindo se haverá promocao
    int RETORNO = insercaoArvoreB(btreeindex, header, node->ponteiroNo[pos], chave, offset, PROMO_R_CHILD, PROMO_KEY, PROMO_OFFSET);

    if(RETORNO == SEM_PROMOCAO || RETORNO == ERRO) { 
      liberaNo(&node);
      return RETORNO;
    }
    else if(node->nroChaves < MAX_CHAVES) { // Há promocao e tem espaco no nó
      inserirOrdenado(node, *PROMO_KEY, *PROMO_OFFSET, *PROMO_R_CHILD);
      escreverNo(btreeindex, RRNAtual, node);
      liberaNo(&node);
      return SEM_PROMOCAO;
    }
    else { // OVERFLOW: Há promocao e o nó está cheio
      BTreeNode *novoNo = criarNo();
      int novaChavePromovida, novoOffsetPromovido;
      
      split(node, *PROMO_KEY, *PROMO_OFFSET, *PROMO_R_CHILD, novoNo, &novaChavePromovida, &novoOffsetPromovido);

      int novoPonteiroPromovido = calcularRRN(btreeindex, header);
      (header->nroNos)++;

      // Se o nó que sofreu split é a raiz do sistema
      if (RRNAtual == header->noRaiz) {        
        // A raiz antiga não é mais raiz
        // Reclassifica os nós do split:
        int novoTipo;

        // Se o no e folha
        if (node->ponteiroNo[0] == -1) {
            novoTipo = -1; // novo no é folha
        } 
        else {
            novoTipo = 1; // novo no é interno 
        }

        node->tipoNo = novoTipo;
        novoNo->tipoNo = novoTipo;

        // Salva o antigo nó raiz
        escreverNo(btreeindex, RRNAtual, node);
        escreverNo(btreeindex, novoPonteiroPromovido, novoNo);

        // Cria a nova raiz
        BTreeNode *novaRaiz = criarNo();
        novaRaiz->tipoNo = 0; // nó raiz
        novaRaiz->nroChaves = 1;
        novaRaiz->indice[0].codEstacao = novaChavePromovida;
        novaRaiz->indice[0].offset = novoOffsetPromovido;
        
        // Ajusta os ponteiros
        novaRaiz->ponteiroNo[0] = RRNAtual;
        novaRaiz->ponteiroNo[1] = novoPonteiroPromovido;

        // Atualiza os campos do cabecalho
        int rrnNovaRaiz = calcularRRN(btreeindex, header);
        header->noRaiz = rrnNovaRaiz;
        (header->nroNos)++;

        escreverNo(btreeindex, rrnNovaRaiz, novaRaiz);

        liberaNo(&novaRaiz);
        liberaNo(&node);
        liberaNo(&novoNo);

        return SEM_PROMOCAO; // Insercao finalizada
      } 
      else {
        // Se nao era a raiz, salva no disco e repassa a promocao
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
  // Procedimeento de leitura 
  BTreeNode *node = criarNo();
  lerNo(btreeindex, rrnSubarvore, node);

  int rrnAtual = rrnSubarvore;

  // Desce sempre pela subarvore da esquerda até chegar numa folha
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

    // Atualiza os contadores para a próxima iteracao
    dir->nroChaves--;
    filho->nroChaves++;
  }

  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnFilho, filho);
  escreverNo(btreeindex, rrnDir, dir);
  escreverNo(btreeindex, rrnPai, pai);
  // Liberacao na RAM
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

// Funcao que faz o merge de um nó com underflow com o seu irmao a esquerda
int fundirComIrmaoEsq(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  // Se for o nó mais a esquerda, nao tem como ter um nó mais a esquerda para fusao
  if (posFilho == 0) return 0;

  // Processo de leitura (dados na RAM)
  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);

  // Salva no rrnEsq o nó a esquerda do nó atual
  int rrnEsq = pai->ponteiroNo[posFilho - 1];
  
  // Processo de leitura (dados na RAM)
  BTreeNode *esq = criarNo();
  BTreeNode *filho = criarNo();
  lerNo(btreeindex, rrnEsq, esq);
  lerNo(btreeindex, rrnFilho, filho);

  // Chave do pai desce para o irmao da esquerda do filho
  esq->indice[esq->nroChaves] = pai->indice[posFilho - 1];

  // Nó esquerdo absorve todas as chaves do nó filho
  for (int i = 0; i < filho->nroChaves; i++) {
    esq->indice[esq->nroChaves + 1 + i] = filho->indice[i];
  }

  // Reajuste de ponteiros se nao for nó folha
  // Os ponteiros do nó filho tambem sao transferidos para o esq
  if (!ehFolha(esq)) {
    for (int i = 0; i <= filho->nroChaves; i++) {
      esq->ponteiroNo[esq->nroChaves + 1 + i] = filho->ponteiroNo[i];
    }
  }

  // Atualiza os contadores
  // Atualiza o numero de chaves que o nó esq tem
  esq->nroChaves += 1 + filho->nroChaves;

  // Como o pai perdeu uma chave, entao todas as chaves a direita dela precisa 
  // mover uma casa para a esquerda
  for (int i = posFilho - 1; i < pai->nroChaves - 1; i++) {
    pai->indice[i] = pai->indice[i + 1];
  }

  // Como o pai perdeu um ponteiro tambem, entao os ponteiros a direita dele
  // dao um passo para a esquerda
  for (int i = posFilho; i < ORDEM - 1; i++) {
    pai->ponteiroNo[i] = pai->ponteiroNo[i + 1];
  }

  // Limpa o lixo e atualiza o seu contador
  pai->ponteiroNo[ORDEM - 1] = -1;
  pai->indice[pai->nroChaves - 1].codEstacao = -1;
  pai->indice[pai->nroChaves - 1].offset = -1;
  pai->nroChaves--;

  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnEsq, esq);
  escreverNo(btreeindex, rrnPai, pai);

  // A pagina filho perde a utilidade e necessita ser colocada
  // na pilha de removidos, o topo do cabeçalho
  liberarPaginaBTree(btreeindex, header, rrnFilho);
  header->nroNos--; // Diminui o total de nós 

  // Liberacao na RAM
  liberaNo(&pai);
  liberaNo(&esq);
  liberaNo(&filho);
  return 1;
}

// Funcao que faz o merge de um nó com underflow com o seu irmao a direita
int fundirComIrmaoDir(FILE *btreeindex, BTreeHeader *header, int rrnPai, int posFilho, int rrnFilho) {
  // Processo de leitura (dados na RAM)
  BTreeNode *pai = criarNo();
  lerNo(btreeindex, rrnPai, pai);

  // Se for o nó mais a direita, nao tem como fundir com alguem a direita
  if (posFilho >= pai->nroChaves) {
    liberaNo(&pai);
    return 0;
  }

  // Salva no rrnDir o nó a direita do nó atual
  int rrnDir = pai->ponteiroNo[posFilho + 1];
  
  // Processo de leitura (dados na RAM)
  BTreeNode *filho = criarNo();
  BTreeNode *dir = criarNo();
  lerNo(btreeindex, rrnFilho, filho);
  lerNo(btreeindex, rrnDir, dir);

  // Chave do pai desce para o nó filho
  filho->indice[filho->nroChaves] = pai->indice[posFilho];

  // Nó filho absorve todas as chaves do nó direito
  for (int i = 0; i < dir->nroChaves; i++) {
    filho->indice[filho->nroChaves + 1 + i] = dir->indice[i];
  }

  // Reajuste de ponteiros se nao for nó folha
  // Os ponteiros do nó direito tambem sao transferidos para o filho
  if (!ehFolha(filho)) {
    for (int i = 0; i <= dir->nroChaves; i++) {
      filho->ponteiroNo[filho->nroChaves + 1 + i] = dir->ponteiroNo[i];
    }
  }

  // Atualiza os contadores
  // Atualiza o numero de chaves que o nó filho tem
  filho->nroChaves += 1 + dir->nroChaves;

  // Como o pai perdeu uma chave, entao todas as chaves a direita dela precisa
  // mover uma casa para a esquerda
  for (int i = posFilho; i < pai->nroChaves - 1; i++) {
    pai->indice[i] = pai->indice[i + 1];
  }

  // Como o pai perdeu um ponteiro tambem, entao os ponteiros a direita dele
  // dao um passo para a esquerda
  for (int i = posFilho + 1; i < ORDEM - 1; i++) {
    pai->ponteiroNo[i] = pai->ponteiroNo[i + 1];
  }

  // Limpa o lixo e atualiza o seu contador
  pai->ponteiroNo[ORDEM - 1] = -1;
  pai->indice[pai->nroChaves - 1].codEstacao = -1;
  pai->indice[pai->nroChaves - 1].offset = -1;
  pai->nroChaves--;

  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnFilho, filho);
  escreverNo(btreeindex, rrnPai, pai);

  // A pagina do irmao direito perde a utilidade e necessita ser colocada
  // na pilha de removidos, o topo do cabeçalho
  liberarPaginaBTree(btreeindex, header, rrnDir);
  header->nroNos--; // Diminui o total de nós

  // Liberacao na RAM
  liberaNo(&pai);
  liberaNo(&filho);
  liberaNo(&dir);
  return 1;
}

// Funcao responsável por coordenar o tratamento de underflow de um nó
int tratarUnderflow(FILE *btreeindex, BTreeHeader *header, int rrnAtual) {
  // Condicao de parada: a raiz não sofre underflow por possuir poucas chaves
  if (rrnAtual == header->noRaiz) return 1;

  int rrnPai = -1;
  int posFilho = -1;

  // Busca e identifica quem é o pai e qual a posicao do nó atual em relacao a ele
  if (!buscarPai(btreeindex, header->noRaiz, rrnAtual, &rrnPai, &posFilho)) {
    return 0;
  }

  // Tentativa 1: Redistribuicao
  // Tenta primeiro da direita, se falhar, tenta da esquerda
  if (emprestarDireita(btreeindex, header, rrnPai, posFilho, rrnAtual)) return 1;
  if (emprestarEsquerda(btreeindex, header, rrnPai, posFilho, rrnAtual)) return 1;

  // Tentativa 2: Merge com o irmão da ESQUERDA
  // Se não for o primeiro filho (posFilho > 0), obrigatoriamente possui irmão à esquerda
  if (posFilho > 0) {
    // Processo de leitura (dados na RAM)
    // Salva o rrnEsq antes de fundir
    BTreeNode *pai = criarNo();
    lerNo(btreeindex, rrnPai, pai);
    int rrnEsq = pai->ponteiroNo[posFilho - 1];
    liberaNo(&pai); // Liberacao na RAM

    int resultado = fundirComIrmaoEsq(btreeindex, header, rrnPai, posFilho, rrnAtual);

    if (resultado) {
      // Processo de leitura (dados na RAM)
      // Verifica quantas chaves restaram no pai após ceder uma para a fusão
      BTreeNode *paiAtualizado = criarNo();
      lerNo(btreeindex, rrnPai, paiAtualizado);
      int nroChavesPai = paiAtualizado->nroChaves;
      liberaNo(&paiAtualizado); // Liberacao na RAM

      // Caso especial: o pai era a raiz e ficou vazio (Diminuicao da altura da arvore)
      if (rrnPai == header->noRaiz && nroChavesPai == 0) {
        header->noRaiz = rrnEsq; // O irmão esquerdo se torna a raiz

        // Processo de leitura (dados na RAM)
        // Atualiza os dados da nova raiz no disco
        BTreeNode *novaRaiz = criarNo();
        lerNo(btreeindex, rrnEsq, novaRaiz);
        if (!ehFolha(novaRaiz)) {
            novaRaiz->tipoNo = 0; // Atualiza de intermediário (1) para raiz (0)
            
            // Processo de escrita (de volta no binario)
            escreverNo(btreeindex, rrnEsq, novaRaiz);
        }
        liberaNo(&novaRaiz); // Liberacao na RAM

        // A pagina da raiz antiga perde o uso e necessita ser colocada
        // na pilha de removidos, o topo do cabeçalho
        header->nroNos--; // Diminui o total de nós
        liberarPaginaBTree(btreeindex, header, rrnPai);

      }
      // Propagacao: o pai não era a raiz, mas entrou em underflow
      else if (nroChavesPai < MIN_CHAVES) {
        return tratarUnderflow(btreeindex, header, rrnPai); // Chama a recursão para consertar o pai
      }
    }
    return resultado;
  }

  // Tentativa 3: Merge com o irmão da DIREITA
  // Agora se chegou ate aqui quer dizer que posFilho == 0, ou seja, nao possui irmao a esquerda
  int resultado = fundirComIrmaoDir(btreeindex, header, rrnPai, posFilho, rrnAtual);

    if (resultado) {
      // Processo de leitura (dados na RAM)
      // Verifica quantas chaves restaram no pai após ceder uma para a fusão
      BTreeNode *paiAtualizado = criarNo();
      lerNo(btreeindex, rrnPai, paiAtualizado);
      int nroChavesPai = paiAtualizado->nroChaves;
      liberaNo(&paiAtualizado); // Liberacao na RAM

      // Caso especial: o pai era a raiz e ficou vazio (Diminuicao da altura da arvore)
      if (rrnPai == header->noRaiz && nroChavesPai == 0) {
        header->noRaiz = rrnAtual; // O próprio nó se torna a raiz

        // Processo de leitura (dados na RAM)
        // Atualiza os dados da nova raiz no disco para refletir seu novo status
        BTreeNode *novaRaiz = criarNo();
        lerNo(btreeindex, rrnAtual, novaRaiz);
        if (!ehFolha(novaRaiz)) {
            novaRaiz->tipoNo = 0; // Atualiza de intermediário (1) para raiz (0)
            
            // Processo de escrita (de volta no binario)
            escreverNo(btreeindex, rrnAtual, novaRaiz);
        }
        liberaNo(&novaRaiz); // Liberacao na RAM

        // A pagina da raiz antiga perde a utilidade e precisa ser colocada
        // na pilha de removidos, o topo do cabeçalho
        header->nroNos--; // Diminui o total de nós
        liberarPaginaBTree(btreeindex, header, rrnPai);

      }
      // Propagacao: o pai não era a raiz, mas entrou em underflow
      else if (nroChavesPai < MIN_CHAVES) {
        return tratarUnderflow(btreeindex, header, rrnPai); // Chama a recursão para consertar o pai
      }
    }
  return resultado;
}


int removerArvoreB(FILE *btreeindex, BTreeHeader *header, int rrnAtual, int posChave, int chave) {
  // Garantir que os ponteiros e o RRN são válidos
  if (!btreeindex || !header || rrnAtual == -1) return 0;

  // Processo de leitura (dados na RAM)
  BTreeNode *node = criarNo();
  if (!node) return 0;

  lerNo(btreeindex, rrnAtual, node);

  // Protecao básica de índice
  // Garante que não vamos tentar remover uma chave fora dos limites do array do nó
  if (posChave < 0 || posChave >= node->nroChaves) {
    liberaNo(&node); // Liberacao na RAM
    return 0;
  }

  // CASO 1: Nó folha 
  if (ehFolha(node)) {
    // Chama a funcao auxiliar que empurra as chaves para a esquerda, sobrescrevendo a chave a ser apagada
    removerChaveNo(node, posChave);
    
    // Processo de escrita (de volta no binario)
    escreverNo(btreeindex, rrnAtual, node);

    // Caso especial: raiz
    // Se a remocao ocorreu na raiz (que também é folha neste momento) e ela ficou completamente vazia
    if (rrnAtual == header->noRaiz) {
      if (node->nroChaves == 0) {
        header->noRaiz = -1; // A arvore deixa de existir e fica vazia

        // A pagina da raiz antiga perde a utilidade e necessita ser colocada
        // na pilha de removidos, o topo do cabeçalho
        header->nroNos--;
        liberarPaginaBTree(btreeindex, header, rrnAtual);
      }
      liberaNo(&node); // Liberacao na RAM
      return 1;
    }

    // Underflow acontece quando ficou com 0 chaves
    // Verifica se a remocao deixou a folha com menos chaves do que o mínimo permitido
    if (node->nroChaves < MIN_CHAVES) {
      liberaNo(&node); // Liberacao na RAM
      // Chama a rotina de tratamento que vai tentar emprestar ou fundir
      return tratarUnderflow(btreeindex, header, rrnAtual);
    }

    liberaNo(&node); // Liberacao na RAM
    return 1; // Remocao concluída com sucesso sem underflow
  }

  // CASO 2: Nó interno
  // Como não podemos simplesmente arrancar uma chave de um nó intermediario,
  // precisamos trocá-la por uma chave equivalente que esteja numa folha para não quebrar a arvore.

  // Busca a sucessora imediata na subarvore direita
  int rrnSucessora = -1;
  int posSucessora = -1;

  // A subarvore direita da chave está em ponteiroNo[posChave + 1]
  // A sucessora será o menor elemento de toda essa subarvore direita
  int rrnSubarvore = node->ponteiroNo[posChave + 1];

  // Se por algum motivo estrutural o ponteiro for nulo, aborta
  if (rrnSubarvore == -1) {
    liberaNo(&node); // Liberacao na RAM
    return 0;
  }

  // Desce até o extremo esquerdo da subarvore direita para encontrar a sucessora
  buscarSucessorImediato(btreeindex, rrnSubarvore, &rrnSucessora, &posSucessora);

  // Processo de leitura (dados na RAM)
  // Lê a folha onde a sucessora está
  BTreeNode *folhaSucessora = criarNo();
  lerNo(btreeindex, rrnSucessora, folhaSucessora);

  // Copia a sucessora para a posicao da chave a remover
  // Isso sobrescreve a chave que seria deletada no nó interno
  node->indice[posChave] = folhaSucessora->indice[posSucessora];
  
  // Processo de escrita (de volta no binario)
  escreverNo(btreeindex, rrnAtual, node);

  // Salva a chave sucessora na memória RAM antes de liberar o nó
  // Precisa desse valor logo abaixo para chamar a remocao recursiva lá na folha onde ela estava originalmente
  int chaveSucessora = folhaSucessora->indice[posSucessora].codEstacao;
  
  // Liberacao na RAM
  liberaNo(&folhaSucessora);
  liberaNo(&node);

  // Agora remove a sucessora da folha onde ela estava
  // Chama recursivamente removendo na folha
  return removerArvoreB(btreeindex, header, rrnSucessora, posSucessora, chaveSucessora);
}

