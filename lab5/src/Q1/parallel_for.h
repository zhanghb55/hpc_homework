#include <pthread.h>
#include <vector>

struct parallel_arg{
    std::vector<int> index;
    void * fun_arg;
    int increment;
};


void parallel_for(int start,int end,int increment,void *(*functor)(void*),void *arg,int num_threads,int chunksize);