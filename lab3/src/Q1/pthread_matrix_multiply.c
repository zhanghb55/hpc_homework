#include <stdio.h>
#include <stdlib.h>
#include  <sys/time.h>
#include <pthread.h>

#include "lib.h"
#define MaxElement 10000
int main(){
    printf("Input M:");
    scanf("%d",&m);
    if(m < 512 || m > 2048){
        printf("Out of range!\n");
        return 0;
    }
    printf("Input N:");
    scanf("%d",&n);
    if(n < 512 || n > 2048){
        printf("Out of range!\n");
        return 0;
    }
    printf("Input K:");
    scanf("%d",&k);
    if(k < 512 || k > 2048){
        printf("Out of range!\n");
        return 0;
    }

    struct timeval begin,end;
    gettimeofday(&begin,NULL);


    pthread_t*thread_id = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);

    for(int i = 0;i < thread_count;i++){
        pthread_create(&thread_id[i],NULL,parallel_calculate,i);
    }

    
    for(int i = 0;i < thread_count;i++){
        pthread_join(thread_id[i],NULL);
    }

    free(thread_id);


    gettimeofday(&end,NULL);

    printf("Time is:%ldμs\n",end.tv_sec*1000000 + end.tv_usec - begin.tv_sec*1000000 - begin.tv_usec);
}