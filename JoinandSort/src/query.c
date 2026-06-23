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

void nestedLoopJoin(char *binName1, char *nomeCampo1, char *binName2, char *nomeCampo2) {
 // printf("DEBUG: ANTES DO FOPEN\n");
  FILE *fileBin1 = fopen(binName1, "rb+");
  FILE *fileBin2 = fopen(binName2, "rb+");
  //printf("DEBUG: DEPOIS DO FOPEN\n");

  if (!fileBin1 || !fileBin2) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBin1) fclose(fileBin1);
    if (fileBin2) fclose(fileBin2);
    return;
  }
  //printf("DEBUG: DEPOIS DO PRIMEIRO CHECK IF\n");

  Cabecalho cab1, cab2;

  lerCabecalho(fileBin1, &cab1);
  lerCabecalho(fileBin2, &cab2);
  //printf("DEBUG: ANTES DO CHECK CABEÇALHO\n");
  if (cab1.status == '0' || cab2.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBin1);
    fclose(fileBin2);
    return;
  }

  int encontrou_algum = 0;
  Registro reg1, reg2;

  fseek(fileBin1, TAM_CABECALHO, SEEK_SET);

  while (fread(&reg1.removido, sizeof(char), 1, fileBin1) == 1) {
    if (reg1.removido == '1') {
      fseek(fileBin1, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }
    lerRegistro(fileBin1, &reg1);

    if (reg1.codProxEstacao == -1) {
      continue;
    }

    fseek(fileBin2, TAM_CABECALHO, SEEK_SET);

    while (fread(&reg2.removido, sizeof(char), 1, fileBin2) == 1) {
      if (reg2.removido == '1') {
        fseek(fileBin2, TAM_REGISTRO - 1, SEEK_CUR);
        continue;
      }

      lerRegistro(fileBin2, &reg2);

      if (reg1.codProxEstacao == reg2.codEstacao) {

        // 1. codEstacao (arquivo 1)
        if (reg1.codEstacao != -1) printf("%d ", reg1.codEstacao); else printf("NULO ");
        
        // 2. nomeEstacao (arquivo 1)
        if (reg1.tamNomeEstacao > 0) printf("%s ", reg1.nomeEstacao); else printf("NULO ");
        
        // 3. nomeLinha (arquivo 1)
        if (reg1.tamNomeLinha > 0) printf("%s ", reg1.nomeLinha); else printf("NULO ");
        
        // 4. codProxEstacao (arquivo 1)
        if (reg1.codProxEstacao != -1) printf("%d ", reg1.codProxEstacao); else printf("NULO ");

        // Vindo pela Junção
        // 5. nomeProxEstacao (que é o nomeEstacao do arquivo 2) -> Último sem espaço
        if (reg2.tamNomeEstacao > 0) printf("%s\n", reg2.nomeEstacao); else printf("NULO\n");

        encontrou_algum = 1;

        break;
      }
    }

  }

  if (encontrou_algum == 0) {
    printf("Registro inexistente.\n");
  }

  fclose(fileBin1);
  fclose(fileBin2);

  return;
}

