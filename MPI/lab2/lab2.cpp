#include <iostream>
#include <vector>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // Инициализация MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Получаем идентификатор процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Получаем общее количество процессов

    // Размер векторов
    const int N = 20000000;
    std::vector<int> vec1, vec2;

    // Инициализация векторов на процессе 0
    if (rank == 0) {
        vec1.resize(N);
        vec2.resize(N);
        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
            vec1[i] = rand() % 100; // Случайные числа от 0 до 99
            vec2[i] = rand() % 100;
        }
    }

    // Размер подзадачи для каждого процесса
    int local_size = N / size;
    std::vector<int> local_vec1(local_size), local_vec2(local_size);

    // Распределение данных между процессами
    MPI_Scatter(vec1.data(), local_size, MPI_INT, local_vec1.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vec2.data(), local_size, MPI_INT, local_vec2.data(), local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Засекаем время начала
    double start_time = MPI_Wtime();

    // Вычисление локального скалярного произведения
    int local_dot_product = 0;
    for (int i = 0; i < local_size; ++i) {
        local_dot_product += local_vec1[i] * local_vec2[i];
    }

    // Сбор всех локальных скалярных произведений на процессе 0
    int global_dot_product = 0;
    MPI_Reduce(&local_dot_product, &global_dot_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Засекаем время окончания
    double end_time = MPI_Wtime();

    // Печать результата на процессе 0
    if (rank == 0) {
        //std::cout << "Скалярное произведение: " << global_dot_product << std::endl;
        std::cout << "" << end_time - start_time << "," << std::endl;
    }

    MPI_Finalize(); // Завершение работы MPI
    return 0;
}
