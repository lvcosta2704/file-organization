#ifndef UTILS_H
#define UTILS_H

void sort(int *array);
int binarySearch(int chave, int array[], int inicio, int fim, int *prox);

static void swap(int *a, int *b);
static int partition(int *array, int low, int high);
static void quickSort(int array[], int low, int high);

#endif
