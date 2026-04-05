#include <iostream>
#include <iomanip>
#include <clocale>
#include "DirihleVPuassone.h"
#include <chrono>
#include <functional>
#include <string>
#include <fstream>

std::chrono::milliseconds howLong(std::function<void()> what) {
    auto begin = std::chrono::steady_clock::now();
    what();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
}


void save_to_binary(const std::string& filename, const std::vector<double>& data) {
    std::ofstream out(filename, std::ios::binary);
    if (out.is_open()) {
        out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(double));
        out.close();
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 10) {
        std::cerr << "Использование: " << argv[0]
            << " a b c d n m e_max n_max task_type" << std::endl;
        return 1;
    }

    // Считывание параметров
    double a = std::stod(argv[1]);
    double b = std::stod(argv[2]);
    double c = std::stod(argv[3]);
    double d = std::stod(argv[4]);
    int n = std::stoi(argv[5]);
    int m = std::stoi(argv[6]);
    double e_max = std::stod(argv[7]);
    int n_max = std::stoi(argv[8]);
    int task_type = std::stoi(argv[9]);

    // Вывод параметров в консоль для контроля
    std::cout << "--- ПАРАМЕТРЫ ЗАПУСКА ---" << std::endl;
    std::cout << "Область: [" << a << ", " << b << "] x [" << c << ", " << d << "]" << std::endl;
    std::cout << "Сетка: " << n << " x " << m << std::endl;
    std::cout << "Точность (eps): " << e_max << ", Макс. итераций: " << n_max << std::endl;
    std::cout << "Тип задачи: " << (task_type == 0 ? "Тестовая" : "Основная") << std::endl;
    std::cout << "-------------------------" << std::endl;

    try {
        DirihleVPuassone solver(a, b, c, d, n, m);

        if (task_type == 0) {
            solver.prepare_v_and_i_test();
        }
        else {
            solver.prepare_v_and_i_main();
        }

        auto duration = howLong([&]() {
            DirihleVPuassone::solver_iterator(solver, e_max, n_max);
            });

        save_to_binary("output_grid.bin", solver.v);

        // Открываем файл в режиме добавления (std::ios::app)
        std::ofstream stats("stats.txt", std::ios::app);
        if (stats.is_open()) {
            stats << "--- Запуск от " << __DATE__ << " " << __TIME__ << " ---" << "\n";
            stats << "Задача: " << (task_type == 0 ? "Test" : "Main") << "\n";
            stats << "Границы: " << a << " " << b << " " << c << " " << d << "\n";
            stats << "Сетка (N x M): " << n << " x " << m << "\n";
            stats << "Время (мс): " << duration.count() << "\n";
            stats << "Итераций: " << solver.last_iterations << "\n";
            stats << "Финальная невязка: " << solver.final_eps << "\n";
            stats << "------------------------------------------" << "\n\n";
            stats.close();
        }

        std::cout << "Время выполнения: " << duration.count() << " мс" << std::endl;
        std::cout << "Итераций выполнено: " << solver.last_iterations << std::endl;
        std::cout << "Финальная ошибка: " << solver.final_eps << std::endl;
        std::cout << "SUCCESS" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}