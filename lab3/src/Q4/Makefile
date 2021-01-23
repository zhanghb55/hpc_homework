CC = g++-9 

SRCS = $(wildcard *.cpp *.h)


TARGET = pthread_Monte_Carlo

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -w -o $@ $^ -lpthread -fpermissive

clean:
	rm $(TARGET)
