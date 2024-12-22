#include <iostream>
#include <vector>
#include <mpi.h>
#include <limits>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получаем идентификатор процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получаем общее количество процессов

    // Размер вектора и его инициализация
    const int N = 20000000;
    std::vector<int> vec;

    // Инициализация вектора на процессе 0
    if (rank == 0) {
        vec.resize(N);
        for (int i = 0; i < N; ++i) {
            vec[i] = rand() % 1000; // Случайные числа от 0 до 999
        }
    }

    // Размер подзадачи для каждого процесса
    int local_size = N / size;
    std::vector<int> local_vec(local_size);

    // Распределение данных между процессами
    MPI_Scatter(vec.data(), local_size, MPI_INT, local_vec.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Засекаем время начала
    double start_time = MPI_Wtime();

    // Поиск минимального значения на локальной части вектора
    int local_min = std::numeric_limits<int>::max();
    for (int i = 0; i < local_size; ++i) {
        if (local_vec[i] < local_min) {
            local_min = local_vec[i];
        }
    }

    // Сбор всех локальных минимальных значений на процессе 0
    int global_min;
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    // Засекаем время окончания
    double end_time = MPI_Wtime();

    // Печать результата на процессе 0
    if (rank == 0) {
        // std::cout << "Минимальное значение вектора: " << global_min << std::endl;
        std::cout << "" << end_time - start_time << "," << std::endl;
    }

    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
