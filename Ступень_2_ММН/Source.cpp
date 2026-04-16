#include <iostream>
#include <iomanip>
#include <clocale>
#include "DirihleVPuassone.h"
#include <chrono>
#include <functional>
#include <string>
#include <fstream>
#include <vector>

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
        std::cerr << "Недостаточно аргументов!" << std::endl;
        return 1;
    }

    int task_type = std::stoi(argv[9]);
    if (task_type == 1 && argc < 12) {
        std::cerr << "Для основной задачи необходимо указать e_max_2 и n_max_2!" << std::endl;
        return 1;
    }

    double a = std::stod(argv[1]), b = std::stod(argv[2]);
    double c = std::stod(argv[3]), d = std::stod(argv[4]);
    int n = std::stoi(argv[5]), m = std::stoi(argv[6]);
    double e_max = std::stod(argv[7]);
    int n_max = std::stoi(argv[8]);
    double e_max_2 = (argc >= 12) ? std::stod(argv[10]) : e_max;
    int n_max_2 = (argc >= 12) ? std::stoi(argv[11]) : n_max;

    int total_n = 0, total_n_2 = 0;
    double total_e = 0.0, r_n = 0.0, r_0_initial = 0.0;
    double total_e_2 = 0.0, r_n_2 = 0.0, r_0_2 = 0.0;
    double E_error = 0.0, x_max = 0.0, y_max = 0.0;
    double time1 = 0.0, time2 = 0.0;

    DirihleVPuassone solver1(a, b, c, d, n, m);

    if (task_type == 0) {

        solver1.prepare_v_and_i_test();
        solver1.calculate_r();
        r_0_initial = solver1.get_chebyshov_norma_for_vector(solver1.r);

        save_to_binary("v0_test_numeric.bin", solver1.v);

        auto start = std::chrono::high_resolution_clock::now();
        DirihleVPuassone::solver_iterator(solver1, e_max, n_max);
        auto end = std::chrono::high_resolution_clock::now();
        time1 = std::chrono::duration<double>(end - start).count();

        total_n = solver1.last_iterations;
        total_e = solver1.final_eps;
        r_n = solver1.r_n;

        solver1.calculate_delta_u();
        std::vector<double> diff_utest = solver1.calculate_vec_diff(solver1.u, solver1.v);

        save_to_binary("v_test_numeric.bin", solver1.v);
        save_to_binary("u_test_exact.bin", solver1.u);
        save_to_binary("uv_test_diff.bin", diff_utest);

        E_error = solver1.get_test_error(x_max, y_max);
        std::cout << "Время расчета: " << time1 << " сек." << std::endl;
    }
    else {

        solver1.prepare_v_and_i_main();
        solver1.calculate_r();
        r_0_initial = solver1.get_chebyshov_norma_for_vector(solver1.r);

        save_to_binary("v1_0_main_numeric.bin", solver1.v);

        auto start1 = std::chrono::high_resolution_clock::now();
        DirihleVPuassone::solver_iterator(solver1, e_max, n_max);
        auto end1 = std::chrono::high_resolution_clock::now();
        time1 = std::chrono::duration<double>(end1 - start1).count();

        total_n = solver1.last_iterations;
        total_e = solver1.final_eps;
        r_n = solver1.r_n;


        DirihleVPuassone solver2(a, b, c, d, n * 2, m * 2);
        solver2.prepare_v_and_i_main();
        solver2.calculate_r();
        r_0_2 = solver2.get_chebyshov_norma_for_vector(solver2.r);

        std::vector<double> temp_vector_v2 = solver2.reshape_to_half_nodes(solver2.v, n, m);

        save_to_binary("v2_0_main_numeric.bin", temp_vector_v2);

        auto start2 = std::chrono::high_resolution_clock::now();
        DirihleVPuassone::solver_iterator(solver2, e_max_2, n_max_2);
        auto end2 = std::chrono::high_resolution_clock::now();
        time2 = std::chrono::duration<double>(end2 - start2).count();

        total_n_2 = solver2.last_iterations;
        total_e_2 = solver2.final_eps;
        r_n_2 = solver2.r_n;

        E_error = solver1.compare_with_half_step(solver2, x_max, y_max);

        save_to_binary("v_main_n.bin", solver1.v);
        std::vector<double> v2_subsampled = solver1.get_subsampled_v2(solver2.v, n, m);
        save_to_binary("v_main_2n_sub.bin", v2_subsampled);
        std::vector<double> diff = solver1.calculate_vec_diff(solver1.v, v2_subsampled);
        save_to_binary("v_main_diff.bin", diff);

        
        std::cout << "Время 1 (n x m): " << time1 << " сек. " << "Время 2 (2n x 2m): " << time2 << " сек." << std::endl;
    }

    std::ofstream stats("stats.txt", std::ios::app);
    if (stats.is_open()) {
        stats << task_type << " " << n << " " << m << " "
            << total_n << " " << total_e << " " << r_n << " " << r_0_initial << " ";

        if (task_type == 0) {
            stats << E_error << " " << x_max << " " << y_max << " " << time1;
        }
        else {
            stats << total_n_2 << " " << total_e_2 << " " << r_n_2 << " " << r_0_2 << " "
                << E_error << " " << x_max << " " << y_max << " " << time1 << " " << time2;
        }
        stats << std::endl;
        stats.close();
    }

    return 0;
}