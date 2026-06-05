#include "utils.h"

// Troca os elementos
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Encontra a posicao de particionamento
int partition(int *array, int low, int high) {
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
void quickSort(int array[], int low, int high) {
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

