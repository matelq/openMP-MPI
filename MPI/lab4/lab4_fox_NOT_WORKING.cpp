#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <mpi.h>

void generate_matrix(std::vector<std::vector<int>>& matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;  // Случайные числа от 0 до 99
        }
    }
}

void multiply_matrices_fox(std::vector<std::vector<int>>& A,
                            std::vector<std::vector<int>>& B,
                            std::vector<std::vector<int>>& C,
                            int N, int rank, int size) {
    int sqrt_size = static_cast<int>(sqrt(size));
    int block_size = N / sqrt_size;

    std::vector<std::vector<int>> A_block(block_size, std::vector<int>(block_size));
    std::vector<std::vector<int>> B_block(block_size, std::vector<int>(block_size));
    std::vector<std::vector<int>> C_block(block_size, std::vector<int>(block_size, 0));

    // Печать блока для отладки
    if (rank == 0) {
        //std::cout << "A matrix block before scatter (Rank 0):" << std::endl;
        for (int i = 0; i < block_size; ++i) {
            for (int j = 0; j < block_size; ++j) {
                //std::cout << A[i][j] << " ";
            }
            //std::cout << std::endl;
        }
    }

    // Разбиение A и B на блоки
    MPI_Scatter(&A[0][0], block_size * block_size, MPI_INT, &A_block[0][0], block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&B[0][0], block_size * block_size, MPI_INT, &B_block[0][0], block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int step = 0; step < sqrt_size; ++step) {
        // Умножение блоков
        for (int i = 0; i < block_size; ++i) {
            for (int j = 0; j < block_size; ++j) {
                C_block[i][j] = 0;
                for (int k = 0; k < block_size; ++k) {
                    C_block[i][j] += A_block[i][k] * B_block[k][j];
                }
            }
        }

        MPI_Bcast(&A_block[0][0], block_size * block_size, MPI_INT, (rank + step) % sqrt_size, MPI_COMM_WORLD);
        MPI_Bcast(&B_block[0][0], block_size * block_size, MPI_INT, (rank + step) % sqrt_size, MPI_COMM_WORLD);
    }

    // Сбор результатов
    MPI_Gather(&C_block[0][0], block_size * block_size, MPI_INT, &C[0][0], block_size * block_size, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1024;  // Размер матриц NxN
    std::vector<std::vector<int>> A(N, std::vector<int>(N));
    std::vector<std::vector<int>> B(N, std::vector<int>(N));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    if (rank == 0) {
        generate_matrix(A, N);
        generate_matrix(B, N);
    }

    double start_time = MPI_Wtime();

    multiply_matrices_fox(A, B, C, N, rank, size);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << "" << end_time - start_time << "," << std::endl;
    }

    MPI_Finalize();
    return 0;
}
