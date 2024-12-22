#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>

int main(int argc, char *argv[]) {
    const int n = std::stoi(argv[1]); // Размер вектора
    std::vector<int> vec(n);
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Используем ID потока для генератора
        #pragma omp for
        for (int i = 0; i < n; ++i) {
            vec[i] = rand_r(&seed) % n;
        }
    }

    int min_value = std::numeric_limits<int>::max();
    int max_value = std::numeric_limits<int>::min();

    // Нахождение минимального и максимального без редукции
    #pragma omp parallel
    {
        int local_min = std::numeric_limits<int>::max();
        int local_max = std::numeric_limits<int>::min();

        #pragma omp for
        for (int i = 0; i < n; ++i) {
            if (vec[i] < local_min) local_min = vec[i];
            if (vec[i] > local_max) local_max = vec[i];
        }

        #pragma omp critical
        {
            if (local_min < min_value) min_value = local_min;
            if (local_max > max_value) max_value = local_max;
        }
    }

    //std::cout << "Минимум: " << min_value << "\n";
    //std::cout << "Максимум: " << max_value << "\n";

    return 0;
}
