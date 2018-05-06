CFLAGS = -g -Wall -Dfft -I/home/gpdba/greenplum/include/postgresql/server -fpic -std=c99
LDFLAGS = -lm

FFT_SRC = fft.c comb.c
#编译所有c文件
#FFT_SRC = $(wildcard *.c source/*.c) 
FFT_OBJ = $(patsubst %c, %o, $(FFT_SRC))
TARGET = fft

.PHONY:all clean 

all:$(TARGET)

$(TARGET):$(FFT_OBJ)
	gcc $(LDFLAGS) -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean :
	rm -f *.o *.so $(TARGET)
