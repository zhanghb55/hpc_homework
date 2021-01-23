CC = gcc-9 

SRCS = $(wildcard *.c *.h)


TARGET = pthread_equation

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -w -o $@ $^ -lpthread

clean:
	rm $(TARGET)
