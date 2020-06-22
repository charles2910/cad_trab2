//compilar: mpicc pcv_master.c -o pcv_master
//executar: mpirun pcv_master -np 1 pcv_master

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <mpi.h>

#define NUM_SPAWNS 1
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

int main(int argc, char **argv)  {
	// Inicialização do PCV
	int	i,
		j,
		n_vert,
		*matriz;

	FILE	*fp;

	if (argc > 1) fp = fopen(argv[1], "r");
	else fp = fopen("caixeiro-4", "r");

	// Lê a dimensão da matriz
	fscanf(fp, "%d\n", &dim);
	printf("Dimensão: %d\n", dim);
 
	// Aloca a matriz
	matriz = (int *) malloc(dim *dim * sizeof(int));

	// Lê a matriz
	for(i = 0; i < dim; i++) {
		for(j = 0; j < dim; j++) {
			fscanf(fp, "%d ", &(matriz[i*dim+j]));
		}
	}

	printf("Argumento: %s\n", argv[1]);

	// Preparação do MPI - Declarações
	int 	master_tag = 1,
		my_rank,
		num_proc,
		src,
		dst,
		root,
		buf_rcv,
		name_len,
		*errcodes,
		*origs;

	char	slave[40],
		processor_name[MPI_MAX_PROCESSOR_NAME];;
	c_vert * vertices;
	c_vert ** verts_mpi;
	min_path ** path_min_mpi;

	// Alocações dinâmicas
	verts_mpi = (c_vert **) calloc(dim - 1, sizeof(c_vert *));
	path_min_mpi = (min_path **) calloc(dim - 1, sizeof(min_path *));
	errcodes = (int *) calloc(dim - 1, sizeof(int));
	origs = (int *) calloc(dim - 1, sizeof(int));
	vertices = init_c_vert(dim - 1);
	vertices->vert = (int *) calloc(dim - 1, sizeof(int));

	if (dim > 12) {
		printf("Tem certeza que quer fazer isso?\n");
		printf("Eu tenho certeza que você não quer fazer isso.\n");
		return 0;
	}

	printf("Conjunto de vértices\n");
	for (int t = 0; t < dim - 1; t++) {
		origs[t] = t + 1;
		verts_mpi[t] = init_c_vert(dim - 2);
		path_min_mpi[t] = init_min_path(dim - 1);
		vertices->vert[t] = t + 1;
		printf("%d\t", vertices->vert[t]);
	}

	for(int t = 0; t < dim - 1; t++) {
		int marcacao = 0;
		printf("\nVértice de origem: %d\n", vertices->vert[t]);
		for (int j = 0; j < dim - 1; j++) {
			if (j == t) {
				marcacao = 1;
				continue;
			}
			if (marcacao) {
				verts_mpi[t]->vert[j - 1] = vertices->vert[j];
				printf("%d = %d\t", verts_mpi[t]->vert[j - 1], vertices->vert[j]);
			} else {
				verts_mpi[t]->vert[j] = vertices->vert[j];
				printf("%d = %d\t", verts_mpi[t]->vert[j], vertices->vert[j]);
			}
			printf("%d\t", verts_mpi[t]->vert[j]);
		}
	}

	MPI_Status  	status;
	MPI_Comm    	*inter_comm;
	MPI_Info 		info_param = 0;

	strcpy(slave,"pcv_slave");
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	
	MPI_Get_processor_name(processor_name, &name_len);

	MPI_Info_create(&info_param);
	MPI_Info_set(info_param, "hostfile", "./halley.txt");

	inter_comm = (MPI_Comm *) calloc(dim - 1, sizeof(MPI_Comm));


	src = dst = root = 0;
	
	int array_of_errcodes[10], tag = 0;
	char arquivo[20];
	char master_data[] = "slaves to work";
	char message_1[15][50];
	printf("MASTER on processor %s : spawning %d slaves ... \n", processor_name, NUM_SPAWNS);

	// Copia nome do arquivo da matriz
	if (argc > 1) strncpy(arquivo, argv[1], 16);
	else strncpy(arquivo, "caixeiro-4", 16);

	// Cria filhos
	strcpy(slave,"./pcv_slave");
	for(int m = 0; m < dim - 1; m++) {
		MPI_Comm_spawn(slave, MPI_ARGV_NULL, 1, info_param, root, MPI_COMM_WORLD, &(inter_comm[m]), errcodes);
	}

	for(int p = 0; p < dim - 1; p++) {
		printf("MASTER : send a message to master of slaves %d (%s) ...\n", p, arquivo);
		MPI_Send(arquivo, 20, MPI_CHAR, dst , tag, inter_comm[p]);

		MPI_Recv(message_1[p], 50, MPI_CHAR, dst, tag, inter_comm[p], &status);
		printf("MASTER : message received : %s\n", message_1[p]);

		MPI_Send(master_data, 50, MPI_CHAR, dst , tag, inter_comm[p]);

		// Pai envia número de vértices que devem ser explorados
		MPI_Send(&verts_mpi[p]->n_vert, 1, MPI_INT, dst , tag, inter_comm[p]);
		printf("Enviando pro filho %d os %d vértices\n", p, verts_mpi[p]->n_vert);
		// Agora envia a origem a partir da qual procurar
		MPI_Send(&origs[p], 1, MPI_INT, dst , tag, inter_comm[p]);
		// Então envia o conjunto de vértices
		MPI_Send(verts_mpi[p]->vert, verts_mpi[p]->n_vert, MPI_CHAR, dst , tag, inter_comm[p]);
	}

	printf("\n\nPassou dos envios\n\n");

	// Está na hora de receber os caminhos mínimos calculados pelos filhos
	for(int p = 0; p < dim - 1; p++) {
		// Pai recebe número de vértices do caminho mínimo
		MPI_Recv(&path_min_mpi[p]->n_vert, 1, MPI_INT, dst, tag, inter_comm[p], &status);
		printf("Recebendo %d vertices de filho %d\n", path_min_mpi[p]->n_vert, p);
		// Agora recebe o custo do caminho
		MPI_Recv(&path_min_mpi[p]->custo, 1, MPI_INT, dst , tag, inter_comm[p], &status);
		printf("Recebendo custo %d de filho %d\n", path_min_mpi[p]->custo, p);
		path_min_mpi[p]->caminho = (int *) calloc(path_min_mpi[p]->n_vert, sizeof(int));
		// Então recebe o conjunto de vértices do caminho
		MPI_Recv(path_min_mpi[p]->caminho, path_min_mpi[p]->n_vert, MPI_INT, dst , tag, inter_comm[p], &status);
	}

	for(int k = 0; i < vertices->n_vert; i++) {
		path_min_mpi[k]->custo += matriz[ORIGEM * dim + vertices->vert[k]];
	}

	int index_min = find_index_min(path_min_mpi, dim - 1);
	min_path *path_min = init_min_path(dim);
	path_min->custo = path_min_mpi[index_min]->custo;
	append_min_path(ORIGEM, path_min->caminho, path_min_mpi[index_min]);

	printf("menor custo: %d\n", path_min->custo);

	for(int i = 0; i < path_min->n_vert; i++) {
		if (i == path_min->n_vert - 1) {
			printf("%d --> %d\n", path_min->caminho[i], path_min->caminho[0]);
			break;
		}
		printf("%d --> ", path_min->caminho[i]);
	}

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
