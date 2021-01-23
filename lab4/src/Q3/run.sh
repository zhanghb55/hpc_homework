dir=$(pwd)
icpc -fPIC -shared -o libparallel_for.so parallel_for.cpp -lpthread
icpc My_Parallel_MatrixMul.cpp -L. -lparallel_for -o my_parallel_for_matrix_mul -O3 -w -lpthread
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$dir
numactl -m 1 ./my_parallel_for_matrix_mul
