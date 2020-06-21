//compilar: mpicc pcv_master.c -o pcv_master
//executar: mpirun pcv_master -np 1 pcv_master

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define NUM_SPAWNS 4
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

	

	// Preparação do MPI
	int  master_tag = 1, my_rank, num_proc, src, dst, root;
	char slave[40];
	int  *errcodes, i, *origs, buf_rcv, *vet_master, **paths;

	c_vert ** verts_mpi;

	verts_mpi = (c_vert **) calloc(dim - 1, sizeof(c_vert *));
	path_min_mpi = (min_path **) calloc(dim - 1, sizeof(min_path *));
	errorcodes = (int *) calloc(dim - 1, sizeof(int));
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
	MPI_Comm_spawn(slave, argv + 1, NUM_SPAWNS, info_param, root, MPI_COMM_WORLD, &inter_comm, errcodes);


	//MPI_Send(msg_0, 50, MPI_CHAR, dst, master_tag, inter_comm);
	
	//MPI_Send(master_data, 50, MPI_CHAR, dst, master_tag, inter_comm);
    
	for (i = 0; i < dim; i++) {
		//MPI_Send(&(origs[i]), 1, MPI_CHAR, 
		printf("Master from %s: msg_1=%s,vet[%d]=%d,buf_rcv=%d,vet_master[%d]=%d\n",processor_name,msg_1,i,vet[i],buf_rcv,i,vet_master[i]);

	MPI_Finalize();
	exit(0);
}
