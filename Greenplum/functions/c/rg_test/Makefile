CXXFLAGS = -I/home/gpadmin/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = rg.c

#all:test.fft

rg.so:$(SRC_FFT)
	gcc $(CXXFLAGS) -fpic -c $^
	gcc $(LDFLAGS) -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so


