#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>

#define ORIGEM 0

int main(int argc, char ** argv) {
	// Declara a matrize
	int *A;
	// Declara as variáveis de índice
	int i, j, dim, min;
	// Declara o acumulador para o produto escalar global
	long resultado=0;

	// Declara um vetor para os produtos escalares locais
	long *prod_escalar;
 
	fscanf(stdin, "%d\n", &dim); // Lê a dimensão das matrizes
 
	// Aloca as matrizes
	A=(int *)malloc(dim *dim * sizeof(int));

	// Aloca um vetor para armazenar os produtos escalares de cada linha
	prod_escalar=(long *)malloc(dim * sizeof(long));

	// Lê a matriz A
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			fscanf(stdin, "%d ",&(A[i*dim+j]));
		}
	}

	int *caminho = (int *) calloc(dim, sizeof(int));

	min = find_min_path(ORIGEM, vertices, caminho);
}
