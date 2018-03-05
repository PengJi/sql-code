CXXFLAGS = -I/home/gpadmin/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = fft.c

#all:test.fft

fft.so:$(SRC_FFT)
	mpicc $(CXXFLAGS) -fpic -c $^
	mpicc $(LDFLAGS) -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so


