//GEMM
void matrix_multiply(int ** Matrix_a,int ** Matrix_b,int ** res,int m,int k,int n){
    for(int i = 0;i < m;i++){
        for(int j = 0;j < k;j++){
            for(int q = 0;q < n;q++){
                res[i][j] += Matrix_a[i][q] * Matrix_b[q][j];
            }
        }
    }
} 


