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

    if ((argc != 10) || (argc != 12)) {
        std::cerr << "Использование: " << argv[0]
            << " a b c d n m e_max n_max task_type (e_max_2) (n_max_2)" << std::endl;
        return 1;
    }

    double a = std::stod(argv[1]);
    double b = std::stod(argv[2]);
    double c = std::stod(argv[3]);
    double d = std::stod(argv[4]);
    int n = std::stoi(argv[5]);
    int m = std::stoi(argv[6]);
    double e_max = std::stod(argv[7]); // критерий остановки по точности 
    int n_max = std::stoi(argv[8]); // критерий остановки по числу итераций
    int task_type = std::stoi(argv[9]); // 0 - тестовая, 1 - основная
    double e_max_2 = std::stod(argv[10]); // второй потолок по точности для удвоенной сетки
    int n_max_2 = std::stoi(argv[11]); // по шагам для второй сетки


    int total_n = 0; // количество затраченный итераций
    double total_e = 0.0; // достигнутая точность итерационного метода 
    double r_n = 0.0; // значение невязки

    std::vector<double> vector_diff((n+1)*(m+1));

    //для тестовой задачи
    double E1 = 0.0; // задача решена с погрешностью E1
    double x, y = 0.0; // Максимальное отклонение точного и численного решений наблюдается в узле
    double test_r_0 = 0.0; // Невязка СЛАУ на начальном приближении || R(0) || = 
    double max_dif = 0.0; // Максимальное отклонение точного и численного решений
    

    // для основной
    double main_r_0 = 0.0; //   На основной сетке невязка СЛАУ на начальном приближении || R(0) || = «___» (указать норму невязки и тип нормы)
    double e_max_2 = 0.0; // критерий остановки по точности для удвоенной сетки
    double n_max_2 = 0.0; // критерий остановки по числу итераций для удвоенной сетки
    int total_n_2 = 0; // На решение задачи (СЛАУ) затрачено итераций N2 =«__» 
    double total_e_n_2 = 0.0; // достигнута точ- ность итерационного метода ε(N2) = «__»
    double r_n_2 = 0.0;
    double E2 = 0.0;
    double main_r_0_2 = 0.0;

    DirihleVPuassone First_object(a, b, c, d, n, m); // создаем объект класса

    if (task_type == 0) {
        First_object.prepare_v_and_i_test();
        test_r_0 = First_object.get_chebyshov_norma_for_vector(First_object.r);

        DirihleVPuassone::solver_iterator(First_object, e_max, n_max); // считаем все что надо
        total_n = First_object.last_iterations;
        total_e = First_object.final_eps;
        First_object.calculate_delta_u(); // считаем точную матрицу по заданной функции delta_u

        vector_diff = First_object.calculate_vec_diff(First_object.u, First_object.v); // считаем вектор u - v
        r_n = First_object.get_chebyshov_norma_for_vector(First_object.r);
        E1 = First_object.get_chebyshov_norma_for_vector(vector_diff);

        max_dif = First_object.get_test_error(x, y);
    }
    else {
        First_object.prepare_v_and_i_main();

        DirihleVPuassone Second_object(a, n, c, d, n*2, m*2); // удвоенная сетка 

        main_r_0 = First_object.get_chebyshov_norma_for_vector(First_object.r);

        DirihleVPuassone::solver_iterator(First_object, e_max, n_max); // считаем все что надо
        total_n = First_object.last_iterations;
        total_e = First_object.final_eps;


        DirihleVPuassone::solver_iterator(Second_object, e_max_2, n_max_2); // считаем все что надо

        Second_object.v2 = Second_object.get_subsampled_v2(Second_object.v, n, m);

        vector_diff = First_object.calculate_vec_diff(First_object.v, Second_object.v2);

        total_n_2 = Second_object.last_iterations;
        total_e_n_2 = Second_object.final_eps;

        r_n_2 = Second_object.get_chebyshov_norma_for_vector(Second_object.v);

        
        E2 = Second_object.get_chebyshov_norma_for_vector(vector_diff);
        max_dif = First_object.compare_with_half_step(Second_object, x, y);
    }


    // записываем в файл результаты
    std::ofstream stats("stats.txt", std::ios::app);
    if (stats.is_open()) {
        stats << n << " " 
              << m << " ";
        stats << e_max << " " 
              << n_max << " ";
        stats << total_n << " "
              << total_e << " "
              << r_n << " "
            ;


        if (task_type == 0) { // тестовая задача 
            stats << E1 << " ";
            stats << x << " " << y << " ";
            stats << test_r_0 << " ";

        }
        else {
            stats << main_r_0 << " ";
            stats << e_max_2 << " ";
            stats << n_max_2 << " ";
            stats << total_n_2 << " "
                << total_e_n_2 << " "
                << r_2n << " "
                ;

        }


        stats.close();
    }
    return 0;
}