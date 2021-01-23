#include <iostream>
#include <ctime>
#include <string.h>
#include  <sys/time.h>
#include "lib.h"

using namespace std;


int main()
{

    int MatrixSize;
    cout << "Input Matrix size:";
    cin >> MatrixSize;
    int** MatrixA;   
    int** MatrixB;   
    int** MatrixC;   

    int N = MatrixSize;

    MatrixA = new int *[MatrixSize];
    MatrixB = new int *[MatrixSize];
    MatrixC = new int *[MatrixSize];
    int i;
    for (i = 0; i < MatrixSize; i++)
    {
        MatrixA[i] = new int [MatrixSize];
        MatrixB[i] = new int [MatrixSize];
        MatrixC[i] = new int [MatrixSize];
        memset(MatrixA[i], 0, sizeof(int)*MatrixSize);
        memset(MatrixB[i], 0, sizeof(int)*MatrixSize);
        memset(MatrixC[i], 0, sizeof(int)*MatrixSize);
    }
    struct timeval begin,end;
    gettimeofday(&begin,NULL);
    Strassen( N, MatrixA, MatrixB, MatrixC );
    gettimeofday(&end,NULL);
    printf("Time is:%ldμs\n",end.tv_sec*1000000 + end.tv_usec - begin.tv_sec*1000000 - begin.tv_usec);
    return 0;

}
