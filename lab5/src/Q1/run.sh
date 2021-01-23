icpc -fPIC -shared -o libparallel_for.so parallel_for.cpp -march=native  -O3 -Ofast -no-prec-div  -fp-model fast=2 -par-affinity=compact -Iinclude/
icpc heated_plate_openmp.cpp -L. -lparallel_for -pthread -march=native -fopenmp -O3 -Ofast -no-prec-div  -fp-model fast=2 -par-affinity=compact
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:."
export OMP_NUM_THREADS=36
valgrind --tool=massif --stacks=yes numactl -m 0 ./a.out
#numactl -m 0 ./a.out

