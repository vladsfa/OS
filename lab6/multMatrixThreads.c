#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define N 100
#define M 100
#define K 100

int** A;
int** B;
int** C;

int RowsPerThread;

void construct_matrix(int*** __matrix, int rows, int cols){
    *__matrix = malloc(sizeof(int*) * rows);
    int** matrix = *__matrix;
    for(int i = 0; i < rows; i++){
        matrix[i] = malloc(sizeof(int*) * cols);
        for(int j = 0; j < cols; j++){
            matrix[i][j] = 1;
        }
    }
}

void destruction_matrix(int** matrix, int rows, int cols){
    for (int i = 0; i < rows; i++){
        free(matrix[i]);
    }
    free(matrix);
}

void print_matrix(int** matrix, int rows, int cols){
    printf("Result:\n");
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("[%d, %d] = %d\n", i, j, matrix[i][j]);
        }
    }
}

int eval_cell(int __i, int __j){
    int sum = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < K; j++){
            sum += A[__i][j]*B[i][__j];
        }
    }
    return sum;
}

void* mult(void* __rowStart){
    int rowStart = *(int*)__rowStart;

    for(int i = rowStart; i < rowStart + RowsPerThread; i++){
        for(int j = 0; j < K; j++){
            C[i][j] = eval_cell(i, j);
        }
    }

    free((int*)__rowStart);
}

void multN(int __threads){
    pthread_t t[__threads];
    RowsPerThread = N / __threads;
    int isRemain = N % __threads != 0 ? 1 : 0;


    for(int i = 0; i < __threads - isRemain; i++){
        int* p_rowStart = malloc(sizeof(int));
        *p_rowStart = i * RowsPerThread;
        pthread_create(&t[i], NULL, &mult, p_rowStart);
    }
    if (isRemain){
        int* p_rowStart = malloc(sizeof(int));
        *p_rowStart = N % __threads;
        pthread_create(&t[__threads - 1], NULL, &mult, p_rowStart);
    }

    for(int i = 0; i < __threads; i++){
        pthread_join(t[i], NULL);
    }
}

int main(){
    construct_matrix(&A, N, M);
    construct_matrix(&B, M, K);
    construct_matrix(&C, N, K);

    for(int threads = 1; threads < 10; threads++){
        time_t start = clock();
        multN(threads);
        time_t end = clock();

        printf("threads: %d\n", threads);
        printf("time: %f\n", (((double)end - start) / CLOCKS_PER_SEC) * 1000);
        /*print_matrix(C, N, K);*/
        printf("\n");
    }

    destruction_matrix(A, N, M);
    destruction_matrix(B, M, K);
    destruction_matrix(C, N, K);
}