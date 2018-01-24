
SRC_FFT = fft_openmp.c

#all:test.fft

fft.so:$(SRC_FFT)
	#gcc $^ -o $@ -lm
	gcc -fpic -fopenmp -c $^ -I/home/gpdba/greenplum/include/postgresql/server
	gcc -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o fft.so

