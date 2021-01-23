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
    parallel_calculate();
    gettimeofday(&end,NULL);

    printf("Time is:%ldμs\n",end.tv_sec*1000000 + end.tv_usec - begin.tv_sec*1000000 - begin.tv_usec);
}