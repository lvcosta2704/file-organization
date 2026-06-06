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
void busca(char *binName, char *btreeName, int n) {
  // Abre o arquivo de registros no modo leitura e verifica se ocorreu bem
  FILE *fileBin = fopen(binName, "rb");
  if (!fileBin) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  // Abre o arquivo de indice Arvore-B no modo leitura e verifica se ocorreu bem
  FILE *fileBTree = fopen(btreeName, "rb");
  if (!fileBTree) {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  Cabecalho cabBin;

  lerCabecalho(fileBin, &cabBin);
  if (cabBin.status == '0') {
    printf("Arquivo inconsistente.\n");
    fclose(fileBin);
    return;
  }
}
