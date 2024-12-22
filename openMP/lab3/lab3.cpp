#include <iostream>
#include <omp.h>
#include <cmath>

// Пример функции для интегрирования
double f(double x) {
    return x*x; // Измените функцию при необходимости
}

double calculate_integral(double a, double b, int N) {
    // Шаг разбиения
    double h = (b - a) / N;

    // Переменная для результата интеграла
    double integral = 0.0;

    int max_threads = 0;

    // Вычисление интеграла методом прямоугольников
    
    #pragma omp parallel for reduction(+:integral)
    for (int i = 0; i < N; ++i) {
        int cur_threads = omp_get_num_threads();
        if (max_threads < cur_threads)
            max_threads = cur_threads;
        
        double x_i = a + i * h;
        integral += f(x_i);
    }
    
    // Умножение на шаг h
    integral *= h;

    return integral;
}

int main(int argc, char* argv[]) {
    // Проверка аргументов
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <a> <b> <N>" << std::endl;
        return 1;
    }

    // Считывание границ интегрирования и количества разбиений
    double a = std::stod(argv[1]);
    double b = std::stod(argv[2]);
    int N = std::stoi(argv[3]);

    // Вычисление интеграла
    double result = calculate_integral(a, b, N);

    // Вывод результата
    //std::cout << "Определенный интеграл: " << result << std::endl;

    return 0;
}
