#include <iostream>
#include <iomanip>
#include <clocale>
#include "DirihleVPuassone.h"

int main() {

    setlocale(LC_ALL, "Russian");
    std::cout << std::fixed << std::setprecision(12);

    double a = 0.0, b = 3.0;
    double c = 0.0, d = 1.0;
    int n = 40;
    int m = 40;

    DirihleVPuassone solver(a, b, c, d, n, m);

    solver.prepare_v_and_i_test();

    double eps = 1e-8;
    double error = 1e10;
    int iter = 0;
    int max_iter = 100000;

    std::cout << "Начало расчета ММН для сетки " << n << "x" << m << "..." << std::endl;

    while (error > eps && iter < max_iter) {
        error = solver.solve();

        if (iter % 500 == 0) {
            std::cout << "Итерация: " << iter << " | Текущая невязка: " << error << std::endl;
        }

        iter++;
    }

    std::cout << "\n--- Результаты ---" << std::endl;
    if (iter < max_iter) {
        std::cout << "Процесс сошелся за " << iter << " итераций." << std::endl;
    }
    else {
        std::cout << "Достигнуто максимальное число итераций!" << std::endl;
    }
    std::cout << "Финальная невязка: " << error << std::endl;



    solver.print_final_accuracy();

    return 0;
}