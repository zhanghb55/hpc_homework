CC = gcc-9 
LD = gcc-9

SRCS = $(wildcard *.c *.h)

OBJS = $(patsubst %c, %o, $(SRCS))

TARGET = T1

.PHONY:all	clean


all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -o $@ $^

%.o:%.c
	$(CC) -c $^

clean:
	rm *o 