#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
//#include <omp.h>
//#include <mpi.h>

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

c_vert * init_c_vert(int);

min_path * find_min_path(int, c_vert *, int *);

min_path * init_min_path(int);

int find_index_min(min_path **, int);

int append_min_path(int, int *, min_path *);

int dim;

int main(int argc, char ** argv) {
	// Declara as variáveis de índice
	int i, j, n_vert;

	// Lê a dimensão das matrizes
	fscanf(stdin, "%d\n", &dim);
 
	// Declara a matriz global
	int *matriz;

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

	min_path * min = find_min_path(ORIGEM, vertices, matriz);

	printf("menor custo: %d\n", min->custo);

	for(int i = 0; i < min->n_vert; i++) {
		if (i == min->n_vert - 1) {
			printf("%d --> %d\n", min->caminho[i], min->caminho[0]);
			break;
		}
		printf("%d --> ", min->caminho[i]);
	}
	return 0;
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
	temp->custo = INT_MAX/2 - 1;

	return temp;
}

int find_index_min(min_path ** caminhos, int n_vert) {
	if ((caminhos == NULL) || (n_vert < 3))
		return -1;

	int index_min = -2, min = INT_MAX;

	for(int i = 0; i < n_vert; i++) {
		if (caminhos[i]->custo < min) {
			index_min = i;
			min = caminhos[i]->custo;
		}
	}

	return index_min;
}

int append_min_path(int origem, int *novo_caminho, min_path *caminho) {
	if ((origem < 0) || (novo_caminho == NULL) || (caminho == NULL))
		return -1;

	for(int i = 1; i <= caminho->n_vert; i++) {
		novo_caminho[i] = caminho->caminho[i - 1];
	}

	novo_caminho[0] = origem;

	return 0;
}

min_path * find_min_path(int orig, c_vert *vertices, int * matriz) {
	// fim de recursão
	if (vertices->n_vert == 1) {
		min_path *path = init_min_path(2);
		path->custo = matriz[orig * dim + vertices->vert[0]];
		path->caminho[1] = vertices->vert[0];
		path->caminho[0] = orig;
		return path;
	}
	// caso contrário
	min_path ** caminhos = (min_path **) calloc(vertices->n_vert, sizeof(min_path *));

	for(int i = 0; i < vertices->n_vert; i++) {
		c_vert * novo = init_c_vert(vertices->n_vert - 1);
		int marcacao = 0;
		for (int j = 0; j < vertices->n_vert; j++) {
			if (j == i) {
				marcacao = 1;
				continue;
			}
			if (marcacao)
				novo->vert[j - 1] = j;
			else
				novo->vert[j] = j;
		}
		caminhos[i] = find_min_path(vertices->vert[i], novo, matriz);
		free(novo);
		(caminhos[i])->custo += matriz[orig * dim + vertices->vert[i]];
	}

	int index_min = find_index_min(caminhos, vertices->n_vert);
	min_path *path_min = init_min_path(vertices->n_vert + 1);
	path_min->custo = caminhos[index_min]->custo;
	append_min_path(orig, path_min->caminho, caminhos[index_min]);

	return path_min;
}




















