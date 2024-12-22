#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>  // Для использования std::this_thread::sleep_for

void simulate_computation(int computation_time_us) {
    // Эмуляция вычислений с помощью sleep (микросекунды)
    std::this_thread::sleep_for(std::chrono::microseconds(computation_time_us));
}

void send_receive_data(int rank, int size, int message_size) {
    // Буфер для передачи сообщений
    std::vector<char> buffer(message_size, 'a');
    
    if (rank == 0) {
        // Процесс 0 отправляет данные
        MPI_Send(buffer.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(buffer.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else if (rank == 1) {
        // Процесс 1 принимает и отправляет данные
        MPI_Recv(buffer.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(buffer.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получаем номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Получаем количество процессов

    // Проверка, что процессов ровно два
    if (size != 2) {
        std::cerr << "Для этой задачи необходимо использовать ровно 2 процесса." << std::endl;
        MPI_Finalize();
        return -1;
    }

    int computation_time_us = 100000;  // Время вычислений (в микросекундах)
    int message_size = 1000000;  // Размер сообщения (в байтах)
    int num_iterations = 100;  // Количество повторений для усреднения времени

    for (int compute_time = 100; compute_time <= computation_time_us; compute_time *= 10) {
        for (int msg_size = 1000; msg_size <= message_size; msg_size *= 10) {
            double total_computation_time = 0.0;
            double total_comm_time = 0.0;

            for (int i = 0; i < num_iterations; ++i) {
                // Засекаем время вычислений
                auto start_compute = MPI_Wtime();
                simulate_computation(compute_time);  // Эмулируем вычисления
                auto end_compute = MPI_Wtime();
                total_computation_time += (end_compute - start_compute);

                // Засекаем время передачи данных
                auto start_comm = MPI_Wtime();
                send_receive_data(rank, size, msg_size);  // Эмулируем коммуникации
                auto end_comm = MPI_Wtime();
                total_comm_time += (end_comm - start_comm);
            }

            if (rank == 0) {
                double avg_compute_time = total_computation_time / num_iterations;
                double avg_comm_time = total_comm_time / num_iterations;
                std::cout << "Вычисления: " << compute_time << " микросекунд, "
                          << "Размер сообщения: " << msg_size << " байт, "
                          << "Среднее время вычислений: " << avg_compute_time << " секунд, "
                          << "Среднее время коммуникаций: " << avg_comm_time << " секунд" << std::endl;
            }
        }
    }

    MPI_Finalize();  // Завершаем работу с MPI
    return 0;
}
