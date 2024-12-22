#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>  // Для использования std::this_thread::sleep_for

void simulate_computation(int computation_time_us) {
    // Эмуляция вычислений с помощью sleep (микросекунды)
    std::this_thread::sleep_for(std::chrono::microseconds(computation_time_us));
}

void non_blocking_send_receive(int rank, int message_size, int computation_time_us) {
    // Буферы для передачи и получения данных
    std::vector<char> send_buffer(message_size, 'a');
    std::vector<char> recv_buffer(message_size, 0);

    MPI_Request send_request, recv_request;
    MPI_Status status;

    // Неблокирующий приём
    MPI_Irecv(recv_buffer.data(), message_size, MPI_CHAR, (rank + 1) % 2, 0, MPI_COMM_WORLD, &recv_request);

    // Неблокирующая отправка
    MPI_Isend(send_buffer.data(), message_size, MPI_CHAR, (rank + 1) % 2, 0, MPI_COMM_WORLD, &send_request);

    // Выполняем вычисления параллельно с коммуникациями
    simulate_computation(computation_time_us);

    // Ожидаем завершения операций передачи данных
    MPI_Wait(&send_request, &status);
    MPI_Wait(&recv_request, &status);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получаем номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Получаем количество процессов

    // Проверка, что процессов ровно два
    if (size != 2) {
        if (rank == 0) {
            std::cerr << "Для этой задачи необходимо использовать ровно 2 процесса." << std::endl;
        }
        MPI_Finalize();
        return -1;
    }

    int max_computation_time_us = 100000;  // Максимальное время вычислений (в микросекундах)
    int message_size = 1000000;           // Размер сообщения (в байтах)
    int num_iterations = 100;             // Количество повторений для усреднения времени

    if (rank == 0) {
        std::cout << "Размер сообщения: " << message_size << " байт, "
                  << "Максимальное время вычислений: " << max_computation_time_us << " мкс, "
                  << "Число итераций: " << num_iterations << std::endl;
    }

    for (int compute_time = 100; compute_time <= max_computation_time_us; compute_time *= 10) {
        double total_time = 0.0;

        for (int i = 0; i < num_iterations; ++i) {
            auto start = MPI_Wtime();

            // Выполняем неблокирующие передачи данных с вычислениями
            non_blocking_send_receive(rank, message_size, compute_time);

            auto end = MPI_Wtime();
            total_time += (end - start);
        }

        if (rank == 0) {
            double avg_time = total_time / num_iterations;
            std::cout << "Время вычислений: " << compute_time << " мкс, "
                      << "Среднее полное время: " << avg_time << " секунд" << std::endl;
        }
    }

    MPI_Finalize();  // Завершаем работу с MPI
    return 0;
}
