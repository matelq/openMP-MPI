#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <mutex>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <vector_size>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]); // Размер вектора
    std::vector<int> vec(N, 1); // Инициализация вектора (все элементы равны 1)

    // Переменные для хранения суммы
    long long sum_atomic = 0;
    long long sum_critical = 0;
    long long sum_lock = 0;
    long long sum_reduction = 0;

    omp_lock_t lock;
    omp_init_lock(&lock);

    // Замер времени для атомарной операции
    auto start_atomic = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        #pragma omp atomic
        sum_atomic += vec[i];
    }
    auto end_atomic = std::chrono::high_resolution_clock::now();

    // Замер времени для критической секции
    auto start_critical = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        #pragma omp critical
        {
            sum_critical += vec[i];
        }
    }
    auto end_critical = std::chrono::high_resolution_clock::now();

    // Замер времени для использования замков
    auto start_lock = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        omp_set_lock(&lock);
        sum_lock += vec[i];
        omp_unset_lock(&lock);
    }
    auto end_lock = std::chrono::high_resolution_clock::now();

    // Замер времени для редукции
    auto start_reduction = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for reduction(+:sum_reduction)
    for (int i = 0; i < N; ++i) {
        sum_reduction += vec[i];
    }
    auto end_reduction = std::chrono::high_resolution_clock::now();

    omp_destroy_lock(&lock);

    // Вычисление времени выполнения
    std::chrono::duration<double> elapsed_atomic = end_atomic - start_atomic;
    std::chrono::duration<double> elapsed_critical = end_critical - start_critical;
    std::chrono::duration<double> elapsed_lock = end_lock - start_lock;
    std::chrono::duration<double> elapsed_reduction = end_reduction - start_reduction;

    // Вывод результатов
    std::cout << "Результаты редукции:" << std::endl;
    std::cout << "Сумма (атомарные операции): " << sum_atomic 
              << ", Время: " << elapsed_atomic.count() << " секунд" << std::endl;

    std::cout << "Сумма (критическая секция): " << sum_critical 
              << ", Время: " << elapsed_critical.count() << " секунд" << std::endl;

    std::cout << "Сумма (замки): " << sum_lock 
              << ", Время: " << elapsed_lock.count() << " секунд" << std::endl;

    std::cout << "Сумма (редукция): " << sum_reduction 
              << ", Время: " << elapsed_reduction.count() << " секунд" << std::endl;

    return 0;
}
