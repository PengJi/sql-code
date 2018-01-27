CXXFLAGS = -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = fft.c

#all:test.fft

fft.so:$(SRC_FFT)
	gcc $(CXXFLAGS) -fpic -c $^
	gcc $(LDFLAGS) -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so


