typedef int T;

void add(T** MatrixA, T** MatrixB, T** MatrixResult, int MatrixSize )
{
    int i,j;
    for (i = 0; i < MatrixSize; i++)
    {
        for (j = 0; j < MatrixSize; j++)
        {
            MatrixResult[i][j] =  MatrixA[i][j] + MatrixB[i][j];
        }
    }
}

void sub(T** MatrixA, T** MatrixB, T** MatrixResult, int MatrixSize )
{
    int i,j;
    for (i = 0; i < MatrixSize; i++)
    {
        for (j = 0; j < MatrixSize; j++)
        {
            MatrixResult[i][j] =  MatrixA[i][j] - MatrixB[i][j];
        }
    }
}

void mul( T** MatrixA, T** MatrixB, T** MatrixResult, int MatrixSize )
{
    int i,j,k;
    for (i=0;i<MatrixSize ;i++)
    {
        for (j=0;j<MatrixSize ;j++)
        {
            MatrixResult[i][j]=0;
            for (k=0;k<MatrixSize ;k++)
            {
                MatrixResult[i][j]=MatrixResult[i][j]+MatrixA[i][k]*MatrixB[k][j];
            }
        }
    }
}


void Strassen(int N, T **MatrixA, T **MatrixB, T **MatrixC)
{
    int i,j;
    int HalfSize = N/2;
    int newSize = N/2;

    if ( N <= 64 ) 
    {
        mul(MatrixA,MatrixB,MatrixC,N);
    }
    else
    {
        T** A11, **A12, **A21, **A22, **B11, **B12, **B21, **B22, **C11, **C12, **C21, **C22, **M1, **M2, **M3, **M4, **M5, **M6, **M7, **AResult, **BResult;

        A11 = new T *[newSize];
        A12 = new T *[newSize];
        A21 = new T *[newSize];
        A22 = new T *[newSize];
        
        B11 = new T *[newSize];
        B12 = new T *[newSize];
        B21 = new T *[newSize];
        B22 = new T *[newSize];
        
        C11 = new T *[newSize];
        C12 = new T *[newSize];
        C21 = new T *[newSize];
        C22 = new T *[newSize];
        
        M1 = new T *[newSize];
        M2 = new T *[newSize];
        M3 = new T *[newSize];
        M4 = new T *[newSize];
        M5 = new T *[newSize];
        M6 = new T *[newSize];
        M7 = new T *[newSize];

        AResult = new T *[newSize];
        BResult = new T *[newSize];

        int newLength = newSize;

        for (i = 0; i < newSize; i++)
        {
            A11[i] = new T[newLength];
            A12[i] = new T[newLength];
            A21[i] = new T[newLength];
            A22[i] = new T[newLength];
            
            B11[i] = new T[newLength];
            B12[i] = new T[newLength];
            B21[i] = new T[newLength];
            B22[i] = new T[newLength];
            
            C11[i] = new T[newLength];
            C12[i] = new T[newLength];
            C21[i] = new T[newLength];
            C22[i] = new T[newLength];

            M1[i] = new T[newLength];
            M2[i] = new T[newLength];
            M3[i] = new T[newLength];
            M4[i] = new T[newLength];
            M5[i] = new T[newLength];
            M6[i] = new T[newLength];
            M7[i] = new T[newLength];

            AResult[i] = new T[newLength];
            BResult[i] = new T[newLength];


        }
        for (i = 0; i < N / 2; i++)
        {
            for (int j = 0; j < N / 2; j++)
            {
                A11[i][j] = MatrixA[i][j];
                A12[i][j] = MatrixA[i][j + N / 2];
                A21[i][j] = MatrixA[i + N / 2][j];
                A22[i][j] = MatrixA[i + N / 2][j + N / 2];

                B11[i][j] = MatrixB[i][j];
                B12[i][j] = MatrixB[i][j + N / 2];
                B21[i][j] = MatrixB[i + N / 2][j];
                B22[i][j] = MatrixB[i + N / 2][j + N / 2];

            }
        }


        add( A11,A22,AResult, HalfSize);
        add( B11,B22,BResult, HalfSize);               
        Strassen( HalfSize, AResult, BResult, M1 ); 
        add( A21,A22,AResult, HalfSize);           
        Strassen(HalfSize, AResult, B11, M2);  
        sub( B12,B22,BResult, HalfSize);          
        Strassen(HalfSize, A11, BResult, M3);  
        sub( B21, B11, BResult, HalfSize);        
        Strassen(HalfSize, A22, BResult, M4);     
        add( A11, A12, AResult, HalfSize);          
        Strassen(HalfSize, AResult, B22, M5);      
        sub( A21, A11, AResult, HalfSize);
        add( B11, B12, BResult, HalfSize);            
        Strassen( HalfSize, AResult, BResult, M6);
        sub(A12, A22, AResult, HalfSize);
        add(B21, B22, BResult, HalfSize);          
        Strassen(HalfSize, AResult, BResult, M7);   
        add( M1, M4, AResult, HalfSize);
        sub( M7, M5, BResult, HalfSize);
        add( AResult, BResult, C11, HalfSize);
        add( M3, M5, C12, HalfSize);
        add( M2, M4, C21, HalfSize);
        add( M1, M3, AResult, HalfSize);
        sub( M6, M2, BResult, HalfSize);
        add( AResult, BResult, C22, HalfSize);


        for (i = 0; i < N/2 ; i++)
        {
            for (j = 0 ; j < N/2 ; j++)
            {
                MatrixC[i][j] = C11[i][j];
                MatrixC[i][j + N / 2] = C12[i][j];
                MatrixC[i + N / 2][j] = C21[i][j];
                MatrixC[i + N / 2][j + N / 2] = C22[i][j];
            }
        }



        for (i = 0; i < newLength; i++)
        {
            delete[] A11[i];delete[] A12[i];delete[] A21[i];
            delete[] A22[i];

            delete[] B11[i];delete[] B12[i];delete[] B21[i];
            delete[] B22[i];
            delete[] C11[i];delete[] C12[i];delete[] C21[i];
            delete[] C22[i];
            delete[] M1[i];delete[] M2[i];delete[] M3[i];delete[] M4[i];
            delete[] M5[i];delete[] M6[i];delete[] M7[i];
            delete[] AResult[i];delete[] BResult[i] ;
        }
        delete[] A11;delete[] A12;delete[] A21;delete[] A22;
        delete[] B11;delete[] B12;delete[] B21;delete[] B22;
        delete[] C11;delete[] C12;delete[] C21;delete[] C22;
        delete[] M1;delete[] M2;delete[] M3;delete[] M4;delete[] M5;
        delete[] M6;delete[] M7;
        delete[] AResult;
        delete[] BResult ;

    }

}

