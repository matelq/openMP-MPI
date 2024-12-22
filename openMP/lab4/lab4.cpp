#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>

int main(int argc, char *argv[]) {
    // Проверка наличия аргументов
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size>" << std::endl;
        return 1;
    }

    // Размер матрицы (N x N)
    int N = std::stoi(argv[1]);

    // Инициализация матрицы случайными числами
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Используем ID потока для генератора
        #pragma omp for collapse(2)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matrix[i][j] = rand_r(&seed) % 1000; // Генерация случайных чисел
            }
        }
    }

    // Для каждого i находим минимальный элемент строки
    int max_of_mins = std::numeric_limits<int>::min();

    // Параллельный расчет с использованием OpenMP
    
    #pragma omp parallel for reduction(max: max_of_mins)
    for (int i = 0; i < N; ++i) {
        int min_in_row = std::numeric_limits<int>::max();
        for (int j = 0; j < N; ++j) {
            if (matrix[i][j] < min_in_row) {
                min_in_row = matrix[i][j];
            }
        }
        // Для каждой строки обновляем максимальное значение среди минимальных
        max_of_mins = std::max(max_of_mins, min_in_row);
    }

    // Вывод результата
    //std::cout << "Максимальное значение среди минимальных элементов строк: " << max_of_mins << std::endl;

    return 0;
}
