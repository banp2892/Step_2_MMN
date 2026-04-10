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

    double a = std::stod(argv[1]);
    double b = std::stod(argv[2]);
    double c = std::stod(argv[3]);
    double d = std::stod(argv[4]);
    int n = std::stoi(argv[5]);
    int m = std::stoi(argv[6]);
    double e_max = std::stod(argv[7]);
    int n_max = std::stoi(argv[8]);
    int task_type = std::stoi(argv[9]);

    try {
        DirihleVPuassone solver(a, b, c, d, n, m);

        // 1. Подготовка и начальная невязка
        if (task_type == 0) solver.prepare_v_and_i_test();
        else solver.prepare_v_and_i_main();

        double initial_res = solver.get_initial_residual(); // Нужно добавить в .cpp [cite: 116, 153]

        // 2. Основной расчет
        auto duration = howLong([&]() {
            DirihleVPuassone::solver_iterator(solver, e_max, n_max);
            });
        std::chrono::milliseconds time_for_2n_2m = std::chrono::milliseconds(0);

        // 3. Сбор данных для справки
        double error_val = 0.0;
        double mx = 0.0, my = 0.0;
        std::string task_name = (task_type == 0 ? "ТЕСТОВАЯ" : "ОСНОВНАЯ");

        if (task_type == 0) {
            // Тестовая задача: погрешность e1 [cite: 63, 64]
            error_val = solver.get_test_error(mx, my);
        }
        else {
            // Основная задача: точность e2 (требует вторую сетку 2n x 2m)
            std::cout << "Запуск на измельченной сетке для оценки e2..." << std::endl;
            DirihleVPuassone solver2(a, b, c, d, n * 2, m * 2);
            
            solver2.prepare_v_and_i_main();
            time_for_2n_2m = howLong([&]() {
                DirihleVPuassone::solver_iterator(solver2, e_max, n_max);
            });
            

            error_val = solver.compare_with_half_step(solver2, mx, my);
        }

        // 4. Сохранение результатов
        save_to_binary("output_grid.bin", solver.v);


        std::ofstream stats("stats.txt", std::ios::app);
        if (stats.is_open()) {
            stats << "=== СПРАВКА: " << task_name << " ЗАДАЧА ===" << "\n";
            std::time_t now = std::time(nullptr);
            std::tm ltm;
            localtime_s(&ltm, &now); 
            stats << "Дата запуска: "
                << std::put_time(&ltm, "%d.%m.%Y %H:%M:%S") << "\n";
            stats << "Сетка: " << n << " x " << m << "\n";
            stats << "Метод: Минимальных невязок\n";
            stats << "Параметры остановки: eps=" << e_max << ", Nmax=" << n_max << "\n";
            stats << "Начальная невязка: " << initial_res << "\n";
            stats << "Итераций затрачено: " << solver.last_iterations << "\n";
            stats << "Достигнутая точность метода: " << solver.final_eps << "\n";

                if (task_type == 0) {
                    stats << "Погрешность e1: " << error_val << "\n";
                }
                else {
                    stats << "Точность e2 (сравнение сеток): " << error_val << "\n";
                }
            stats << "Узел макс. отклонения: x=" << mx << ", y=" << my << "\n";
                stats << "Время расчета на изначально сетке: " << duration.count() << " мс\n";
                stats << "Время расчета на удвоенной сетке: " << time_for_2n_2m.count() << " мс\n";
            stats << "------------------------------------------\n\n";
            stats.close();
        }

        std::cout << "\nРасчет завершен успешно (SUCCESS)." << std::endl;
        std::cout << "Результат записан в stats.txt" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}