#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <random>
#include <iostream>
#include <string>

std::uniform_real_distribution<double> u(0,1);
long long int number_in_domain = 0;
int thread_count = 8;//线程数量
long long int number_of_tosses = 1000000000;
pthread_mutex_t lock;
sem_t s;

void * monte_carlo(void* p){
    int64_t tid = reinterpret_cast<int64_t> (p);
    std::mt19937 e;
    time_t temp = time(NULL);
    e.seed(tid + temp % 1000);
    long long int local_sum = 0;
    for(int i = tid * number_of_tosses / thread_count;i < (tid + 1) * number_of_tosses / thread_count;i++){
        double x = u(e);
        double y = u(e);
        if(y <= x * x){
            local_sum += 1;
        }    
    }
    pthread_mutex_lock(&lock);
    number_in_domain += local_sum;
    pthread_mutex_unlock(&lock);
}
int main(int argc,char **argv) {
    std::string s(argv[1]); 
    number_of_tosses = stoi(s);
    pthread_mutex_init(&lock,NULL);
    pthread_t id[thread_count];
    for(int i = 0;i < thread_count;i++){
        pthread_create(&id[i],NULL,monte_carlo,(void*)i);
    }
    for(int i = 0;i < thread_count;i++){
        pthread_join(id[i],NULL);
    }
    pthread_mutex_destroy(&lock);
    printf("res = %f\n",1.0 * number_in_domain / number_of_tosses);
}
