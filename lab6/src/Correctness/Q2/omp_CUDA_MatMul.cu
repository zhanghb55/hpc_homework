#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
using namespace std;


__global__ void MatMul(double * A, double * B ,double * C,int m,int n,int k)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    int sum = 0;
    if (i < m && j < k){
        for(int x = 0;x < n;x++){
            sum += A[i * n + x] * B[x * k + j];
        }
        C[i * k + j] = sum;
    }
}

void CPU_MatMul(double * A, double * B ,double * C,int m,int n,int k){
    for(int i = 0;i < m;i++){
        for(int j = 0;j < k;j++){
            for(int x = 0;x < n;x++){
                C[i * k + j] += A[i * n + x] * B[x * k + j];
            }
        }
    }
}
int main()
{
    timeval t1, t2;
    int x,y;
    cout << "Input threadsPerBlock.x:";
    cin >> x;
    cout << "Input threadsPerBlock.y:";
    cin >> y;
    dim3 threadsPerBlock(x,y);
    int m, n, k;
    cout << "Input problem size:";
    cin >> m;
    n = m;
    k = m;

    dim3 numBlocks((m % threadsPerBlock.x) ? m / threadsPerBlock.x + 1 : m / threadsPerBlock.x ,(k % threadsPerBlock.y) ? k / threadsPerBlock.y + 1 : k / threadsPerBlock.y);
    double *A,*B,*C,*C1;
    A = (double*)malloc(sizeof(double) * m * n);
    B = (double*)malloc(sizeof(double) * k * n);
    C = (double*)malloc(sizeof(double) * m * k);
    C1 = (double*)malloc(sizeof(double) * m * k);
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
    int omp_threads;
    int gpu_count;
    cudaGetDeviceCount(&gpu_count);
    cout << "Input number of omp threads:";
    cin >> omp_threads;
    gettimeofday(&t1, NULL);
    #pragma omp parallel num_threads(omp_threads)
    {
        int id = omp_get_thread_num();
        int size = omp_get_num_threads();
        cudaSetDevice(id % gpu_count);
        double * d_A,*d_B,*d_C;
        cudaMalloc(&d_A, sizeof(double) * m * n / size);
        cudaMalloc(&d_B,sizeof(double) * n * k);
        cudaMalloc(&d_C,sizeof(double) * m * k / size);

        cudaMemcpy(d_A, A + id * m * n / size, sizeof(double) * m * n / size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_B, B, sizeof(double) * n * k, cudaMemcpyHostToDevice);
        MatMul<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C, m / size, n, k);
        cudaMemcpy(C + id * m * k / size, d_C, sizeof(double) * m * k / size, cudaMemcpyDeviceToHost);
        cudaFree(d_A);
        cudaFree(d_B);
        cudaFree(d_C);
    }
    gettimeofday(&t2, NULL);
    printf("GPU time is:%ldμs\n",t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec);
    
    gettimeofday(&t1, NULL);
    CPU_MatMul(A,B,C1,m,n,k);
    gettimeofday(&t2, NULL);
    printf("CPU time is:%ldμs\n",t2.tv_sec*1000000 + t2.tv_usec - t1.tv_sec*1000000 - t1.tv_usec);

    int flag = 0;
    for(int i = 0;i < m * k;i++){
        if(fabs((C[i] - C1[i])) > 1e-4){
            flag = 1;
            break;
        }
    }
    if(flag){
        cout << "Wrong result." << endl;
    }
    else {
        cout << "The results are correct. " << endl;
    }
  
    free(A);
    free(B);
    free(C);
    free(C1);
}
