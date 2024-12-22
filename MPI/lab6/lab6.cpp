#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

// Функция для синхронной передачи данных
void sync_send_recv(int rank, int size, std::vector<int>& data, int N) {
    MPI_Status status;
    if (rank == 0) {
        // Отправка данных процессом 0
        MPI_Ssend(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // Получение данных процессом 0 от процесса 1
        MPI_Recv(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    } else if (rank == 1) {
        // Получение данных процессом 1 от процесса 0
        MPI_Recv(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // Отправка данных процессом 1
        MPI_Ssend(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

// Функция для асинхронной передачи данных
void async_send_recv(int rank, int size, std::vector<int>& data, int N) {
    MPI_Request request;
    MPI_Status status;

    if (rank == 0) {
        // Асинхронная отправка данных процессом 0
        MPI_Isend(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        // Асинхронный прием данных от процесса 1
        MPI_Irecv(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    } else if (rank == 1) {
        // Асинхронный прием данных от процесса 0
        MPI_Irecv(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        // Асинхронная отправка данных процессом 1
        MPI_Isend(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    }
}

// Функция для буферизованной передачи данных
void buffered_send_recv(int rank, int size, std::vector<int>& data, int N) {
    MPI_Buffer_attach(malloc(N * sizeof(int)), N * sizeof(int));
    MPI_Status status;

    if (rank == 0) {
        // Буферизованная отправка данных процессом 0
        MPI_Bsend(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // Получение данных процессом 0
        MPI_Recv(data.data(), N, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    } else if (rank == 1) {
        // Получение данных процессом 1
        MPI_Recv(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        // Буферизованная отправка данных процессом 1
        MPI_Bsend(data.data(), N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Buffer_detach(NULL, NULL);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получаем номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Получаем количество процессов

    // Проверка на количество процессов (должно быть ровно 2)
    if (size != 2) {
        std::cerr << "Для этой задачи необходимо использовать ровно 2 процесса." << std::endl;
        MPI_Finalize();
        return -1;
    }

    int N = 2147483642;  // Размер данных (например, 1000 элементов)
    std::vector<int> data(N, rank);  // Заполняем вектор значениями, равными номеру процесса

    // Засекаем время для синхронного обмена
    if (rank == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();
        sync_send_recv(rank, size, data, N);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time;
        std::cout << "Время синхронного обмена: " << diff.count() << " секунд" << std::endl;
    } else {
        sync_send_recv(rank, size, data, N);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Синхронизируем процессы перед запуском следующего теста

    // Засекаем время для асинхронного обмена
    if (rank == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();
        async_send_recv(rank, size, data, N);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time;
        std::cout << "Время асинхронного обмена: " << diff.count() << " секунд" << std::endl;
    } else {
        async_send_recv(rank, size, data, N);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Синхронизируем процессы перед запуском следующего теста

    /*
    // Засекаем время для буферизованного обмена
    if (rank == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();
        buffered_send_recv(rank, size, data, N);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time;
        std::cout << "Время буферизованного обмена: " << diff.count() << " секунд" << std::endl;
    } else {
        buffered_send_recv(rank, size, data, N);
    }
    */

    // Завершаем работу с MPI
    MPI_Finalize();
    return 0;
}
