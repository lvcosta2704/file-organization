#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BTree.h"
#include "RegistrosIO.h"
#include "fornecidas.h"
#include "query.h"

// === FUNCIONALIDADES ===

void criarArvoreB(char *binName, char *btreeName) {
  // Abre o arquivo de registros no modo leitura e verifica se ocorreu bem
  FILE *fileBin = fopen(binName, "rb");
  if(!fileBin) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Abre o arquivo de indice Arvore-B no modo escrita e verifica se ocorreu bem
  FILE *btreeindex = fopen(btreeName, "wb+");
  if(!btreeindex) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Le o cabecalho do arquivo de dados e verifica se esta consistente
  Cabecalho cabBin;

  lerCabecalho(fileBin, &cabBin);
  if(cabBin.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    return;
  }

  // Inicializa o cabecalho do arquivo de indice Arvore-B
  BTreeHeader cabBTree;

  cabBTree.status = '0'; // Status é inconsistente até o arquivo ser criado completamente
  cabBTree.noRaiz = -1; 
  cabBTree.topo = -1;
  cabBTree.proxRRN = 0;
  cabBTree.nroNos = 0;

  escreverCabecalhoArvoreB(btreeindex, &cabBTree);
   
  // 1. Le o registro a verifica se esta marcado como logicamente removido
  fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Garante que a referencia para o arquivo de dados esta logo após o cabecalho
  
  Registro reg;

  // Passa por cada registro de dados sequencialmente
  // verificando se ele esta removido a principio
  while(fread(&reg.removido, sizeof(char), 1, fileBin) == 1) {
    // Se estiver removido, pula para o próximo
    if(reg.removido == '1') {
      fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    } 

    // 2. Define a chave como o codEstacao e define o offset do campo que o contém

    int offset = ftell(fileBin) - 1; // Guarda o offset do campo

    lerRegistro(fileBin, &reg);

    int chave = reg.codEstacao; // Guarda a chave do arquivo de 
                                
    // 3. Realiza a insercao da chave e do offset no arquivo de indice Arvore-B
    int PROMO_R_CHILD;
    int PROMO_KEY;
    int PROMO_OFFSET;

    insercaoArvoreB(btreeindex,
        &cabBTree,
        cabBTree.noRaiz,
        chave,
        offset,
        &PROMO_R_CHILD,
        &PROMO_KEY,
        &PROMO_OFFSET);
  } 

  cabBTree.status = '1'; // Marca o arquivo como consistente novamente
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);

  // Fecha os arquivos
  fclose(fileBin);
  fclose(btreeindex);

  BinarioNaTela(btreeName);
}

