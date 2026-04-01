#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Registros.h"

// -------- FLUXO PRINCIPAL DO PROGRAMA ---------
int main () {
    int funcionalidade;
    char inputfile[50];
    char outputfile[50];

    if (scanf("%d", &funcionalidade) != 1) return 0;

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
        default:
            break;
    }
    
    return 0;
}

