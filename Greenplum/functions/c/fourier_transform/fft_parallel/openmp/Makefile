
SRC_FFT = fft_openmp.c

#all:test.fft

fft.so:$(SRC_FFT)
	#gcc $^ -o $@ -lm
	#mpi parallel
	#mpicc -fpic -c $^ -I/home/gpdba/greenplum/include/postgresql/server -lm
	#mpicc -shared -o $@ *.o
	gcc -fpic -c $^ -I/home/gpdba/greenplum/include/postgresql/server -fopenmp -lm
	gcc -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o fft.so

