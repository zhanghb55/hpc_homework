#include <omp.h>

#define max 2048

int Matrix_a[max][max];
int Matrix_b[max][max];
int res[max][max];

int m,n,k;
int thread_count = 8;

void parallel_calculate(){
    #pragma omp parallel for num_threads(64) schedule(static,1)
    for(int i = 0;i < m;i++){
	#pragma ivdep
	#pragma vector aligned
        for(int j = 0;j < n;j++){
            for(int x = 0;x < k;x++){
                res[i][x] += Matrix_a[i][j] * Matrix_b[j][x];
            }
        }
    }
}
