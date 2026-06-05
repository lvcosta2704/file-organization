// Inclusão de bibliotecas
#include <stdio.h>
#include <stdlib.h>

// Variáveis globais
int comp_quick;
int atri_quick;
int comp_shell;
int atri_shell;

// Prototipos de funções
void shell(int v[], int n);
void quick(int v[], int f, int l);
void cria_subvetor(const int vetor[], int subvetor[], int n);

// Função main
int main()
{
	// Le a quantidade de elementos do vetor
	int num_elementos;
	scanf("%d", &num_elementos);

	// Le os elementos do vetor
	int vetor[num_elementos];
	for(int i = 0; i < num_elementos; i++)
	{
		scanf("%d", &vetor[i]);
	}

	// Realiza a ordenação para cada subvetor
	for(int i = 0; i < num_elementos; i++)
	{
		// Diz qual é o subvetor (debug)
		// printf("CASO %d\n", i + 1);

		// Reseta a contagem de operações
		comp_quick = 0;
		atri_quick = 0;
		comp_shell = 0;
		atri_shell = 0;

		int tam = i + 1; // Define o tamanho dos subvetores

		// Cria os subvetores
		int *subvetor = (int *) malloc(sizeof(int) * tam);
		if(subvetor == NULL)
		{
			printf("Erro.\n");
			return 1;
		}
		cria_subvetor(vetor, subvetor, tam);

		int *subvetor2 = (int *) malloc(sizeof(int) * tam);
		if(subvetor2 == NULL)
		{
			printf("Erro.\n");
			return 1;
		}
		cria_subvetor(vetor, subvetor2, tam);
		
		// Imprime os subvetores (debug)
		/* printf("Subvetor 1: ");
		for(int j = 0; j < tam; j++)
		{
			printf("%d ", subvetor[j]);
		}

		printf("\n");

		printf("Subvetor 2: ");
		for(int j = 0; j < tam; j++)
		{
			printf("%d ", subvetor2[j]);
		}

		printf("\n"); */

		shell(subvetor, tam); // Ordena com o Shell Sort o subvetor
		quick(subvetor2, 0, tam - 1); // Ordena com o Quick Sort o subvetor

		// Imprime os subvetores (debug)
		/* printf("Subvetor 1: ");
		for(int j = 0; j < tam; j++)
		{
			printf("%d ", subvetor[j]);
		}

		printf("\n");

		printf("Subvetor 2: ");
		for(int j = 0; j < tam; j++)
		{
			printf("%d ", subvetor2[j]);
		}

		printf("\n"); */


		// Conta a quantidade de operações
		int op_shell = atri_shell + comp_shell;
		int op_quick = atri_quick + comp_quick;

		// Diz a quantidade de operações (debug)
		/* printf("Shell: %d\n", op_shell);
		printf("Quick: %d\n", op_quick); */

		// Compara a quantidade de operações e faz as impressões necessárias
		if(op_shell == op_quick)
		{
			printf("- "); // Se a quantidade é a mesma, imprime '-'
		}
		else if(op_shell < op_quick)
		{
			printf("S "); // Se o Shell Sort fez menos operações, imprime 'S'
		}
		else
		{
			printf("Q "); // Se o Quick Sort fez menos operações, imprime 'Q'
		}
	}

	printf("\n");

	return 0;
}

// Outras funções
void shell(int v[], int n) // Shell Sort
{
	int gap = 1;
	while(gap <= n) 
	{
		gap *= 2;
	}

	gap = gap / 2 - 1;

	while(gap > 0)
	{
		for (int i = gap; i < n; i++)
		{
			atri_shell++;
			int x = v[i];

			int j = i - gap;

			while(j >= 0) 
			{
				comp_shell++;
				if(v[j] > x)
				{
					atri_shell++;
					v[j + gap] = v[j];
					j -= gap;
				}
				else
				{
					break;
				}
			}
			atri_shell++;
			v[j + gap] = x;
            }
        gap /= 2;
    }

}

void quickSort(int v[], int f, int l)  // Quick Sort
{
	if (f >= l) 
	{
		return;
    }
	int m = (l + f)/2;

	atri_quick++;
	int pivot = v[m]; 

	int i = f;
	int j = l;

        while(1) 
	{
		comp_quick++;
                while(v[i] < pivot) 
		{
			comp_quick++;
                        i++;
                }

		comp_quick++;
                while(v[j] > pivot) 
		{
			comp_quick++;
                        j--;
                }
                if (i >= j) 
		{
                        break;
                }
		atri_quick++;
                int aux = v[i];

		atri_quick++;
                v[i] = v[j];

		atri_quick++;
                v[j] = aux;

                i++;
                j--;

        }

        quick(v, f, j);
        quick(v, j+1, l);
}

void cria_subvetor(const int vetor[], int subvetor[], int n) // 'n' é o tamanho do SUBVETOR 
{
	for(int i = 0; i < n; i++)
	{
		subvetor[i] = vetor[i];
	}
}
