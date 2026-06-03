#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include "Registros.h"
#include "BTree.h"
#include "utils.h"
#include "fornecidas.h"

// Definicao de macros
#define ORDEM 4
#define MAX_CHAVES ORDEM-1
#define TAM_CABECALHO 17
#define TAM_NO 13+8*MAX_CHAVES+4*ORDEM

// Struct para armazenar a chave e seu respectivo índice
struct chaves{
    int codEstacao;
    int RRN;
};

struct btreeheader{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
};

struct btreenode{
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    struct chaveIndice chaves[MAX_CHAVES] // C1, Pr1 a Cn, Prn
    int indiceNo[ORDEM]; // P1 a Pn
};


