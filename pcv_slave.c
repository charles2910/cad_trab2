// para compilar: mpicc slave.c -o slave -Wall
// para rodar: sera inciado pelo mestre com MPI_Comm_spawn

/** Grupo 7 - by Carlos Melara
 * Filho que deve calcular o caminho mínimo a partir de uma origem em um
 * conjunto de vértices, ambos fornecidos pelo pai. Em seguida, deve devolver
 * o custo e o caminho mínimo para o pai e finalizar.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

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

int main(int argc, char **argv)  {
	// Inicializções MPI
	int         tag = 0, my_rank, num_proc;
	char        message_0[50], message_1[] = "hello master :-)" ;
	char        master_data[50], slaves_data[50], arquivo[20];

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;

	// Inicializações PCV
	int	i,
		j,
		n_vert,
		origem,
		*matriz;

	FILE	*fp;

	c_vert	*vertices;
	min_path *min;

	MPI_Status      status;
	MPI_Comm        inter_comm; 

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	MPI_Get_processor_name(processor_name, &name_len);

	MPI_Comm_get_parent(&inter_comm);

    /* master gets a message from the
    parent (Master) and broadcasts it to the 
    other processes (slaves) */
    
	MPI_Recv(arquivo, 20, MPI_CHAR, 0, tag, inter_comm, &status);
	printf("SLAVE on processor %s (master) : message received : %s\n", processor_name, arquivo);
	 
	MPI_Send(message_1, 50, MPI_CHAR,0 , tag, inter_comm);
	MPI_Recv(master_data, 50, MPI_CHAR, 0, tag, inter_comm, &status);

	printf("SLAVE (master) : master data received : %s\n", master_data);
	strcpy(slaves_data, master_data);

	// Filho recebe número de vértices para procurar caminho mínimo
	MPI_Recv(&n_vert, 1, MPI_INT, 0, tag, inter_comm, &status);
	// Filho recebe origem a partir da qual deve procurar
	MPI_Recv(&origem, 1, MPI_INT, 0, tag, inter_comm, &status);
	// Inicializa estrutura do conjunto de vértices
	vertices = init_c_vert(n_vert);
	// E recebe o conjunto de vértices
	MPI_Recv(vertices->vert, n_vert, MPI_INT, 0, tag, inter_comm, &status);

	// Inicializa matriz de custos
	fp = fopen(arquivo, "r");
	fscanf(fp, "%d\n", &dim);
	matriz = (int *) malloc(dim *dim * sizeof(int));
	for(i = 0; i < dim; i++) {
		for(j = 0; j < dim; j++) {
			fscanf(fp, "%d ", &(matriz[i*dim+j]));
		}
	}

	// Calcula caminho mínimo
	min = find_min_path(origem, vertices, matriz);

	// Filho envia tamanho do caminho para o pai
	MPI_Send(&min->n_vert, 1, MPI_INT, 0, tag, inter_comm);
	// Agora temos que devolver o custo do caminho mínimo para o pai
	MPI_Send(&min->custo, 1, MPI_INT, 0, tag, inter_comm);
	// E enviar o caminho
	MPI_Send(min->caminho, min->n_vert, MPI_INT, 0, tag, inter_comm);

    
    //MPI_Bcast(slaves_data, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    //printf("SLAVE - %d : slaves data  received from Bcast : %s\n", my_rank, slaves_data);

	MPI_Finalize();
    
	exit(0);
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
	if ((caminhos == NULL) || (n_vert < 2))
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
				novo->vert[j - 1] = vertices->vert[j];
			else
				novo->vert[j] = vertices->vert[j];
		}
		caminhos[i] = find_min_path(vertices->vert[i], novo, matriz);
		free(novo);
		(caminhos[i])->custo += matriz[orig * dim + vertices->vert[i]];
	}

	int index_min = find_index_min(caminhos, vertices->n_vert);
	min_path *path_min = init_min_path(vertices->n_vert + 1);
	path_min->custo = caminhos[index_min]->custo;
	append_min_path(orig, path_min->caminho, caminhos[index_min]);

	for(int i = 0; i < vertices->n_vert; i++) {
		free(caminhos[i]);
	}
	free(caminhos);

	return path_min;
}
