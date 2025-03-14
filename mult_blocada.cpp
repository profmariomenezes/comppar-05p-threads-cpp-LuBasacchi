#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono> // Para medir tempo


const int M = 512; // Número de linhas de A
const int N = 512; // Número de colunas de A e linhas de B
const int X = 512; // Número de colunas de B
const int BLOCK_SIZE = 64; // Tamanho do bloco


int A[M][N], B[N][X], C[M][X]; // Matrizes globais
int thread_count; // Número de threads


void* multiply_blocks(void* rank) {
    long my_rank = (long)rank;
    int rows_per_thread = M / thread_count;
    int my_start_row = my_rank * rows_per_thread;
    int my_end_row = (my_rank + 1) * rows_per_thread - 1;


    // Blocagem para otimizar o uso do cache
    for (int i = my_start_row; i <= my_end_row; i += BLOCK_SIZE) {
        for (int j = 0; j < X; j += BLOCK_SIZE) {
            for (int k = 0; k < N; k += BLOCK_SIZE) {
                for (int ii = i; ii < std::min(i + BLOCK_SIZE, M); ++ii) {
                    for (int jj = j; jj < std::min(j + BLOCK_SIZE, X); ++jj) {
                        for (int kk = k; kk < std::min(k + BLOCK_SIZE, N); ++kk) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
    return NULL;
}


int main(int argc, char* argv[]) {
    pthread_t* thread_handles;
    long thread;


    // Inicializando as matrizes A e B
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = 1; // Exemplo: todos elementos de A são 1


    for (int i = 0; i < N; i++)
        for (int j = 0; j < X; j++)
            B[i][j] = 2; // Exemplo: todos elementos de B são 2


    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = new pthread_t[thread_count];


    auto start = std::chrono::high_resolution_clock::now(); // Começar cronômetro


    // Criar threads
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, multiply_blocks, (void*)thread);
    }


    // Esperar threads terminarem
    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }


    auto end = std::chrono::high_resolution_clock::now(); // Fim do cronômetro
    std::chrono::duration<double> elapsed = end - start;


    std::cout << "Tempo de execução: " << elapsed.count() << " segundos\n";


    // Exibir parte do resultado (opcional, já que o output pode ser muito grande)
    std::cout << "Resultado (amostra):\n";
    for (int i = 0; i < std::min(10, M); i++) {
        for (int j = 0; j < std::min(10, X); j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << "\n";
    }


    delete[] thread_handles;
    return 0;
}
