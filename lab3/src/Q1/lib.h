#define max 2048

int Matrix_a[max][max];
int Matrix_b[max][max];
int res[max][max];

int m,n,k;
int thread_count = 8;

void* parallel_calculate(void*p){
    int thread_id = (int)p;
    for(int i = thread_id * m / thread_count;i < (thread_id + 1) * m / thread_count;i++){
        for(int j = 0;j < n;j++){
            for(int x = 0;x < k;x++){
                res[i][x] += Matrix_a[i][j] * Matrix_b[j][x];
            }
        }
    }
}