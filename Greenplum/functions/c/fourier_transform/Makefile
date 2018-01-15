
SRC_FFT = main.c zx_fft.c zx_math.c

#all:test.fft

test.fft:$(SRC_FFT)
	gcc $^ -o $@ -lm

.PHONY:clean
clean:
	rm -f test_fft.txt test.fft

