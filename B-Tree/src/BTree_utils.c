#include "BTree_utils.h"

// Realiza busca binaria
int binarySearch(int chave, int array[], int inicio, int fim, int *prox) {
    if (inicio > fim) {
        *prox = inicio;   // posição de inserção
        return -1;
    }

    int pos = (inicio + fim) / 2;

    if (array[pos] == chave) {
        return pos;
    }

    if (array[pos] < chave) {
        return binarySearch(chave, array, pos + 1, fim, prox);
    } else {
        return binarySearch(chave, array, inicio, pos - 1, prox);
    }
}
