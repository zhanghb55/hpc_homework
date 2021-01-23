#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define max 2048
int m = 1024,n = 1024,k = 1024;

int matrixA[max][max];
int matrixB[max][max];
int res[max][max];

int temp[max][max];
int temp_res[max][max];
MPI_Status status_p;

void Send_Matrix(int p){
    MPI_Scatter(matrixA[0],m * n / p,MPI_INT,temp[0],m * n / p,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(matrixB[0],k * n,MPI_INT,0,MPI_COMM_WORLD);
}

void calculate(int p){
    for(int i = 0;i < m / p;i++){
        for(int j = 0;j < k;j++){
            for(int x = 0;x < n;x++){
                temp_res[i][j] += temp[i][x] * matrixB[x][j];
            }
        }
    }
}

void Recv_Res(int p){
    MPI_Gather(temp_res[0],m * k / p,MPI_INT,res[0],m * k / p,MPI_INT,0,MPI_COMM_WORLD);
}
