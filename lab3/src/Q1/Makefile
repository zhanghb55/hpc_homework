CC = gcc-9 

SRCS = $(wildcard *.c *.h)


TARGET = pthread_matrix_multiply

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -w -o $@ $^ -lpthread -O3

clean:
	rm $(TARGET)
