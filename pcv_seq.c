#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>

#define ORIGEM 0

int find_min_path(int orig, int *vert, int n_vert, int *caminho);

int main(int argc, char ** argv) {
	// Declara a matrize
	int *A;
	// Declara as variáveis de índice
	int i, j, dim, min, n_vert;
	// Declara o acumulador para o produto escalar global
	long resultado=0;

	// Declara um vetor para os produtos escalares locais
	int *vertices, *caminho;
 
	fscanf(stdin, "%d\n", &dim); // Lê a dimensão das matrizes
 
	// Aloca as matrizes
	A=(int *)malloc(dim *dim * sizeof(int));

	// Lê a matriz A
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			fscanf(stdin, "%d ",&(A[i*dim+j]));
		}
	}

	caminho = (int *) calloc(dim, sizeof(int));
	vertices = (int *) calloc(dim, sizeof(int));
	n_vert = dim;

	min = find_min_path(ORIGEM, vertices, n_vert, caminho);
}
