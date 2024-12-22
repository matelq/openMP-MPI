#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

void generate_matrix(std::vector<std::vector<int>>& matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;  // Случайные числа от 0 до 99
        }
    }
}

void multiply_matrices(const std::vector<std::vector<int>>& A,
                       const std::vector<std::vector<int>>& B,
                       std::vector<std::vector<int>>& C,
                       int N, int rank, int size) {
    int rows_per_process = N / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank + 1) * rows_per_process;

    // Вычисление части матрицы C
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 1000;  // Размер матриц NxN
    std::vector<std::vector<int>> A(N, std::vector<int>(N));
    std::vector<std::vector<int>> B(N, std::vector<int>(N));
    std::vector<std::vector<int>> C(N, std::vector<int>(N, 0));

    if (rank == 0) {
        generate_matrix(A, N);
        generate_matrix(B, N);
    }

    double start_time = MPI_Wtime();

    // Распараллеленное умножение матриц
    multiply_matrices(A, B, C, N, rank, size);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << "" << end_time - start_time << "," << std::endl;
    }

    MPI_Finalize();
    return 0;
}
