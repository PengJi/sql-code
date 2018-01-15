
SRC_FFT = main.c fft.c fft_utils.c

#all:test.fft

fft:$(SRC_FFT)
	gcc $^ -o $@ -lm

.PHONY:clean
clean:
	rm -f test_fft.txt

