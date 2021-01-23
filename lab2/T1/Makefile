CC = mpicc

SRCS = $(wildcard *.c *.h)

TARGET = T2

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $^

clean:
	rm *o 