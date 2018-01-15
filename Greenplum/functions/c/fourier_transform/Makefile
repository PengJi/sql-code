
SRC_FFT = main.c fft.c fft_utils.c

#all:test.fft

fft.so:$(SRC_FFT)
	#gcc $^ -o $@ -lm
	cc -fpic -c $^ -I/home/gpdba/greenplum/include/postgresql/server -lm
	cc -shared -o $@ *.o

.PHONY: clean
clean :
	rm -f *.o fft.so

