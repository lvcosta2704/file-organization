#include "Registros.h"
#include <stdio.h>

// -------- FLUXO PRINCIPAL DO PROGRAMA ---------
int main() {
  int funcionalidade;
  char inputfile[50];
  char outputfile[50];
  char btreeindex[50];

  if (scanf("%d", &funcionalidade) != 1)
    return 0;

  switch (funcionalidade) {
  case 1:
    scanf("%s %s", inputfile, outputfile);
    criarBin(inputfile, outputfile);
    break;
  case 2:
    scanf("%s", inputfile);
    listarRegistros(inputfile);
    break;
  case 3: {
    int n;
    scanf("%s %d", inputfile, &n);
    buscarRegistros(inputfile, n);
    break;
  }
  case 4: {
    int n;
    scanf("%s %d", inputfile, &n);
    removerRegistros(inputfile, n);
    break;
  }
  case 5: {
    int n;
    scanf("%s %d", inputfile, &n);
    inserirRegistros(inputfile, n);
    break;
  }
  case 6: {
    int n;
    scanf("%s %d", inputfile, &n);
    atualizarRegistros(inputfile, n);
    break;
  }
  case 7:
    // Cria o arquivo de índice Arvore-B
    scanf("%s %s", inputfile, btreeindex);
    criarArvoreB(inputfile, btreeindex);
    break;
  case 8: {
    // Realiza n buscas usando o arquivo de índice arvore B
    int n;
    scanf("%s %s %d", inputfile, btreeindex, &n);
    buscaArvoreB(inputfile, btreeindex, n);
    break;
  }
  case 9: {
    // Realiza n remocoes usando o arquivo de indice arvore B
    int n;
    scanf("%s %s %d", inputfile, btreeindex, &n);
    remocaoArvoreB(inputfile, btreeindex, n);
    break;
  }
  case 10: {
    // Realiza n insercoes usando o arquivo de indice arvore B
    int n;
    scanf("%s %s %d", inputfile, btreeindex, &n);
    insercaoArvoreB(inputfile, btreeindex, n);
    break;
  }
  default:
    break;
  }

  return 0;
}
