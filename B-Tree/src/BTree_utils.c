#include "BTree_utils.h"

// Troca os elementos
static void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Encontra a posicao de particionamento
static int partition(int *array, int low, int high) {
  int pivot = array[high];

  int i = (low - 1);
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {
      i++;
      swap(&array[i], &array[j]);
    }
  }

  swap(&array[i + 1], &array[high]);

  return (i + 1);
}

// Algoritmo QuickSort
static void quickSort(int array[], int low, int high) {
  if (low < high) {
    int pi = partition(array, low, high);

    quickSort(array, low, pi - 1);

    quickSort(array, pi + 1, high);
  }
}


// Ordena um array usando o algoritmo QuickSort
void sort(int *array) {
  int n = sizeof(array) / sizeof(array[0]);

  quickSort(array, 0, n - 1);
}

// Realiza busca binaria
int binarySearch(int chave, int array[], int inicio, int fim, int *prox) {
  int pos = (inicio + fim) / 2;

  if(array[pos] == chave)
    return pos;
  else if(inicio >= fim) {
    *prox = inicio; // Salva a posicao em que a chave deveria estar
    return -1; // Nao encontrou
  }
  else
    if(array[pos] < chave) {
      return binarySearch(chave, array, pos+1, fim, prox);
    }
    else {
      return binarySearch(chave, array, inicio, pos-1, prox);
    }
}


