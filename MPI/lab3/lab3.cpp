#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);  // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Получаем номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Получаем количество процессов

    if (size != 2) {
        std::cerr << "Для этой задачи необходимо использовать ровно 2 процесса." << std::endl;
        MPI_Finalize();
        return -1;
    }

    const int max_size = 200000000;  // Максимальный размер сообщения (в байтах)
    const int step = 4000000;  // Шаг изменения размера сообщения
    int num_iterations = 100;  // Количество повторений для усреднения времени

    for (int n = step; n <= max_size; n += step) {
        // Создаем буфер для передачи
        std::vector<char> send_buffer(n, 'a');  // Буфер для отправки
        std::vector<char> recv_buffer(n);  // Буфер для приема

        // Засекаем время
        double start_time = MPI_Wtime();

        for (int i = 0; i < num_iterations; ++i) {
            if (rank == 0) {
                // Процесс 0 отправляет данные
                MPI_Send(send_buffer.data(), n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_buffer.data(), n, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                // Процесс 1 принимает и отправляет данные
                MPI_Recv(recv_buffer.data(), n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_buffer.data(), n, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        double end_time = MPI_Wtime();

        // Вычисление времени передачи для одного обмена
        double elapsed_time = end_time - start_time;
        double avg_time = elapsed_time / num_iterations;

        if (rank == 0) {
            std::cout << "Размер сообщения: " << n << " байт, Время на один обмен: " << avg_time << " секунд" << std::endl;
        }
    }

    MPI_Finalize();  // Завершаем работу с MPI
    return 0;
}
