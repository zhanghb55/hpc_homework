#include <stdio.h>
#include <stdlib.h>
#include  <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#define Size 1000

int a[Size];
sem_t s;
int sum = 0;
int global_index = 0;
int thread_count = 8;
void* add(void* p){
    int my_id = (int)p;
    while(1){
        sem_wait(&s);
        if(global_index < Size){
            sum += a[global_index];
            global_index++;
        }
        else{
            sem_post(&s);
            break;
        }
        sem_post(&s);
    }
}
int main(){
    sem_init(&s,0,1);
    for(int i = 0;i < 1000;i++){
        a[i] = i;
    }
    pthread_t tid[thread_count];
    for(int i = 0;i < thread_count;i++){
        pthread_create(&tid[i],NULL,add,i);
    }
    
    for(int i = 0;i < thread_count;i++){
        pthread_join(tid[i],NULL);
    }
    printf("%d\n",sum);
}