// Realiza n buscas usando o arquivo de índice Arvore-B
// para codEstacao
// Para
void busca(char *binName, char *btreeName, int N) {
  // Abre o arquivo de registros no modo leitura e verifica se ocorreu bem
  FILE *fileBin = fopen(binName, "rb");
  if (!fileBin) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Abre o arquivo de indice Arvore-B no modo leitura e verifica se ocorreu bem
  FILE *btreeindex = fopen(btreeName, "rb");
  if (!btreeindex) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Le o cabecalho do arquivo binario e verifica se esta consistente
  Cabecalho cabBin;

  lerCabecalho(fileBin, &cabBin);
  if (cabBin.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    return;
  }

  // Le o cabecalho do arquivo de indice Arvore-B e verifica se esta consistente
  BTreeHeader cabBTree;

  lerCabecalhoArvoreB(btreeindex, &cabBTree);
  if(cabBTree.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(btreeindex);
    return;
  }

  // Realiza a lógica de busca
  for(int i = 0; i < N; i++) { // Executa N vezes
    Busca filtro = inputFiltro(); // Cria um filtro para a busca (Simula o WHERE)

    // Se a busca pedir o codEstacao, usa o arquivo de indice Arvore-B
    if(filtro.codEstacao != -2) {
      // Busca pelo RRN do codEstacao do arquivo de dados, usando o arquivo de indice Arvore-B
      int OFFSET;
      //printf("No raiz: %d\n", cabBTree.noRaiz);

      if(!(buscaArvoreB(btreeindex, cabBTree.noRaiz, filtro.codEstacao, NULL, NULL, &OFFSET))) {
        printf("Registro inexistente.\n\n"); // Se estiver, nao existe o registro
        continue;
      }

      //printf("Breakpoint. \n");

      //printf("%d\n", filtro.codEstacao);
      //printf("%d\n", cabBTree.noRaiz);
      
      // Realiza a busca no registro de RRN especificado
      Registro reg;
      
      // Posiciona a referencia para o arquivo de dados no registro especificado
      fseek(fileBin, OFFSET, SEEK_SET);

      // Verifica se o registro esta marcado como logicamente removido
      fread(&reg.removido, sizeof(char), 1, fileBin);
      if(reg.removido == '1') {
        printf("Registro inexistente.\n\n"); // Se estiver, nao existe o registro
        continue; // Realiza a proxima busca
      }
      else { // Se nao,
        // Verifica se todos os outros filtros satisfazem à busca
        lerRegistro(fileBin, &reg);

        int coincide = comparaFiltro(filtro, reg);

        if(coincide) {
        // Imprime usando a mesma lógica da listarRegistros, porem se tiver -1 imprime NULO
            // 1. codEstacao
            if (reg.codEstacao != -1) printf("%d ", reg.codEstacao); 
            else printf("NULO ");
            
            // 2. nomeEstacao
            if (reg.tamNomeEstacao > 0) printf("%s ", reg.nomeEstacao); 
            else printf("NULO ");
            
            // 3. codLinha
            if (reg.codLinha != -1) printf("%d ", reg.codLinha); 
            else printf("NULO ");
            
            // 4. nomeLinha
            if (reg.tamNomeLinha > 0) printf("%s ", reg.nomeLinha); 
            else printf("NULO ");
            
            // 5. codProxEstacao
            if (reg.codProxEstacao != -1) printf("%d ", reg.codProxEstacao); 
            else printf("NULO ");
            
            // 6. distProxEstacao
            if (reg.distProxEstacao != -1) printf("%d ", reg.distProxEstacao); 
            else printf("NULO ");
            
            // 7. codLinhaIntegra
            if (reg.codLinhaIntegra != -1) printf("%d ", reg.codLinhaIntegra); 
            else printf("NULO ");
            
            // 8. codEstIntegra (Último campo tem a quebra de linha \n)

            if (reg.codEstIntegra != -1) printf("%d\n\n", reg.codEstIntegra); 
            else printf("NULO\n\n");

        }
      }
    }
    else { // Se nao, realiza a busca usando somente arquivo de dados (Igual à funcionalidade 3)
      fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Coloca a referencia para o arquivo no primeiro registro de dados

      int encontrouAlgum = 0;
      Registro reg;

      // Passa por cada registro de dados sequencialmente
      // verificando se ele esta removido a principio
      while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
          // Se estiver removido pula para o proximo
          if (reg.removido == '1') {
              fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
              continue;
          }
          // --- LEITURA DOS REGISTROS ---
          // Le o restante do registro para comparar
          // Le o registro com os freads e tambem pula o lixo do registro no final
          
          lerRegistro(fileBin, &reg);

          int coincide = comparaFiltro(filtro, reg);

          if (coincide) {
          // Imprime usando a mesma lógica da listarRegistros, porem se tiver -1 imprime NULO
              // 1. codEstacao
              if (reg.codEstacao != -1) printf("%d ", reg.codEstacao); 
              else printf("NULO ");
              
              // 2. nomeEstacao
              if (reg.tamNomeEstacao > 0) printf("%s ", reg.nomeEstacao); 
              else printf("NULO ");
              
              // 3. codLinha
              if (reg.codLinha != -1) printf("%d ", reg.codLinha); 
              else printf("NULO ");
              
              // 4. nomeLinha
              if (reg.tamNomeLinha > 0) printf("%s ", reg.nomeLinha); 
              else printf("NULO ");
              
              // 5. codProxEstacao
              if (reg.codProxEstacao != -1) printf("%d ", reg.codProxEstacao); 
              else printf("NULO ");
              
              // 6. distProxEstacao
              if (reg.distProxEstacao != -1) printf("%d ", reg.distProxEstacao); 
              else printf("NULO ");
              
              // 7. codLinhaIntegra
              if (reg.codLinhaIntegra != -1) printf("%d ", reg.codLinhaIntegra); 
              else printf("NULO ");
              
              // 8. codEstIntegra (Último campo tem a quebra de linha \n)
              if (reg.codEstIntegra != -1) printf("%d\n", reg.codEstIntegra); 
              else printf("NULO\n");

              encontrouAlgum = 1;
            }

        }
        if (!encontrouAlgum) {
            printf("Registro inexistente.\n");
        }

        printf("\n");
    }
  }

  // Fecha os arquivos
  fclose(fileBin);
  fclose(btreeindex);
}

