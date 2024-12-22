#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>
#include <cstdlib>
#include <chrono>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <bandwidth>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]); // Размер матрицы
    int bandwidth = std::stoi(argv[2]); // Ширина полосы

    // Генерация ленточной матрицы
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 0));

    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Уникальный seed для каждого потока
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            for (int j = std::max(0, i - bandwidth); j <= std::min(N - 1, i + bandwidth); ++j) {
                matrix[i][j] = rand_r(&seed) % 1000; // Заполнение только в пределах полосы
            }
        }
    }

    // Варианты распределения работы
    const char* schedules[] = {"static", "dynamic", "guided"};
    for (const char* schedule : schedules) {
        std::cout << "Распределение итераций: " << schedule << std::endl;

        int max_of_mins = std::numeric_limits<int>::min();

        // Замер времени выполнения
        auto start_time = std::chrono::high_resolution_clock::now();

        // Устанавливаем правило распределения
        if (std::string(schedule) == "static") {
            omp_set_schedule(omp_sched_static, 0);
        } else if (std::string(schedule) == "dynamic") {
            omp_set_schedule(omp_sched_dynamic, 0);
        } else if (std::string(schedule) == "guided") {
            omp_set_schedule(omp_sched_guided, 0);
        }

        // Поиск максимального среди минимальных элементов строк
        #pragma omp parallel for reduction(max: max_of_mins) schedule(runtime)
        for (int i = 0; i < N; ++i) {
            int min_in_row = std::numeric_limits<int>::max();

            // Вычисляем границы столбцов для текущей строки
            int j_start = std::max(0, i - bandwidth);
            int j_end = std::min(N - 1, i + bandwidth);

            for (int j = j_start; j <= j_end; ++j) {
                if (matrix[i][j] < min_in_row) {
                    min_in_row = matrix[i][j];
                }
            }
            max_of_mins = std::max(max_of_mins, min_in_row);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        // Вывод результатов
        std::cout << "Максимум среди минимумов: " << max_of_mins << std::endl;
        std::cout << "Время выполнения: " << elapsed.count() << " секунд" << std::endl;
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}
