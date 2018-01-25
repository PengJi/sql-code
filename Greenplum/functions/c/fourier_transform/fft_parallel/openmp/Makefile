CXXFLAGS = -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = fft_openmp.c

#all: fft.so

fft.so: $(SRC_FFT)
	gcc $(CXXFLAGS) -fpic -fopenmp -c $^
	gcc $(LDFLAGS) -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o fft.so

