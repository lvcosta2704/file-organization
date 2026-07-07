// NOME:                    NUSP:
// Ryan Sulino Arrua        16900070
// Lucas Vinicius da Costa  16885265

#include <stdio.h>
#include "query.h"
#include "fornecidas.h"

// -------- FLUXO PRINCIPAL DO PROGRAMA ---------
int main() {
  int funcionalidade;
  char inputfile1[50];
  char inputfile2[50];
  char outputfile[50];
  char btreeindex[50];
  char nomeCampo1[50], nomeCampo2[50];

  if (scanf("%d", &funcionalidade) != 1)
    return 0;

  switch (funcionalidade) {
    case 11: {
      scanf("%s %s %s %s", inputfile1, nomeCampo1, inputfile2, nomeCampo2);
      nestedLoopJoin(inputfile1, nomeCampo1, inputfile2, nomeCampo2);
      break;
    }
    case 12: {
      scanf("%s %s %s %s %s", inputfile1, nomeCampo1, inputfile2, nomeCampo2, btreeindex);
      singleLoopJoin(inputfile1, nomeCampo1, inputfile2, nomeCampo2, btreeindex);
      break;
    }
    case 13: {
      scanf("%s %s %s", inputfile1, nomeCampo1, outputfile);
      orderBy(inputfile1, nomeCampo1, outputfile);
      BinarioNaTela(outputfile);
      break;
    }
    case 14: {
      scanf("%s %s %s %s", inputfile1, nomeCampo1, inputfile2, nomeCampo2);
      sortMergeJoin(inputfile1, nomeCampo1, inputfile2, nomeCampo2);
      break;
    }
    default:
      break;
  }

  return 0;
}
