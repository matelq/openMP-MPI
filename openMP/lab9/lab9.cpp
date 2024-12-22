#include <iostream>
#include <vector>
#include <omp.h>
#include <random>

const size_t N = 5000; // Размер матрицы

// Функция для генерации случайных чисел в матрице
void generate_matrix(std::vector<std::vector<int>>& matrix) {
    std::mt19937 gen(42);

    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Используем ID потока для генератора
        #pragma omp for collapse(2)
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                matrix[i][j] = rand_r(&seed);
            }
        }
    }
}

// Функция для вычисления суммы элементов матрицы без вложенного параллелизма
int sum_matrix_no_nested(const std::vector<std::vector<int>>& matrix) {
    int sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            sum += matrix[i][j];
        }
    }

    return sum;
}

// Функция для вычисления суммы элементов матрицы с вложенным параллелизмом
int sum_matrix_with_nested(const std::vector<std::vector<int>>& matrix) {
    int sum = 0;
    
    #pragma omp parallel
    {
        #pragma omp parallel for reduction(+:sum)
        for (size_t i = 0; i < N; ++i) {
            int linesum = 0;
            #pragma omp parallel for reduction(+:linesum)
            for (size_t j = 0; j < N; ++j) {
                linesum += matrix[i][j];
            }
            sum += linesum;
        }
    }

    return sum;
}

int main() {
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));
    generate_matrix(matrix);

    double start_time, end_time;
    
    // Без вложенного параллелизма
    start_time = omp_get_wtime();
    int sum_no_nested = sum_matrix_no_nested(matrix);
    end_time = omp_get_wtime();
    //std::cout << "Sum without nested parallelism: " << sum_no_nested << std::endl;
    std::cout << "Time without nested parallelism: " << (end_time - start_time) << " seconds" << std::endl;

    // Включаем вложенный параллелизм
    omp_set_nested(1);
    
    // С вложенным параллелизмом
    start_time = omp_get_wtime();
    int sum_with_nested = sum_matrix_with_nested(matrix);
    end_time = omp_get_wtime();
    //std::cout << "Sum with nested parallelism: " << sum_with_nested << std::endl;
    std::cout << "Time with nested parallelism: " << (end_time - start_time) << " seconds" << std::endl;

    return 0;
}
