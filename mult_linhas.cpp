#include <iostream>
#include <vector>
#include <pthread.h>


const int M = 4; // Número de linhas de A
const int N = 4; // Número de colunas de A e linhas de B
const int X = 4; // Número de colunas de B


int A[M][N], B[N][X], C[M][X];
int thread_count;


void* multiply_columns(void* rank) {
    long my_rank = (long)rank;
    int local_x = X / thread_count;
    int my_first_col = my_rank * local_x;
    int my_last_col = (my_rank + 1) * local_x - 1;


    for (int j = my_first_col; j <= my_last_col; j++) {
        for (int i = 0; i < M; i++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    pthread_t* thread_handles;
    long thread;


    // Inicializa as matrizes A e B
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = 1;


    for (int i = 0; i < N; i++)
        for (int j = 0; j < X; j++)
            B[i][j] = 2;


    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = new pthread_t[thread_count];


    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, multiply_columns, (void*)thread);
    }


    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }


    // Exibe o resultado
    std::cout << "Resultado da Multiplicação (Ordem de Coluna):\n";
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < X; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << "\n";
    }


    delete[] thread_handles;
    return 0;
}
