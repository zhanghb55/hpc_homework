#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "cuda_runtime.h"
#include "cublas_v2.h"
using namespace std;

#define IDX2F(i,j,ld) ((((j)-1)*(ld))+((i)-1))
#define IDX2C(i,j,ld) (((j)*(ld))+(i))

void CPU_MatMul(double * A, double * B ,double * C,int m,int n,int k){
    for(int i = 0;i < m;i++){
        for(int j = 0;j < k;j++){
            for(int x = 0;x < n;x++){
                C[IDX2C(i,j,k)] += A[i * n + x] * B[x * k + j];
            }
        }
    }
}
int main()
{
    int m,n,k;
    timeval t1, t2;
    cout << "Input problem size:";
    cin >> m;
    n = m;
    k = m;
    cublasHandle_t handle;
    cublasCreate(&handle);
    double *A,*B,*C;
    A = (double*)malloc(sizeof(double) * m * n);
    B = (double*)malloc(sizeof(double) * k * n);
    C = (double*)malloc(sizeof(double) * m * k);
    
    for(int i = 0;i < m;i++){
        for(int j = 0;j < n;j++){
            A[i * n + j] = rand() % 10;
        }
    }
    for(int i = 0;i < n;i++){
        for(int j = 0;j < k;j++){
            B[i * k + j] = rand() % 10;
        }
    }
    
    memset(C,0,sizeof(C));

    double * d_A,*d_B,*d_C;
    gettimeofday(&t1, NULL);
    cudaMalloc(&d_A, sizeof(double) * m * n);
    cudaMalloc(&d_B,sizeof(double) * n * k);
    cudaMalloc(&d_C,sizeof(double) * m * k);
    cudaMemcpy(d_A, A, sizeof(double) * m * n, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, sizeof(double) * n * k, cudaMemcpyHostToDevice);
    gettimeofday(&t1, NULL);
    double a = 1,b = 0;
    cublasDgemm( 
            handle,
            CUBLAS_OP_T, CUBLAS_OP_T,   
            m, n, k,
            &a, //alpha
            d_A, n, 
            d_B, k,         
            &b, //beta        
            d_C, m
    );

    cudaMemcpy(C, d_C, sizeof(double) * m * k, cudaMemcpyDeviceToHost);
    gettimeofday(&t2, NULL);
    printf("GPU time is:%ldμs\n",t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec);




    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    free(A);
    free(B);
    free(C);
    cublasDestroy(handle);
}

