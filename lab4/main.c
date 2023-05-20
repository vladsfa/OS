#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define N 5
#define PRINT_LEN 30
#define SLEEP_TIME 5000

int forks[N];
int ate[N];
int n_ate;
int points[N];

pthread_mutex_t mtx_get_forks;

struct forks_n{
    int left;
    int right;
} typedef forks_n;

pthread_mutex_t mtx_print;

void print(char* msg){
    pthread_mutex_lock(&mtx_print);
    printf("%s\n", msg);
    pthread_mutex_unlock(&mtx_print);
}

pthread_mutex_t mtx_rand;

int get_rand(int max_mls){
    pthread_mutex_lock(&mtx_rand);
    int mls = (rand() % max_mls) / 1000;
    pthread_mutex_unlock(&mtx_rand);
    return mls;
}

void reset_ate(){
    for(int i = 0; i < N; i++){
        ate[i] = 0;
    }
    n_ate = 0;
}

forks_n get_forks_n(int n){
    forks_n forks__;

    forks__.right = n;
    if (n - 1 < 0){
        forks__.left  =  N - 1;
    }
    else{
        forks__.left  =  n - 1;
    }

    return forks__;
}

pthread_mutex_t mtx_notify;

void ate_notify(int n){
    pthread_mutex_lock(&mtx_notify);
    ate[n] = 1;
    n_ate += 1;
    if (n_ate == 5){
        reset_ate();
    }
    pthread_mutex_unlock(&mtx_notify);
}

void eat(int n){
    char msg[PRINT_LEN];
    sprintf(msg, "%d eating", n);
    print(msg);
    
    sleep(get_rand(SLEEP_TIME));
    points[n] += 1;
    ate_notify(n);
}

void think(int n){
    char msg[PRINT_LEN];
    sprintf(msg, "%d thinking", n);
    print(msg);

    sleep(get_rand(SLEEP_TIME));
}

void release_forks(int n){
    forks_n __forks = get_forks_n(n);
    forks[__forks.right] = 1;
    forks[__forks.left] = 1;
}

void get_forks(int n){
    while(ate[n])
        sleep(0.05);

    forks_n __forks = get_forks_n(n);

    while(1){
        pthread_mutex_lock(&mtx_get_forks);

        if (!forks[__forks.right] || !forks[__forks.left]){
            pthread_mutex_unlock(&mtx_get_forks);
            continue;
        }

        forks[__forks.right] = 0;
        forks[__forks.left] = 0;

        pthread_mutex_unlock(&mtx_get_forks);
        break;
    }

}

void* start_dinner(void* __n){
    int n = *(int*)__n;
    while (1){
        get_forks(n);
        eat(n);
        release_forks(n);
        think(n);
    }

    free((int*)__n);
}

#define PRINT_POINTS_INTERVAL 6

void* print_points(void* arg){
    sleep(PRINT_POINTS_INTERVAL);
    while(1){
        char msg[PRINT_LEN * N];
        char temp[PRINT_LEN];

        strcat(msg, "\n");
        for (int i = 0; i < N; i++){
            sprintf(temp, "%d phil: %d\n", i, points[i]);
            strcat(msg, temp);
        }

        print(msg);
        memset(msg, 0,  PRINT_LEN * N);
        memset(msg, 0,  PRINT_LEN);

        sleep(PRINT_POINTS_INTERVAL);
    }
}

int main(){
    srand(time(NULL));
    reset_ate();
    for(int i = 0; i < N; i++){
        forks[i] = 1;
        points[i] = 0;
    }

    pthread_t print_points_t;
    pthread_create(&print_points_t, NULL, &print_points, NULL);

    pthread_t threads[N];
    for (int i = 0; i < N; i++){
        int* n = malloc(sizeof(int));
        *n = i;
        pthread_create(&threads[i], NULL, &start_dinner, (void*)n);
    }

    for (int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}