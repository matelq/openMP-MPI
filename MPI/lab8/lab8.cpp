#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

void exchange_messages(int rank, int size, int message_size, int iterations) {
    // Создаем буферы для отправки и получения
    std::vector<char> send_buffer(message_size, rank);  // Буфер отправки, заполняем значением rank
    std::vector<char> recv_buffer(message_size, 0);     // Буфер получения

    MPI_Status status;

    // Измеряем время для классического подхода (Send + Recv)
    MPI_Barrier(MPI_COMM_WORLD);  // Синхронизация процессов

    auto start_classic = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        if (rank == 0) {
            MPI_Send(send_buffer.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recv_buffer.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
        } else if (rank == 1) {
            MPI_Recv(recv_buffer.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(send_buffer.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }
    auto end_classic = std::chrono::high_resolution_clock::now();

    if (rank == 0) {
        std::chrono::duration<double> duration_classic = end_classic - start_classic;
        std::cout << "Send + Recv: " << duration_classic.count() << " секунд\n";
    }

    // Измеряем время для подхода с MPI_Sendrecv
    MPI_Barrier(MPI_COMM_WORLD);  // Синхронизация процессов

    auto start_sendrecv = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        MPI_Sendrecv(send_buffer.data(), message_size, MPI_CHAR, (rank + 1) % 2, 0,  // Отправка
                     recv_buffer.data(), message_size, MPI_CHAR, (rank + 1) % 2, 0,  // Прием
                     MPI_COMM_WORLD, &status);
    }
    auto end_sendrecv = std::chrono::high_resolution_clock::now();

    if (rank == 0) {
        std::chrono::duration<double> duration_sendrecv = end_sendrecv - start_sendrecv;
        std::cout << "MPI_Sendrecv: " << duration_sendrecv.count() << " секунд\n";
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получаем номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Получаем количество процессов

    // Проверка на количество процессов (должно быть ровно 2)
    if (size != 2) {
        if (rank == 0) {
            std::cerr << "Для этой задачи необходимо использовать ровно 2 процесса." << std::endl;
        }
        MPI_Finalize();
        return -1;
    }

    int message_size = 22222222;  // Размер сообщения в байтах
    int iterations = 1000;   // Количество обменов сообщениями

    if (rank == 0) {
        std::cout << "Размер сообщения: " << message_size << " байт, число итераций: " << iterations << "\n";
    }

    // Тестируем обмен сообщениями
    exchange_messages(rank, size, message_size, iterations);

    MPI_Finalize();  // Завершаем работу с MPI
    return 0;
}