void insercao(char *binName, char *btreeName, int N) {
  // Abre o arquivo de registros no modo leitura/escrita e verifica se ocorreu bem
  FILE *fileBin = fopen(binName, "rb+");
  if (!fileBin) {
    printf("Falha no processamento de dados\n");
    return;
  }

  // Abre o arquivo de indice Arvore-B no modo leitura/escrita e verifica se ocorreu bem
  FILE *btreeindex = fopen(btreeName, "rb+");
  if (!btreeindex) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Le os cabeçalhos dos arquivos para a memória RAM
  Cabecalho cabBin;
  lerCabecalho(fileBin, &cabBin);

  BTreeHeader cabBTree;
  lerCabecalhoArvoreB(btreeindex, &cabBTree);

  // Verifica se ambos os arquivos estão consistentes
  if (cabBin.status == '0' || cabBTree.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    fclose(btreeindex);
    return;
  }

  // Marca os arquivos como inconsistentes durante o processo de inserção
  cabBin.status = '0';
  cabBTree.status = '0';
  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);
  
  // Realiza N insercoes
  for (int i = 0; i < N; i++) {
    // Pede ao usuario o registro a ser adicionado
    Registro reg = inputRegistro();
    int chave = reg.codEstacao; // salva a chave

    // 1. Calcula onde esse registro deveria ficar no arquivo de dados, sem escrever no disco
    int RRN;
    int offset;

    // Se a pilha de removidos não estiver vazia, reaproveita o espaco
    if(cabBin.topo != -1) {
      RRN = cabBin.topo;
    } 
    // Caso contrario, insere no final do arquivo (expande o arquivo)
    else { 
      RRN = cabBin.proxRRN;
    }
    
    offset = TAM_CABECALHO + (RRN * TAM_REGISTRO); // salva o offset

    // 2. Insere no arquivo de indice Arvore-B
    int PROMO_R_CHILD;
    int PROMO_KEY;
    int PROMO_OFFSET;

    // printf("Inserindo codEstacao %d\n", chave);
    
    int status = insercaoArvoreB(btreeindex,
        &cabBTree,
        cabBTree.noRaiz,
        chave,
        offset,
        &PROMO_R_CHILD,
        &PROMO_KEY,
        &PROMO_OFFSET);

    // BTreeNode *node = criarNo();
    // lerNo(btreeindex, 0, node);

    // imprimeNo(*node);

    // Se a chave que estamos tentando inserir ja existe, nao insere.
    if(status == ERRO) {
      continue;
    }

    // 3. Atualiza o arquivo de dados fisicamente
    if(cabBin.topo != -1) { // Caso haja algum registro logicamente removido
        // Insere no RRN correspondente ao topo da pilha
        // Coloca a próxima referencia da lista no topo da pilha para atualizar a encadeada
        fseek(fileBin, offset + 1, SEEK_SET);
        fread(&cabBin.topo, sizeof(int), 1, fileBin);

        // Vai pro RRN do antigo topo da pilha e escreve o registro novo por cima
        fseek(fileBin, offset, SEEK_SET);
        escreverRegistro(fileBin, reg);
    }
    else { // Caso contrario (inserção no final do arquivo)
        // Vai pro final do arquivo e escreve o registro
        fseek(fileBin, offset, SEEK_SET);
        escreverRegistro(fileBin, reg);

        // Atualiza o proximo RRN do cabeçalho, arquivo cresceu
        cabBin.proxRRN++;
    }
  }

  // Recalcula o número real de estacoes e pares para atualizar o cabeçalho
  contarEstacoesEPares(fileBin, &cabBin);

  // Marca o arquivo como consistente novamente e salva as alteracoes
  cabBin.status = '1';
  cabBTree.status = '1';
  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);

  // Fecha os arquivos
  fclose(fileBin);
  fclose(btreeindex);
 
  // Exibe os arquivos binarios para avaliação no testador automatico
  BinarioNaTela(binName);
  BinarioNaTela(btreeName);
}

