#include <iostream>
#include <vector>
#include <omp.h>

int main(int argc, char *argv[]) {
    // Проверка на наличие аргумента для размера вектора
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <vector_size>" << std::endl;
        return 1;
    }

    // Размер векторов
    const int n = std::stoi(argv[1]);

    // Инициализация векторов
    std::vector<int> vec1(n);
    std::vector<int> vec2(n);

    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Используем ID потока для генератора
        #pragma omp for
        // Заполнение векторов случайными числами
        for (int i = 0; i < n; ++i) {
            vec1[i] = rand_r(&seed) % 100; // Случайные числа от 0 до 99
            vec2[i] = rand_r(&seed) % 100; // Случайные числа от 0 до 99
        }
    }

    // Переменная для результата скалярного произведения
    long long dot_product = 0;

    // Вычисление скалярного произведения с использованием OpenMP
    #pragma omp parallel for reduction(+:dot_product)
    for (int i = 0; i < n; ++i) {
        dot_product += vec1[i] * vec2[i];
    }

    return 0;
}
