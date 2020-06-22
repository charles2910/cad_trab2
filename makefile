# Makefile do projeto. make all gera o executável e make clean remove os *.o e
# o executável.

all: pcv_seq pcv_master pcv_slave

pcv_seq: pcv_seq.c
	gcc -fopenmp -o pcv_seq pcv_seq.c

pcv_master: pcv_master.c
	mpicc -fopenmp -o pcv_master pcv_master.c

pcv_slave: pcv_slave.c
	mpicc -fopenmp -o pcv_slave pcv_slave.c

clean:
	rm -f *.o *~ pcv_slave pcv_master pcv_seq
