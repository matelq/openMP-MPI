#include <iostream>
#include <vector>
#include <omp.h>
#include <random>

// Функция для генерации случайного вектора
std::vector<int> generate_vector(size_t size, int seed) {
    std::vector<int> vec(size);
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(1, 100);

    for (size_t i = 0; i < size; ++i) {
        vec[i] = distribution(generator);
    }
    return vec;
}

// Функция для вычисления скалярного произведения
int scalar_product(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    int result = 0;

    #pragma omp parallel for reduction(+:result)
    for (size_t i = 0; i < vec1.size(); ++i) {
        result += vec1[i] * vec2[i];
    }

    return result;
}

int main() {
    size_t vector_size = 10000000; // Размер векторов
    int seed1 = 42;              // Сид для генерации первого вектора
    int seed2 = 24;              // Сид для генерации второго вектора

    std::vector<int> vec1, vec2;
    int result = 0;

    double start_time = omp_get_wtime();

    #pragma omp parallel sections
    {
        // Генерация первого вектора
        #pragma omp section
        {
            vec1 = generate_vector(vector_size, seed1);
        }

        // Генерация второго вектора
        #pragma omp section
        {
            vec2 = generate_vector(vector_size, seed2);
        }

        #pragma barrier

        #pragma omp section
        {
            result = scalar_product(vec1, vec2);
        }
    }

    // Вычисление скалярного произведения

    double end_time = omp_get_wtime();

    //std::cout << "Scalar product: " << result << "\n";
    std::cout << "" << (end_time - start_time) << ",\n";

    return 0;
}