void remocao(char *binName, char *btreeName, int N) {
  // Abre o arquivo de registros no modo leitura/escrita e verifica se ocorreu bem
  FILE *fileBin = fopen(binName, "rb+");
  if (!fileBin) { printf("Falha no processamento do arquivo.\n"); return; }

  // Abre o arquivo de indice ArvoreB no modo leitura/escrita e verifica se ocorreu bem
  FILE *btreeindex = fopen(btreeName, "rb+");
  if (!btreeindex) { printf("Falha no processamento do arquivo.\n"); return; }

  // Le os cabeçalhos dos arquivos para a memória RAM
  Cabecalho cabBin;
  lerCabecalho(fileBin, &cabBin);

  BTreeHeader cabBTree;
  lerCabecalhoArvoreB(btreeindex, &cabBTree);

  // Verifica se ambos os arquivos estão consistentes
  if (cabBin.status == '0' || cabBTree.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin); fclose(btreeindex);
    return;
  }

  // Marca os arquivos como inconsistentes durante a operação de remoção
  cabBin.status = '0';
  cabBTree.status = '0';
  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);

  // Realiza N operacoes de remoção
  for (int i = 0; i < N; i++) {
    // Cria um filtro com os campos fornecidos pelo usuario
    Busca filtro = inputFiltro();

    if (filtro.codEstacao == -2) {
      // CASO: busca sequencial por outro campo (quando não é fornecida a chave primaria)
      
      // Coloca a referencia para o arquivo no primeiro registro de dados (pulando o cabeçalho)
      fseek(fileBin, TAM_CABECALHO, SEEK_SET);
      Registro reg;
      int rrnAtual = 0;

      // Passa por cada registro verificando seu status de remoção
      while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1) {
        // Se ja estiver logicamente removido, pula o restante do registro e avança o RRN
        if (reg.removido == '1') {
          fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
          rrnAtual++;
          continue;
        }

        // Le o conteúdo do registro para a memória para fazer a verificação
        lerRegistro(fileBin, &reg);
      
        // Salva a posição correta do ponteiro de leitura do arquivo
        // pois a remoção ira mover o ponteiro internamente
        long posicaoCorreta = ftell(fileBin);

        // Se o registro lido coincidir com o filtro de busca fornecido
        if (comparaFiltro(filtro, reg)) {
          // Apaga logicamente o registro do arquivo de dados (empilhando no topo dos removidos)
          apagarRegistro(fileBin, &reg, &cabBin, rrnAtual);

          // Procura a chave correspondente na Arvore-B para remove-la também
          int foundRRN = -1, foundPos = -1;
          int encontrou = buscaArvoreB(btreeindex, cabBTree.noRaiz,
                                        reg.codEstacao, &foundRRN, &foundPos, NULL);
          // Se a chave for encontrada na Arvore-B, realiza a remoção estrutural
          if (encontrou) {
            removerArvoreB(btreeindex, &cabBTree, foundRRN, foundPos, reg.codEstacao);
          }

        // Restaura o ponteiro do arquivo para a posição exata após a leitura do registro atual
        fseek(fileBin, posicaoCorreta, SEEK_SET);

        }
        rrnAtual++;
      }

    } else {
      // CASO: busca pelo índice Arvore-B usando a chave primaria (codEstacao)
      int foundRRN = -1, foundPos = -1, foundOffset = -1;

      // Busca diretamente pelo codEstacao na Arvore-B
      int encontrou = buscaArvoreB(btreeindex, cabBTree.noRaiz,
                                    filtro.codEstacao, &foundRRN, &foundPos, &foundOffset);
      // Se a chave não existir na Arvore, ignora a remoção e vai para a próxima
      if (!encontrou) continue;

      Registro reg;
      // Pula direto para o byte (offset) exato no arquivo de dados retornado pela busca no índice
      fseek(fileBin, foundOffset, SEEK_SET);
      fread(&reg.removido, sizeof(char), 1, fileBin);

      // Verificação de segurança: se o registro ja estiver marcado como removido, ignora
      if (reg.removido == '1') continue;

      // Le o registro alvo para a memória
      lerRegistro(fileBin, &reg);

      // Calcula o RRN com base no offset físico do arquivo
      int rrnDados = (foundOffset - TAM_CABECALHO) / TAM_REGISTRO;
      
      // Apaga logicamente o registro do arquivo de dados
      apagarRegistro(fileBin, &reg, &cabBin, rrnDados);

      // Remove definitivamente a chave do índice Arvore-B
      removerArvoreB(btreeindex, &cabBTree, foundRRN, foundPos, filtro.codEstacao);
    }
  }

  // Recalcula o número real de estacoes e pares (ignorando os recém-removidos) para atualizar o cabeçalho
  contarEstacoesEPares(fileBin, &cabBin);

  // Marca ambos os arquivos como consistentes novamente após o término seguro das remocoes
  cabBin.status = '1';
  cabBTree.status = '1';
  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);

  // Fecha e salva as alteracoes em disco
  fclose(fileBin);
  fclose(btreeindex);

  // Exibe os arquivos binarios
  BinarioNaTela(binName);
  BinarioNaTela(btreeName);
}

