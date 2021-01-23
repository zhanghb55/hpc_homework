.PHONY: build test clean

build: libmatrix.so

libmatrix.so: matrix.o
	gcc -o $@  -shared $<

matrix.o: matrix.c
	gcc -c -fPIC $<

test: a.out

a.out: test.c libmatrix.so
	gcc test.c -L. -lmatrix
	LD_LIBRARY_PATH=. ./a.out

clean:
	rm -f *.o *.so a.out

