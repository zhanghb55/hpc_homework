#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#define target 5

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex;

pthread_cond_t cond;

double a,b,c;
double sqrt_delta;
double numerator1;
double res1;
double numerator2;
double res2;

int count = 0;

int flag = 0;

void calculate_sqrt_delta(){
    double delta = b * b - 4 * a * c;
    if(delta == 0){
        flag = 1;
    }
    else if(delta < 0)flag = -1;
    sqrt_delta = sqrt(delta);
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
}

void calculate_numerator_1(){
    pthread_mutex_lock(&mutex1);
    numerator1 = -b + sqrt_delta;
}

void calculate_numerator_2(){
    pthread_mutex_lock(&mutex2);
    numerator2 = -b - sqrt_delta;
}

void calculate_res1(){
    res1 = numerator1 / (2 * a);
}
void calculate_res2(){
    res2 = numerator2 / (2 * a);
}

void * calculate(void*p){
    int tid = (int)p;
    if(tid == 0){
        calculate_sqrt_delta();
    }
    if(tid == 1){
        calculate_numerator_1();
    }
    if(tid == 2){
        calculate_numerator_2();
    }
    pthread_mutex_lock(&mutex);
    count++;
    if(count == target){
        pthread_cond_broadcast(&cond);
    }
    else{
        while(pthread_cond_wait(&cond,&mutex) != 0);
    }
    pthread_mutex_unlock(&mutex);

    if(tid == 3)calculate_res1();
    if(tid == 4)calculate_res2();
} 


int main(){
    pthread_mutex_init(&mutex1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    pthread_mutex_init(&mutex,NULL);

    pthread_mutex_lock(&mutex1);
    pthread_mutex_lock(&mutex2);


    pthread_cond_init(&cond,NULL);
    printf("Please input a, b, c.\n");
    scanf("%lf%lf%lf",&a,&b,&c);
    pthread_t tid[5];
    for(int i = 0;i < 5;i++){
        pthread_create(&tid[i],NULL,calculate,i);
    }
    for(int i = 0;i < 5;i++){
        pthread_join(tid[i],NULL);
    }
    if(flag == 1){
        printf("x1 = x2 = %lf\n",res1);
    }
    else if(!flag){
        printf("x1 = %lf, x2 = %lf\n",res1,res2);
    }
    else if(flag == -1){
        printf("No solution.\n");
    }
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&cond);
}