// Realiza a junção entre dois arquivos usando a técnica de força bruta (nested loop join)
void nestedLoopJoin(char *binName1, char *nomeCampo1, char *binName2, char *nomeCampo2) {
  
  // Processo de abertura de arquivos em modo de leitura/escrita
  FILE *fileBin1 = fopen(binName1, "rb+");
  FILE *fileBin2 = fopen(binName2, "rb+");

  // Verifica se a abertura de todos os arquivos foi bem-sucedida
  if (!fileBin1 || !fileBin2) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBin1) fclose(fileBin1);
    if (fileBin2) fclose(fileBin2);
    return;
  }

  Cabecalho cab1, cab2;

  // Carrega os cabeçalhos dos arquivos para a memória principal
  lerCabecalho(fileBin1, &cab1);
  lerCabecalho(fileBin2, &cab2);
  
  // Verificação de consistência dos arquivos
  if (cab1.status == '0' || cab2.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBin1);
    fclose(fileBin2);
    return;
  }

  int encontrou_algum = 0;
  Registro reg1, reg2;

  // Posiciona o ponteiro de leitura no primeiro registro do arquivo 1
  fseek(fileBin1, TAM_CABECALHO, SEEK_SET);

  // Laço externo: percorre sequencialmente o arquivo 1
  while (fread(&reg1.removido, sizeof(char), 1, fileBin1) == 1) {
    // Pula registros logicamente removidos
    if (reg1.removido == '1') {
      fseek(fileBin1, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }
    lerRegistro(fileBin1, &reg1);

    // Se o campo de junção for nulo, ignora este registro
    if (reg1.codProxEstacao == -1) {
      continue;
    }

    // Posiciona o ponteiro de leitura no primeiro registro do arquivo 2
    // Esta operação é repetida a cada iteração do arquivo 1 (força bruta)
    fseek(fileBin2, TAM_CABECALHO, SEEK_SET);

    // Laço interno: percorre sequencialmente o arquivo 2 em busca da correspondência
    while (fread(&reg2.removido, sizeof(char), 1, fileBin2) == 1) {
      // Pula registros logicamente removidos
      if (reg2.removido == '1') {
        fseek(fileBin2, TAM_REGISTRO - 1, SEEK_CUR);
        continue;
      }

      lerRegistro(fileBin2, &reg2);

      // Verifica a condição de junção
      if (reg1.codProxEstacao == reg2.codEstacao) {

        // Impressão formatada dos campos requisitados
        if (reg1.codEstacao != -1) printf("%d ", reg1.codEstacao); else printf("NULO ");
        if (reg1.tamNomeEstacao > 0) printf("%s ", reg1.nomeEstacao); else printf("NULO ");
        if (reg1.tamNomeLinha > 0) printf("%s ", reg1.nomeLinha); else printf("NULO ");
        if (reg1.codProxEstacao != -1) printf("%d ", reg1.codProxEstacao); else printf("NULO ");

        // Exibe o nome da estação destino obtido do arquivo 2
        if (reg2.tamNomeEstacao > 0) printf("%s\n", reg2.nomeEstacao); else printf("NULO\n");

        encontrou_algum = 1;

        // Como a chave do arquivo 2 é primária, encerramos a busca interna após encontrar o registro
        break;
      }
    }
  }

  // Mensagem caso nenhum registro satisfaça a condição de junção
  if (encontrou_algum == 0) {
    printf("Registro inexistente.\n");
  }

  // Liberação de recursos
  fclose(fileBin1);
  fclose(fileBin2);

  return;
}