void singleLoopJoin(char *binName1, char *nomeCampo1, char *binName2, char *nomeCampo2, char *btreeName) {

  if (strcmp(nomeCampo1, "codProxEstacao") != 0 || strcmp(nomeCampo2, "codEstacao") != 0) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }
  
  FILE *fileBin1 = fopen(binName1, "rb+");
  FILE *fileBin2 = fopen(binName2, "rb+");
  FILE *btreeindex = fopen(btreeName, "rb+");

  if (!fileBin1 || !fileBin2 || !btreeindex) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBin1) fclose(fileBin1);
    if (fileBin2) fclose(fileBin2);
    if (btreeindex) fclose(btreeindex);
    return; 
  }

  Cabecalho cab1, cab2;
  BTreeHeader cabBTree;

  lerCabecalhoArvoreB(btreeindex, &cabBTree);
  lerCabecalho(fileBin1, &cab1);
  lerCabecalho(fileBin2, &cab2);

  if (cab1.status == '0' || cab2.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBin1);
    fclose(fileBin2);
    return;
  }

  if (cabBTree.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(btreeindex);
    return;
  }



  int encontrou_algum = 0;
  Registro reg1, reg2;

  fseek(fileBin1, TAM_CABECALHO, SEEK_SET);

  while (fread(&reg1.removido, sizeof(char), 1, fileBin1) == 1) {
    if (reg1.removido == '1') {
      fseek(fileBin1, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }
    lerRegistro(fileBin1, &reg1);

    if (reg1.codProxEstacao == -1) {
      continue;
    }

    fseek(fileBin2, TAM_CABECALHO, SEEK_SET);

    int foundRRN = -1, foundPos = -1, foundOffset = -1;

    int encontrou = buscaArvoreB(btreeindex, cabBTree.noRaiz, reg1.codProxEstacao, &foundRRN, &foundPos, &foundOffset);

    if (encontrou) {
      fseek(fileBin2, foundOffset, SEEK_SET);
      fread(&reg2.removido, sizeof(char), 1, fileBin2);

      if (reg2.removido == '1') {
        continue;
      }

      lerRegistro(fileBin2, &reg2);

      if (reg1.codEstacao != -1) printf("%d ", reg1.codEstacao); else printf("NULO ");
      if (reg1.tamNomeEstacao > 0) printf("%s ", reg1.nomeEstacao); else printf("NULO ");
      if (reg1.tamNomeLinha > 0) printf("%s ", reg1.nomeLinha); else printf("NULO ");
      if (reg1.codProxEstacao != -1) printf("%d ", reg1.codProxEstacao); else printf("NULO ");
      // Vindo pela Junção
      if (reg2.tamNomeEstacao > 0) printf("%s\n", reg2.nomeEstacao); else printf("NULO\n");
    }
  }
}
// Funcao auxiliar que é usada no qsort() da funcao orderBy()
int comparaReg(const void *r1, const void *r2) {
  Registro *regA = (Registro *)r1;
  Registro *regB = (Registro *)r2;

  int valorA = regA->codProxEstacao;
  int valorB = regB->codProxEstacao;

  if (valorA == -1 && valorB != -1) return 1;
  if (valorB == -1 && valorA != -1) return -1;

  if (valorA > valorB) return 1; // poe o valorA a direita do valorB 
  if (valorA < valorB) return -1; // poe o valorA a esquerda do valorB

  return 0;
}

void orderBy(char *binEntrada, char *campoOrd, char *binSaida) {
  FILE *fileBinEntrada = fopen(binEntrada, "rb+");
  FILE *fileBinSaida = fopen(binSaida, "wb+");

  if (!fileBinEntrada || !fileBinSaida) {
    printf("Falha no processamento do arquivo.\n");
    if (fileBinEntrada) fclose(fileBinEntrada);
    if (fileBinSaida) fclose(fileBinSaida);
    return;
  }

  Cabecalho cabEntrada;
  lerCabecalho(fileBinEntrada, &cabEntrada);

  if (cabEntrada.status == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(fileBinEntrada);
    fclose(fileBinSaida);
    return;
  }

  int regVal = cabEntrada.proxRRN;

  
  Registro *vetor = malloc(sizeof(Registro)*regVal);
  int qtdValidos = 0;

  fseek(fileBinEntrada, TAM_CABECALHO, SEEK_SET);
  Registro reg1;

  while (fread(&reg1.removido, sizeof(char), 1, fileBinEntrada) == 1) {
    if (reg1.removido == '1') {
      fseek(fileBinEntrada, TAM_REGISTRO - 1, SEEK_CUR);
      continue;
    }

    lerRegistro(fileBinEntrada, &reg1);

    vetor[qtdValidos] = reg1;
    qtdValidos++;
  }
 
  qsort(vetor, qtdValidos, sizeof(Registro), comparaReg);

  Cabecalho cabSaida;

  cabSaida.nroEstacoes = 0;
  cabSaida.nroParesEstacao = 0;
  cabSaida.topo = -1;
  cabSaida.status = '0';
  cabSaida.proxRRN = 0;

  escreverCabecalho(fileBinSaida, cabSaida);

  for (int i = 0; i < qtdValidos; i++)
  {
    escreverRegistro(fileBinSaida, vetor[i]);
  }

  cabSaida.proxRRN = qtdValidos;

  contarEstacoesEPares(fileBinSaida, &cabSaida);

  cabSaida.status = '1';
  escreverCabecalho(fileBinSaida, cabSaida);

  free(vetor);
  fclose(fileBinEntrada);
  fclose(fileBinSaida);

  BinarioNaTela(binSaida);
}

