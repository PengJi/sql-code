CXXFLAGS = -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = fft_mpi.c

#all:test.fft

fft_mpi.so:$(SRC_FFT)
	#gcc $^ -o $@ -lm
	mpicc $(CXXFLAGS) -fpic -c $^
	mpicc $(LDFLAGS) -fopenmp -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so