// Realiza a junção entre dois arquivos usando a técnica de junção de loop único com Árvore-B
void singleLoopJoin(char *binName1, char *nomeCampo1, char *binName2, char *nomeCampo2, char *btreeName) {

  // Validação das condições de junção conforme especificado no trabalho
  if (strcmp(nomeCampo1, "codProxEstacao") != 0 || strcmp(nomeCampo2, "codEstacao") != 0) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }
  
  // Processo de abertura de arquivos em modo de leitura/escrita
  FILE *fileBin1 = fopen(binName1, "rb+");
  FILE *fileBin2 = fopen(binName2, "rb+");
  FILE *btreeindex = fopen(btreeName, "rb+");

  // Verifica se a abertura de todos os arquivos foi bem-sucedida
  if (!fileBin1 || !fileBin2 || !btreeindex) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBin1) fclose(fileBin1);
    if (fileBin2) fclose(fileBin2);
    if (btreeindex) fclose(btreeindex);
    return; 
  }

  // Leitura dos cabeçalhos dos arquivos de dados e da Árvore-B
  Cabecalho cab1, cab2;
  BTreeHeader cabBTree;

  lerCabecalhoArvoreB(btreeindex, &cabBTree);
  lerCabecalho(fileBin1, &cab1);
  lerCabecalho(fileBin2, &cab2);

  // Verificação de consistência dos arquivos de dados
  if (cab1.status == '0' || cab2.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBin1);
    fclose(fileBin2);
    return;
  }

  // Verificação de consistência do arquivo de índice
  if (cabBTree.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(btreeindex);
    return;
  }

  int encontrou_algum = 0;
  Registro reg1, reg2;

  // Posiciona o ponteiro de leitura no primeiro registro do arquivo principal
  fseek(fileBin1, TAM_CABECALHO, SEEK_SET);

  // Percorre sequencialmente apenas o arquivo 1
  while (fread(&reg1.removido, sizeof(char), 1, fileBin1) == 1) {
    // Pula registros logicamente removidos
    if (reg1.removido == '1') {
      fseek(fileBin1, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }
    lerRegistro(fileBin1, &reg1);

    // Se o campo de junção for nulo, não há correspondência possível no arquivo 2
    if (reg1.codProxEstacao == -1) {
      continue;
    }

    // Busca pela chave no índice da Árvore-B
    // Em vez de varrer o arquivo 2, buscamos diretamente o deslocamento (offset) da chave no índice
    int foundRRN = -1, foundPos = -1, foundOffset = -1;
    int encontrou = buscaArvoreB(btreeindex, cabBTree.noRaiz, reg1.codProxEstacao, &foundRRN, &foundPos, &foundOffset);

    // Se a chave existe no índice, acessamos o registro correspondente no arquivo 2
    if (encontrou) {
      // Pula diretamente para o byte (offset) exato no arquivo 2
      fseek(fileBin2, foundOffset, SEEK_SET);
      fread(&reg2.removido, sizeof(char), 1, fileBin2);

      // Verificação de segurança: ignora se o registro no destino estiver removido
      if (reg2.removido == '1') {
        continue;
      }

      lerRegistro(fileBin2, &reg2);

      // Impressão formatada dos dados combinados
      if (reg1.codEstacao != -1) printf("%d ", reg1.codEstacao); else printf("NULO ");
      if (reg1.tamNomeEstacao > 0) printf("%s ", reg1.nomeEstacao); else printf("NULO ");
      if (reg1.tamNomeLinha > 0) printf("%s ", reg1.nomeLinha); else printf("NULO ");
      if (reg1.codProxEstacao != -1) printf("%d ", reg1.codProxEstacao); else printf("NULO ");
      
      // Imprime o nome da estação destino obtido pelo arquivo 2
      if (reg2.tamNomeEstacao > 0) printf("%s\n", reg2.nomeEstacao); else printf("NULO\n");
      
      encontrou_algum = 1; // Registra que ao menos uma correspondência foi exibida
    }
  }

  // Caso nenhum registro satisfaça a condição de junção
  if (encontrou_algum == 0) {
    printf("Registro inexistente.\n");
  }

  // Liberação de recursos
  fclose(fileBin1);
  fclose(fileBin2);
  fclose(btreeindex);
}

