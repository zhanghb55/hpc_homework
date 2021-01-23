#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define max 2048

int m = 1024,n = 1024,k = 1024;

int matrixA[max][max];
int matrixB[max][max];
int res[max][max];
MPI_Status status_p;

void Send_Matrix(int p){
    for(int i = 1;i < p;i++){
        MPI_Send(matrixA[i * m / p],m * n / p,MPI_INT,i,i,MPI_COMM_WORLD);               
        MPI_Send(matrixB[0],k * n,MPI_INT,i,i,MPI_COMM_WORLD);        
    }
}
void Recv_Matrix(int my_id,int p){
    MPI_Recv(matrixA[my_id * m / p],m * n / p,MPI_INT,0,my_id,MPI_COMM_WORLD,&status_p);
    MPI_Recv(matrixB[0],k * n,MPI_INT,0,my_id,MPI_COMM_WORLD,&status_p);
}

void calculate(int p,int my_id){
    for(int i = my_id * m / p;i < (my_id + 1) * m / p;i++){
        for(int j = 0;j < k;j++){
            for(int x = 0;x < n;x++){
                res[i][j] += matrixA[i][x] * matrixB[x][j];
            }
        }
    }
}

void Recv_Res(int p){
    for(int i = 1;i < p;i++){
        MPI_Recv(res[i * m / p],m * k / p,MPI_INT,i,i,MPI_COMM_WORLD,&status_p);            
    }
}

void Send_Res(int my_id,int p){
    MPI_Send(res[my_id * m / p],m * k / p,MPI_INT,0,my_id,MPI_COMM_WORLD);
}