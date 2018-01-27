CXXFLAGS = -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lpthread
SRC_FFT = pthread.c

#all: fft.so

fft_pthreads.so: $(SRC_FFT)
	gcc $(CXXFLAGS) -fpic -c $^
	gcc $(LDFLAGS) -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o *.so

