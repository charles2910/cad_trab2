#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>

#define ORIGEM 0

int find_min_path(int orig, int *vert, int n_vert, int *caminho, int *matriz);

int main(int argc, char ** argv) {
	// Declara a matrize
	int *matriz;
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
			fscanf(stdin, "%d ",&(matriz[i*dim+j]));
		}
	}

	caminho = (int *) calloc(dim, sizeof(int));

	n_vert = dim - 1;

	vertices = (int *) calloc(n_vert, sizeof(int));

	min = find_min_path(ORIGEM, vertices, n_vert, caminho, matriz);
}

int find_min_path(int orig, int *vert, int n_vert, int *caminho, int *matriz) {
	if (n_vert == 1) {
		return matriz[orig][vert[0]];
	}
	int * temp = (int *) calloc(n_vert, sizeof(int));

	for(int i = 0; i < n_vert; i++) {
		int 
		for(int s = 0; s < n_vert; s++) {
			vert
			
		temp[i] = find_min_path(vert[i], vert_i, n_vert - 1, caminho, matriz);
	}
	int index_min = find_index_min(temp, n_vert);
}
