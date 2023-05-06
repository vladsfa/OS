#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 100000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 0;

void* routine(void* n){
    int __n = *(int*)n;

    for(int i = 0; i < __n; i++){
        pthread_mutex_lock(&mutex);
        x++;
        pthread_mutex_unlock(&mutex);
    }
}

int main(){

    for (int i = 1; i < 10; i++){
        time_t start = clock();
        
        pthread_t t[i];
        int threads = i;
        int n = N;
        for(int j = 0; j < i; j++){
            int* increments = malloc(sizeof(int));
            *increments = n / threads;

            pthread_create(&t[j], NULL, &routine, (void*)increments);

            threads--;
            n -= *increments; 
        }

        for(int j = 0; j < i; j++){
            pthread_join(t[j], NULL);
        }

        time_t end = clock();
        printf("threads: %d\n", i);
        printf("x: %d\n", x);
        printf("miliseconds: %f\n", (((double)end - start) / CLOCKS_PER_SEC) * 1000);
        x = 0;
    }
}