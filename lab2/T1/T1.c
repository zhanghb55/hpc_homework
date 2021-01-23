#include "lib.h"


int main(int argc, char **argv){
    if(argc == 4){
        m = strtol(argv[1],NULL,10);
        n = strtol(argv[2],NULL,10);
        k = strtol(argv[3],NULL,10);
    }
    int my_id = 0;
    int p;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    if(my_id == 0){
        for(int i = 0;i < m;i++){
            for(int j = 0;j < n;j++){
                matrixA[i][j] = rand() % 10;
            }
        }
        for(int i = 0;i < n;i++){
            for(int j = 0;j < k;j++){
                matrixB[i][j] = rand() % 10;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double my_start,my_end,my_elapsed,elapsed;
    my_start = MPI_Wtime();

    if(my_id == 0){
        Send_Matrix(p);
    }
    else{
       Recv_Matrix(my_id,p);
    }

    calculate(p,my_id);

    if(my_id == 0){
        Recv_Res(p);
    }
    else{
        Send_Res(my_id,p);
    }

    my_end = MPI_Wtime();

    my_elapsed = my_end - my_start;

    MPI_Reduce(&my_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if(my_id == 0){
        printf("Parallel time is:%e seconds\n",elapsed);
    }
    MPI_Finalize();
}