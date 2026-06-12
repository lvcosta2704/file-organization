#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BTree.h"
#include "RegistrosIO.h"
#include "fornecidas.h"
#include "query.h"

// === FUNCIONALIDADES ===

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

  // Lê o cabecalho do arquivo binario e verifica se está consistente
  Cabecalho cabBin;

  lerCabecalho(fileBin, &cabBin);
  if (cabBin.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    return;
  }

  // Lê o cabecalho do arquivo de indice Arvore-B e verifica se está consistente
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
        printf("Registro inexistente\n");
        continue;
      }

      //printf("Breakpoint. \n");

      //printf("%d\n", filtro.codEstacao);
      //printf("%d\n", cabBTree.noRaiz);
      
      // Realiza a busca no registro de RRN especificado
      Registro reg;
      
      // Posiciona a referencia para o arquivo de dados no registro especificado
      fseek(fileBin, OFFSET, SEEK_SET);

      // Verifica se o registro está marcado como logicamente removido
      fread(&reg.removido, sizeof(char), 1, fileBin);
      if(reg.removido == '1') {
        printf("Registro inexistente.\n"); // Se estiver, nao existe o registro
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
            if (reg.codEstIntegra != -1) printf("%d\n", reg.codEstIntegra); 
            else printf("NULO\n");
        } else {
          printf("Registro inexistente.\n");
        }
      }
    }
    else { // Se nao, realiza a busca usando somente arquivo de dados (Igual à funcionalidade 3)
      fseek(fileBin, TAM_CABECALHO, SEEK_SET); // Coloca a referência para o arquivo no primeiro registro de dados

      int encontrouAlgum = 0;
      Registro reg;

      // Passa por cada registro de dados sequencialmente
      // verificando se ele está removido a principio
      while (fread(&reg.removido, sizeof(char), 1, fileBin) == 1){
          // Se estiver removido pula para o proximo
          if (reg.removido == '1') {
              fseek(fileBin, TAM_REGISTRO - 1, SEEK_CUR);
              continue;
          }
          // --- LEITURA DOS REGISTROS ---
          // Lê o restante do registro para comparar
          // Lê o registro com os freads e tambem pula o lixo do registro no final
          
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


void remocao(char *binName, char *btreeName, int N) {
  FILE *fileBin = fopen(binName, "rb+");
  if (!fileBin) {
    printf("Falha no processamento de dados\n");
    return;
  }

  FILE *btreeindex = fopen(btreeName, "rb+");
  if (!btreeindex) {
    printf("Falha no processameto do arquivo.\n");
    return;
  }

  Cabecalho cabBin;
  lerCabecalho(fileBin, &cabBin);

  BTreeHeader cabBTree;
  lerCabecalhoArvoreB(btreeindex, &cabBTree);

  if (cabBin.status == '0' || cabBTree.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    fclose(btreeindex);
    return;
  }

  cabBin.status = '0';
  cabBTree.status = '0';
  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);

  for (int i = 0; i < N; i++)
  {
    Busca filtro = inputFiltro();

    if (filtro.codEstacao == -2){
      continue;
    }

    int foundRRN = -1;
    int foundPos = -1;
    int foundOffset = -1;

    int encontrou = buscaArvoreB(
      btreeindex,
      cabBTree.noRaiz,
      filtro.codEstacao,
      &foundRRN,
      &foundPos,
      &foundOffset
    );

    if (!encontrou) {
      printf("Registro Inexistente.\n");
      continue;
    }

    Registro reg;
    fseek(fileBin, foundOffset, SEEK_SET);
    fread(&reg.removido, sizeof(char), 1, fileBin);

    if (reg.removido == '1') {
      printf("Registro inexistente.\n");
      continue;
    }

    lerRegistro(fileBin, &reg);

    int rrnDados = (foundOffset - TAM_CABECALHO) / TAM_REGISTRO;
    apagarRegistro(fileBin, &reg, &cabBin, rrnDados);

    // Remove também a chave da árvore-B
    removerArvoreB(
      btreeindex,
      &cabBTree,
      foundRRN,
      foundPos,
      filtro.codEstacao
    );
  }

  cabBin.status = '1';
  cabBTree.status = '1';

  escreverCabecalho(fileBin, cabBin);
  escreverCabecalhoArvoreB(btreeindex, &cabBTree);
  
  fclose(fileBin);
  fclose(btreeindex);

  BinarioNaTela(binName);
  BinarioNaTela(btreeName);
}