// Função auxiliar utilizada pelo qsort para ordenar registros pelo campo codProxEstacao
// Segue a regra de negócio onde valores nulos (-1) são posicionados ao final da ordenação
int compara_codProxEstacao(const void *r1, const void *r2) {
  Registro *regA = (Registro *)r1;
  Registro *regB = (Registro *)r2;

  int valorA = regA->codProxEstacao;
  int valorB = regB->codProxEstacao;

  // Tratamento de valores nulos: o valor nulo é considerado maior para ser enviado ao fim
  if (valorA == -1 && valorB != -1) return 1;
  if (valorB == -1 && valorA != -1) return -1;

  // Comparação numérica padrão para valores válidos
  if (valorA > valorB) return 1; 
  if (valorA < valorB) return -1;

  return 0;
}

// Função auxiliar utilizada pelo qsort para ordenar registros pelo campo codEstacao
// Mantém a mesma lógica de posicionar valores nulos (-1) ao final da ordenação
int compara_codEstacao(const void *r1, const void *r2) {
  Registro *regA = (Registro *)r1;
  Registro *regB = (Registro *)r2;

  int valorA = regA->codEstacao;
  int valorB = regB->codEstacao;

  // Tratamento de valores nulos: o valor nulo é considerado maior para ser enviado ao fim
  if (valorA == -1 && valorB != -1) return 1;
  if (valorB == -1 && valorA != -1) return -1;

  // Comparação numérica padrão para valores válidos
  if (valorA > valorB) return 1; 
  if (valorA < valorB) return -1;

  return 0;
}


