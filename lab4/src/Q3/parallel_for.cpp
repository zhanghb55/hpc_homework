#include "parallel_for.h"

void parallel_for(int start,int end,int increment,void *(*functor)(void*),void *arg,int num_threads,int chunksize){
    struct parallel_arg argument[num_threads];
    for(int i = start;i < end;i += increment){
        argument[((i - start) / (chunksize * increment) ) % num_threads].index.push_back(i);//将索引按块划分给指定线程
    }
    pthread_t tid[num_threads];
    for(int i = 0;i < num_threads;i += increment){
        argument[i].fun_arg = arg;
        argument[i].increment = increment;
        pthread_create(&tid[i],NULL,functor,&argument[i]);
    }
    for(int i = 0;i < num_threads;i++){
        pthread_join(tid[i],NULL);
    }
}