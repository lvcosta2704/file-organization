#include <stdio.h>
#include "query.h"

// -------- FLUXO PRINCIPAL DO PROGRAMA ---------
int main() {
  int funcionalidade;
  char inputfile[50];
  char outputfile[50];
  char btreeindex[50];

  if (scanf("%d", &funcionalidade) != 1)
    return 0;

  switch (funcionalidade) {
    case 7:
      // Cria o arquivo de índice Arvore-B
      scanf("%s %s", inputfile, btreeindex);
  //    criarArvoreB(inputfile, btreeindex);
      break;
    case 8: {
      // Realiza n buscas usando o arquivo de índice arvore B
      int n;
      scanf("%s %s %d", inputfile, btreeindex, &n);
      busca(inputfile, btreeindex, n);
      break;
    }
    case 9: {
      // Realiza n insercoes usando o arquivo de indice arvore B
      int n;
      scanf("%s %s %d", inputfile, btreeindex, &n);
  //    insercao(inputfile, btreeindex, n);
      break;
    }
    case 10: {
      // Realiza n remocoes usando o arquivo de indice arvore B
      int n;
      scanf("%s %s %d", inputfile, btreeindex, &n);
      remocao(inputfile, btreeindex, n);
      break;
    }
    default:
      break;
  }

  return 0;
}
