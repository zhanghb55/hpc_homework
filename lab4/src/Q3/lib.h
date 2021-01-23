#include "parallel_for.h"
#include <string.h>
struct Matrix_Mul{
    int ** Matrix_a;
    int ** Matrix_b;
    int ** res;
    int m;
    int k;
    int n;
};

//GEMM
void calculate(int ** Matrix_a,int ** Matrix_b,int ** res,int m,int k,int n){
    for(int i = 0;i < m;i++){
        for(int j = 0;j < k;j++){
            for(int q = 0;q < n;q++){
                res[i][j] += Matrix_a[i][q] * Matrix_b[q][j];
            }
        }
    }
} 

void* parallel(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    Matrix_Mul* matrix_arg = (Matrix_Mul*)parallel_arg->fun_arg;
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    for(int i = 0;i < index.size();i += increment){
        for(int j = 0;j < matrix_arg->k;j++){
            for(int q = 0;q < matrix_arg->n;q++){
                matrix_arg->res[index[i]][j] += matrix_arg->Matrix_a[index[i]][q] * matrix_arg->Matrix_b[q][j];
            }
        }
    }
}