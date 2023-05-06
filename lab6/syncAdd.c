#include <stdio.h>
#include <pthread.h>

int x = 0;

int isFirstRead = 0;
int isSecondRead = 0;

void* first(){
    for(int i = 0; i < 1000; i++)
    {
        int temp = x;
        isFirstRead = 1;
        
        temp = temp + 1;

        while(!isSecondRead);
        x = temp;
        isSecondRead = 0;
    }
    isFirstRead = 1;
}

void* second(){
    for(int i = 0; i < 1000; i++)
    {
        int temp = x;
        isSecondRead = 1;

        temp = temp + 1;

        while(!isFirstRead);
        x = temp;
        isFirstRead = 0;
    }
    isSecondRead = 1;
}

int main(){
    for (int i = 0; i < 10000; i++){
        pthread_t t1;
        pthread_t t2;

        pthread_create(&t1, NULL, &first, NULL);
        pthread_create(&t2, NULL, &second, NULL);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        printf("%d\n", x);
        x = 0;
        isFirstRead = 0;
        isSecondRead = 0;
    }
}