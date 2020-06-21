#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <omp.h>
#include <mpi.h>

#define ORIGEM 0

typedef struct {
	int custo;
	int n_vert;
	int *caminho;
} min_path;

typedef struct {
	int n_vert;
	int *vert;
} c_vert;

// Declara a matriz global
int *matriz;

c_vert * init_c_vert(int);

min_path * find_min_path(int, c_vert *);

min_path * init_min_path(int);


int main(int argc, char ** argv) {
	// Declara as variáveis de índice
	int i, j, dim, min, n_vert;

	// Lê a dimensão das matrizes
	fscanf(stdin, "%d\n", &dim);
 
	// Aloca a matriz
	matriz = (int *) malloc(dim *dim * sizeof(int));

	// Lê a matriz
	for(i = 0; i < dim; i++) {
		for(j = 0; j < dim; j++) {
			fscanf(stdin, "%d ", &(matriz[i*dim+j]));
		}
	}

	n_vert = dim - 1;

	c_vert * vertices = init_c_vert(n_vert);

	for (int l = 1; i < n_vert; l++) {
		vertices->vert[l - 1] = l;
	}

	min = find_min_path(ORIGEM, vertices);
}

c_vert * init_c_vert(int num_vertices) {
	if (num_vertices < 1)
		return NULL;

	c_vert * temp = (c_vert *) malloc(sizeof(c_vert));
	temp->vert = (int *) calloc(num_vertices, sizeof(int));
	temp->n_vert = num_vertices;

	return temp;
}

min_path * init_min_path(int num_vertices) {
	if (num_vertices < 2)
		return NULL;

	min_path * temp = (min_path *) malloc(sizeof(min_path));
	temp->caminho = (int *) calloc(num_vertices, sizeof(int));
	temp->n_vert = num_vertices;
	temp->custo = INT_MAX;

	return temp;
}

min_path * find_min_path(int orig, c_vert *vertices) {
	// fim de recursão
	if (vertices->n_vert == 1) {
		min_path *path = init_min_path(2);
		path->custo = matriz[orig][vertices->vert[0]];
		path->caminho[1] = vertices->vert[0];
		path->caminho[0] = orig;
		return path;
	}
	// caso contrário
	int ** caminhos = (min_path **) calloc(n_vert, sizeof(min_path *));

	for(int i = 0; i < n_vert; i++) {
		temp[i] = find_min_path(vert[i], vert_i, n_vert - 1, caminho, matriz);
	}
	int index_min = find_index_min(temp, n_vert);
}




















