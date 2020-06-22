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
	int i, j, n_vert;

	FILE *fp;

	if (argc > 1)
		fp = fopen(argv[1], "r");
	else
		fp = fopen("caixeiro-4", "r");

	// Lê a dimensão das matrizes
	fscanf(fp, "%d\n", &dim);
	printf("Dimensão: %d\n", dim);
 
	// Declara a matriz global
	int *matriz;

	// Aloca a matriz
	matriz = (int *) malloc(dim *dim * sizeof(int));

	// Lê a matriz
	for(i = 0; i < dim; i++) {
		for(j = 0; j < dim; j++) {
			fscanf(fp, "%d ", &(matriz[i*dim+j]));
			printf("\t%d",matriz[i * dim + j]);
		}
	}

	printf("Argumento: %s\n", argv[1]);

	// Preparação do MPI - Declarações
	int  master_tag = 1, my_rank, num_proc, src, dst, root;
	char slave[40];
	int  *errcodes, *origs, buf_rcv, *vet_master, **paths;

	c_vert ** verts_mpi;
	min_path ** path_min_mpi;

	// Alocações dinâmicas
	verts_mpi = (c_vert **) calloc(dim - 1, sizeof(c_vert *));
	path_min_mpi = (min_path **) calloc(dim - 1, sizeof(min_path *));
	errcodes = (int *) calloc(dim - 1, sizeof(int));
	origs = (int *) calloc(dim - 1, sizeof(int));
	vet_master = (int *) calloc(dim - 1, sizeof(int));
	paths = (int **) calloc(dim - 1, sizeof(int *));

	if (dim > 12) {
		printf("Tem certeza que quer fazer isso?\n");
		printf("Eu tenho certeza que você não quer fazer isso.\n");
		return 0;
	}


	for (int t =0; t < dim - 1; t++) {
		origs[t] = t + 1;
		verts_mpi[t] = init_c_vert(dim - 2);
	}

	for(int t = 0; t < dim - 1; t++) {
		int marcacao = 0;
		for (int j = 0; j < dim - 1; j++) {
			if (origs[j] == origs[i]) {
				marcacao = 1;
				continue;
			}
			if (marcacao)
				verts_mpi[t]->vert[j - 1] = origs[j];
			else
				verts_mpi[t]->vert[j] = origs[j];
		}
	}

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
    
	MPI_Status  	status;
	MPI_Comm    	inter_comm;
	MPI_Info 		info_param = 0;


	strcpy(slave,"pcv_slave");
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
	
	MPI_Get_processor_name(processor_name, &name_len);

	MPI_Info_create(&info_param);
	MPI_Info_set(info_param, "hostfile", "./halley.txt");


	src = dst = root = 0;
	//MPI_Comm_spawn(slave, argv + 1, NUM_SPAWNS, info_param, root, MPI_COMM_WORLD, &inter_comm, errcodes);


	//MPI_Send(msg_0, 50, MPI_CHAR, dst, master_tag, inter_comm);
	
	//MPI_Send(master_data, 50, MPI_CHAR, dst, master_tag, inter_comm);
    
	for (i = 0; i < dim; i++) {
		//MPI_Send(&(origs[i]), 1, MPI_CHAR, 
		//printf("Master from %s: msg_1=%s,vet[%d]=%d,buf_rcv=%d,vet_master[%d]=%d\n",processor_name,msg_1,i,vet[i],buf_rcv,i,vet_master[i]);
	}

	int  array_of_errcodes[10], tag = 0;
	char message_0[] = "hello slave, i'm your master";
	char master_data[] = "slaves to work";
	char message_1[50];
	printf("MASTER on processor %s : spawning %d slaves ... \n", processor_name, NUM_SPAWNS);

	/* spawn slave and send it a message */  
	strcpy(slave,"./pcv_slave");
	MPI_Comm_spawn(slave, MPI_ARGV_NULL, NUM_SPAWNS, MPI_INFO_NULL, dst, MPI_COMM_WORLD, &inter_comm, array_of_errcodes);

	
	printf("MASTER : send a message to master of slaves (%s) ...\n", message_0);
	MPI_Send(message_0, 50, MPI_CHAR, dst , tag, inter_comm);

	MPI_Recv(message_1, 50, MPI_CHAR, dst, tag, inter_comm, &status);
	printf("MASTER : message received : %s\n", message_1);

	MPI_Send(master_data, 50, MPI_CHAR, dst , tag, inter_comm);





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
