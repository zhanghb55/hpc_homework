#include "parallel_for.h"
#include <semaphore.h>

sem_t sem1;

# define M 2000
# define N 2000     
# define up 0.0
# define down 100.0
# define left 100.0
# define right 100.0

struct array_and_val{
    double * w;
    double * val;
};

struct arrays{
    double * w;
    double * u;
};

struct arrays_and_val{
    double * w;
    double * u;
    double * val;
};

void* init_w_up_down(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    double * w = reinterpret_cast<double*>(parallel_arg->fun_arg);

    
    int increment = parallel_arg->increment;
    for(int i = 0;i < index.size();i += increment){
      w[index[i]] = up;
      w[(M - 1) * N + index[i]] = down;
    }
    return NULL;
}

void* init_w_left_right(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    double * w = reinterpret_cast<double*>(parallel_arg->fun_arg);

    int increment = parallel_arg->increment;
    for(int i = 0;i < index.size();i += increment){
      w[index[i] * N] = left;
      w[index[i] * N + N - 1] = right;
    }
    return NULL;
}

//把最左和最右两列按行相加
void* mean_row(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    struct array_and_val*temp = reinterpret_cast<struct array_and_val*>(parallel_arg->fun_arg);
    double * mean_pos = temp->val;
    double * w = temp->w;
    double local_mean = 0;
    
    for(int i = 0;i < index.size();i += increment){
        //printf("%d\n",index[i]);
        local_mean = local_mean + w[index[i] * N] + w[index[i] * N + N - 1];
    }

    sem_wait(&sem1);
    *mean_pos += local_mean;
    sem_post(&sem1);  


    return NULL;
}
//把最上和最下两行按列相加
void* mean_col(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    struct array_and_val*temp = reinterpret_cast<struct array_and_val*>(parallel_arg->fun_arg);
    double * mean_pos = temp->val;
    double * w = temp->w;
    double local_mean = 0;
    
    for(int i = 0;i < index.size();i += increment){
        //printf("%d\n",index[i]);
        local_mean = local_mean + w[index[i]] + w[(M - 1) * N + index[i]];
    }

    sem_wait(&sem1);
    *mean_pos += local_mean;
    sem_post(&sem1);  


    return NULL;
}

void * assignment_mean(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    struct array_and_val*temp = reinterpret_cast<struct array_and_val*>(parallel_arg->fun_arg);
    double * mean_pos = temp->val;
    double * w = temp->w;
    double local_mean = 0;

    for(int i = 0;i < index.size();i += increment){

        for(int j = 1;j < N - 1;j++){
            w[index[i] * N + j] = *mean_pos;
        }
    }
}

void * iteration(void* arg){
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    struct arrays*temp = reinterpret_cast<struct arrays*>(parallel_arg->fun_arg);
    double * w = temp->w;
    double * u = temp->u;
    
    for(int i = 0;i < index.size();i += increment){
        #pragma ivdep
        for(int j = 1;j < N - 1;j++){
            w[index[i] * N + j] = (u[(index[i] - 1) * N + j] + u[(index[i] + 1) * N + j] + u[index[i] * N + j - 1] + u[index[i] * N + j + 1]) / 4.0;
        }
    }
}

void * cal_diff(void* arg){
    double my_diff = 0.0;
    parallel_arg* parallel_arg = reinterpret_cast<struct parallel_arg*> (arg);
    std::vector<int> index = parallel_arg->index;
    int increment = parallel_arg->increment;
    struct arrays_and_val*temp = reinterpret_cast<struct arrays_and_val*>(parallel_arg->fun_arg);
    double * u = temp->u;
    double * w = temp->w;
    double * diff = temp->val;
    for(int i = 0;i < index.size();i += increment){
        for(int j = 1;j < N - 1;j++){
            if(my_diff < fabs(w[index[i] * N + j] - u[index[i] * N + j])){
                my_diff = fabs(w[index[i] * N + j] - u[index[i] * N + j]);
            }
        }
    }
    sem_wait(&sem1);
    if(*diff < my_diff){
        *diff = my_diff;
    }
    sem_post(&sem1);  

}
