CXXFLAGS = -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lm
#SRC_FFT = fft_openmp.c
SRC_FFT = hello.c

#all: fft.so

fft_openmp.so: $(SRC_FFT)
	gcc $(CXXFLAGS) -fpic -c $^
	gcc $(LDFLAGS) -shared -fopenmp -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so

