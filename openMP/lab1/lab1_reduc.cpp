#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>

int main(int argc, char *argv[]) {
    const int n = std::stoi(argv[1]); // Размер вектора
    std::vector<int> vec(n);
    
    // Инициализация вектора
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Используем ID потока для генератора
        #pragma omp for
        for (int i = 0; i < n; ++i) {
            vec[i] = rand_r(&seed) % n; // Случайные числа от 0 до 999
        }
    }

    int min_value = std::numeric_limits<int>::max();
    int max_value = std::numeric_limits<int>::min();

    

    // Нахождение минимального и максимального с редукцией
    #pragma omp parallel for reduction(min : min_value) reduction(max : max_value)
    for (int i = 0; i < n; ++i) 
    {
        if (vec[i] < min_value) min_value = vec[i];
        if (vec[i] > max_value) max_value = vec[i];
    }

    //std::cout << "Минимум: " << min_value << "\n";
    //std::cout << "Максимум: " << max_value << "\n";

    return 0;
}
