#CC=icc
#CFLAGS=-std=c++11 -O3 -w -Ofast -qopenmp -no-prec-div  -fp-model fast=2 -par-affinity=compact -Iinclude/

CC=gcc
CFLAGS=-std=c11 -O3

SRCS = $(wildcard *.c)


TARGET = openmp_matrix_multiply

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm $(TARGET)

