#include <stdio.h>
#include <stdlib.h>
#include  <sys/time.h>
#include "lib.h"
#define MaxElement 10000
int main(){
    int m,n,k;
    int thread_count;
    printf("Input number of threads:");
    scanf("%d",&thread_count);
    printf("Input M:");
    scanf("%d",&m);
    if(m < 512 || m > 2048){
        printf("Out of range!\n");
        return 0;
    }
    printf("Input N:");
    scanf("%d",&n);
    if(n < 512 || n > 2048){
        printf("Out of range!\n");
        return 0;
    }
    printf("Input K:");
    scanf("%d",&k);
    if(k < 512 || k > 2048){
        printf("Out of range!\n");
        return 0;
    }
    int ** Matrix_a, ** Matrix_b, ** res;
    Matrix_a = (int**)malloc(sizeof(int*) * m);
    Matrix_b = (int**)malloc(sizeof(int*) * n);
    res = (int**)malloc(sizeof(int*) * m);

    for(int i = 0;i < m;i++){
        Matrix_a[i] = (int*)malloc(sizeof(int) * n);
    }
    for(int i = 0;i < n;i++){
        Matrix_b[i] = (int*)malloc(sizeof(int) * k);
    }
    for(int i = 0;i < m;i++){
        res[i] = (int*)malloc(sizeof(int) * k);
    }

    for(int i = 0;i < m;i++){
        for(int j = 0;j < n;j++){
            Matrix_a[i][j] = rand() % MaxElement;
        }
    }
    for(int i = 0;i < n;i++){
        for(int j = 0;j < k;j++){
            Matrix_b[i][j] = rand() % MaxElement;
        }
    }
    for(int i = 0;i < m;i++){
        for(int j = 0;j < k;j++){
            res[i][j] = 0;
        }
    }

    Matrix_Mul matrix_arg;
    matrix_arg.k = k;
    matrix_arg.m = m;
    matrix_arg.n = n;
    matrix_arg.Matrix_a = Matrix_a;
    matrix_arg.Matrix_b = Matrix_b;
    matrix_arg.res = res;

    struct timeval begin,end;
    gettimeofday(&begin,NULL);
    //calculate(Matrix_a, Matrix_b, res, m, k, n);
    parallel_for(0,m,1,parallel,&matrix_arg,thread_count,1);
    gettimeofday(&end,NULL);
    printf("Time is:%ldμs\n",end.tv_sec*1000000 + end.tv_usec - begin.tv_sec*1000000 - begin.tv_usec);

    for(int i = 0;i < m;i++){
        free(Matrix_a[i]);
    }
    free(Matrix_a);
    for(int i = 0;i < n;i++){
        free(Matrix_b[i]);
    }
    free(Matrix_b);
    for(int i = 0;i < m;i++){
        free(res[i]);
    }
    free(res);
}