// Realiza a ordenação de um arquivo binário em memória principal e grava o resultado em um novo arquivo
void orderBy(char *binEntrada, char *campoOrd, char *binSaida) {
  // Processo de abertura de arquivos: leitura do original e criação do novo arquivo ordenado
  FILE *fileBinEntrada = fopen(binEntrada, "rb+");
  FILE *fileBinSaida = fopen(binSaida, "wb+");

  // Verifica se a abertura dos arquivos foi bem-sucedida
  if (!fileBinEntrada || !fileBinSaida) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBinEntrada) fclose(fileBinEntrada);
    if (fileBinSaida) fclose(fileBinSaida);
    return;
  }

  // Leitura do cabeçalho do arquivo original para verificação de consistência
  Cabecalho cabEntrada;
  lerCabecalho(fileBinEntrada, &cabEntrada);

  if (cabEntrada.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBinEntrada);
    fclose(fileBinSaida);
    return;
  }

  // Determina o número máximo de registros para alocação do vetor na memória
  int regVal = cabEntrada.proxRRN;
  Registro *vetor = malloc(sizeof(Registro) * regVal);
  int qtdValidos = 0;

  // Percorre o arquivo de entrada para carregar apenas os registros ativos (não removidos)
  fseek(fileBinEntrada, TAM_CABECALHO, SEEK_SET);
  Registro reg1;

  while (fread(&reg1.removido, sizeof(char), 1, fileBinEntrada) == 1) {
    // Ignora registros marcados como removidos
    if (reg1.removido == '1') {
      fseek(fileBinEntrada, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }

    lerRegistro(fileBinEntrada, &reg1);

    // Armazena o registro válido no vetor para posterior ordenação
    vetor[qtdValidos] = reg1;
    qtdValidos++;
  }
 
  // Aplica o algoritmo de ordenação de acordo com o campo escolhido pelo usuário
  if (strcmp(campoOrd, "codEstacao") == 0) {
    qsort(vetor, qtdValidos, sizeof(Registro), compara_codEstacao);
  }
  else if (strcmp(campoOrd, "codProxEstacao") == 0) {
    qsort(vetor, qtdValidos, sizeof(Registro), compara_codProxEstacao);
  }

  // Inicializa o cabeçalho do novo arquivo ordenado
  Cabecalho cabSaida;
  cabSaida.nroEstacoes = 0;
  cabSaida.nroParesEstacao = 0;
  cabSaida.topo = -1;
  cabSaida.status = '0';
  cabSaida.proxRRN = 0;

  // Registra o cabeçalho inicial no arquivo de saída
  escreverCabecalho(fileBinSaida, cabSaida);

  // Escreve os registros ordenados no novo arquivo binário
  for (int i = 0; i < qtdValidos; i++) {
    escreverRegistro(fileBinSaida, vetor[i]);
  }

  // Atualiza metadados do cabeçalho com a contagem real de registros e informações de estações
  cabSaida.proxRRN = qtdValidos;
  contarEstacoesEPares(fileBinSaida, &cabSaida);

  // Finaliza a escrita marcando o arquivo como consistente
  cabSaida.status = '1';
  escreverCabecalho(fileBinSaida, cabSaida);

  // Liberação de memória e encerramento do processamento de arquivos
  free(vetor);
  fclose(fileBinEntrada);
  fclose(fileBinSaida);
}

void sortMergeJoin(char *binName1, char *campoOrd1, char *binName2, char *campoOrd2) {
  // Abre os arquivos e verifica se ocorreu bem
  FILE *fileBin1 = fopen(binName1, "rb+");
  FILE *fileBin2 = fopen(binName2, "rb+");

  if (!fileBin1 || !fileBin2) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBin1) fclose(fileBin1);
    if (fileBin2) fclose(fileBin2);
    return;
  }

  // Lê os cabecalhos e verifica se os arquivos estao consistentes
  Cabecalho cabBin1;
  lerCabecalho(fileBin1, &cabBin1);

  Cabecalho cabBin2;
  lerCabecalho(fileBin2, &cabBin2);

  if (cabBin1.status == '0' || cabBin2.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBin1);
    fclose(fileBin2);
    return;
  }

  // Ordena o arquivo1 usando codEstacao
  orderBy(binName1, "codEstacao", "tmp1.bin");

  // Ordena o arquivo2 usando o codProxEstacao
  orderBy(binName2, "codProxEstacao", "tmp2.bin");

  // Realiza o merge
  nestedLoopJoin("tmp1.bin", campoOrd1, "tmp2.bin", campoOrd2);

  // Fecha os arquivos
  fclose(fileBin1);
  fclose(fileBin2);
}
