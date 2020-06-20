#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>

int main(int argc, char ** argv) {
	// Declara as matrizes
	int *A,*B;
	// Declara as variáveis de índice
	int i,j,dim;
	// Declara o acumulador para o produto escalar global
	long resultado=0;

	// Declara um vetor para os produtos escalares locais
	long *prod_escalar;
 
	fscanf(stdin, "%d\n", &dim); // Lê a dimensão das matrizes
 
	// Aloca as matrizes
	A=(int *)malloc(dim *dim * sizeof(int));
	B=(int *)malloc(dim *dim * sizeof(int)); 

	// Aloca um vetor para armazenar os produtos escalares de cada linha
	prod_escalar=(long *)malloc(dim * sizeof(long));

	 // Lê a matriz A
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			fscanf(stdin, "%d ",&(A[i*dim+j]));
		}
	}
}
