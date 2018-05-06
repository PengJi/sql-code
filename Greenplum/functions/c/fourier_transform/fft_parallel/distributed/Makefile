CC = gcc
CFLAGS = -g -Wall -I/home/gpdba/greenplum/include/postgresql/server
LDFLAGS = -lm
SRC_FFT = fft.c
OBJ_FFT = $(patsubst %c, %o, $(SRC_FFT))
BIN_FFT = fft
LIB_FFT = fft.so

all : $(LIB_FFT)

$(OBJ_FFT) : $(SRC_FFT)
	$(CC) $(CFLAGS) -fpic -c $^ -o $@

$(LIB_FFT) : $(OBJ_FFT)
	$(CC) $(LDFLAGS) -shared -o $@ $^

.PHONY: clean
clean :
	rm -f *.o *.so

