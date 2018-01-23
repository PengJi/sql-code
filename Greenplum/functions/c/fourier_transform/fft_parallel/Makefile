
SRC_FFT = fft_mpi.c

#all:test.fft

fft.so:$(SRC_FFT)
	#gcc $^ -o $@ -lm
	#cc -fpic -c $^ -I/home/gpdba/greenplum/include/postgresql/server -lm
	mpicc -fpic -c $^ -I/home/gpdba/greenplum/include/postgresql/server -lm
	mpicc -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o fft.so

