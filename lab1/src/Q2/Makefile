CC = g++-9

SRCS = $(wildcard *.cpp *.h)


TARGET = T2

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $^ -o $@

clean:
	rm *o 
