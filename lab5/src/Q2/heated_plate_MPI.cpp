# include <stdlib.h>
# include <stdio.h>
# include <math.h>
#include <mpi.h>
#include <string.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>
using namespace std;


# define M 500
# define N 500

MPI_Status status;
double u[M][N];
double w[M][N];
#define max(a,b) ((a > b)? a : b)
#define min(a,b) ((a < b)? a : b)
int main ( int argc, char *argv[] );


int main ( int argc, char *argv[] )

{
    double my_start,my_end,my_elapsed,elapsed;
    double diff;
    double epsilon = 0.001;
    int i;
    int iterations;
    int iterations_print;
    int j;
    double mean;
    double my_diff;
    double wtime;

    int my_id = 0;
    int size;
    printf ( "\n" );
    printf ( "HEATED_PLATE_OPENMP\n" );
    printf ( "  C/OpenMP version\n" );
    printf ( "  A program to solve for the steady state temperature distribution\n" );
    printf ( "  over a rectangular plate.\n" );
    printf ( "\n" );
    printf ( "  Spatial grid of %d by %d points.\n", M, N );
    printf ( "  The iteration will be repeated until the change is <= %e\n", epsilon );
    //printf ( "  Number of processors available = %d\n", omp_get_num_procs ( ) );

    mean = 0.0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf ( "  Number of processes =              %d\n",size);
    for(int i = 0;i < N;i++){
        w[M-1][i] = 100.0;
        w[0][i] = 0.0;
    }
    for(int i = max(my_id * M / size,1);i < min((my_id + 1) * M / size,M - 1);i++){
        w[i][0] = 100.0;
        w[i][N-1] = 100.0;
    }


    double loc_mean = 0;
    for(int i = max(my_id * M / size,1);i < min((my_id + 1) * M / size,M - 1);i++){
        loc_mean += w[i][0];
        loc_mean += w[i][N - 1];
    }
    if(my_id == 0){
        for(int i = 0;i < N;i++){
            loc_mean += w[M-1][i];
            loc_mean += w[0][i];
        }
    }

    MPI_Allreduce(&loc_mean,&mean,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

    mean = mean / ( double ) ( 2 * M + 2 * N - 4 );

    if(my_id == 0){
        printf ( "\n" );
        printf ( "  MEAN = %f\n", mean );
    }

    for(int i = max(1,my_id * M / size - 1);i < min(M - 1,(my_id + 1) * M / size + 1);i++){
        for(int j = 1;j < N - 1;j++){
            w[i][j] = mean;
        }
    }
    diff = epsilon;
    if(my_id == 0){
        iterations = 0;
        iterations_print = 1;
        printf ( "\n" );
        printf ( " Iteration  Change\n" );
        printf ( "\n" );
    }

    my_start = MPI_Wtime();
    while ( epsilon <= diff )
    {

        memcpy(&u[0][0],&w[0][0],sizeof(double) * M * N);


        for ( i = max(1,my_id * M / size); i < min(M - 1,(my_id + 1) * M / size); i++ )
        {
            for ( j = 1; j < N - 1; j++ )
            {
            w[i][j] = ( u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1] ) / 4.0;
            }
        }


        diff = 0.0;
        double buf1[N],buf2[N];
        int pos1 = 0,pos2 = 0;
        if(my_id != size - 1){
            MPI_Pack(&w[(my_id + 1) * M / size - 1][1],N - 2,MPI_DOUBLE,buf1,8 * (N - 2),&pos1,MPI_COMM_WORLD);
        }
        if(my_id == 0){
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD);
        }
        else if(my_id % 2 == 0 && my_id != size - 1){
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD);
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD,&status);
        }
        else if(my_id % 2 && my_id != size - 1){
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD,&status);
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD);
        }
        else if(my_id == size - 1){
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD,&status);
        }
        //&w[my_id * M / size - 1][1]
        if(my_id != 0){
            MPI_Unpack(buf2,8*(N - 2),&pos2,&w[my_id * M / size - 1][1],N - 2,MPI_DOUBLE,MPI_COMM_WORLD);
        }
        pos1 = pos2 = 0;
        if(my_id != 0){
            MPI_Pack(&w[my_id * M / size][1],N - 2,MPI_DOUBLE,buf1,8 * (N - 2),&pos1,MPI_COMM_WORLD);
        }
        if(my_id == 0){
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD,&status);
        }
        else if(my_id % 2 == 0 && my_id != size - 1){
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD,&status);
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD);
        }
        else if(my_id % 2 && my_id != size - 1){
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD);
            MPI_Recv(buf2,N - 2,MPI_DOUBLE,my_id + 1,0,MPI_COMM_WORLD,&status);
        }
        else if(my_id == size - 1){
            MPI_Send(buf1,N - 2,MPI_DOUBLE,my_id - 1,0,MPI_COMM_WORLD);
        }

        if(my_id != size - 1){
            MPI_Unpack(buf2,8*(N - 2),&pos2,&w[(my_id + 1) * M / size][1],N - 2,MPI_DOUBLE,MPI_COMM_WORLD);
        }

        my_diff = 0.0;

        
        for ( i = max(1,my_id * M / size); i < min(M - 1,(my_id + 1) * M / size); i++ )
        {
            for ( j = 1; j < N - 1; j++ )
            {
                if ( my_diff < fabs ( w[i][j] - u[i][j] ) )
                {
                my_diff = fabs ( w[i][j] - u[i][j] );
                }
            }
        }

        MPI_Allreduce(&my_diff,&diff,1,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);


        if(my_id == 0){
            iterations++;
            if ( iterations == iterations_print )
            {
            printf ( "  %8d  %f\n", iterations, diff );
            iterations_print = 2 * iterations_print;
            }
        }
    }

    my_end = MPI_Wtime();

    my_elapsed = my_end - my_start;

    MPI_Reduce(&my_elapsed,&elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    if(my_id == 0){
        printf ( "  %8d  %f\n", iterations, diff );
        printf ( "\n" );
        printf ( "  Error tolerance achieved.\n" );
        printf ( "  Wallclock time = %f\n", my_elapsed );
    }



    MPI_Finalize();
}