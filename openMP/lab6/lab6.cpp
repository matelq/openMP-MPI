#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cmath>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <size>" << std::endl;
        return 1;
    }

    int N = std::stoi(argv[1]); // Количество итераций
    std::vector<int> results(N);

    // Устанавливаем распределение работы
    const char* schedules[] = {"static", "dynamic", "guided"};

    for (const char* schedule : schedules) {
        std::cout << "Режим распределения: " << schedule << std::endl;

        // Устанавливаем режим распределения
        if (std::string(schedule) == "static") {
            omp_set_schedule(omp_sched_static, 0);
        } else if (std::string(schedule) == "dynamic") {
            omp_set_schedule(omp_sched_dynamic, 0);
        } else if (std::string(schedule) == "guided") {
            omp_set_schedule(omp_sched_guided, 0);
        }

        auto start_time = std::chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num(); // Уникальный seed для каждого потока
        // Параллельный цикл с неравномерной нагрузкой
        #pragma omp parallel for schedule(runtime)
        for (int i = 0; i < N; ++i) {
            int local_sum = 0;

            // Генерация случайной нагрузки
            int workload = (i % 5 == 0) ? 40000 : 2500;
            for (int j = 0; j < workload; ++j) {
                local_sum += std::sqrt(rand_r(&seed) % 1000);
            }

            results[i] = local_sum;
        }
    }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        std::cout << "Время выполнения: " << elapsed.count() << " секунд" << std::endl;
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}
