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
    int task_type = std::stoi(argv[9]); // 0 - тест, 1 - основная

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


        std::ofstream stats("stats.txt");
        if (stats.is_open()) {
            stats << duration.count() << "\n";
            stats << solver.last_iterations << "\n";
            stats << solver.final_eps << "\n";
            stats << n << "\n";
            stats << m << "\n";
            stats.close();
        }


        std::cout << "SUCCESS" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}