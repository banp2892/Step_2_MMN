#pragma once
#include <vector>
enum StartApproximation { ZERO, AVERAGE, INTERP_X, INTERP_Y };


class DirihleVPuassone {
	const double pi = 3.141592653589793;

public:

	DirihleVPuassone(double a_t, double b_t, double c_t, double d_t, int n_t, int m_t);

	double a = 0.0, b = 3.0;
	double c = 0.0, d = 1.0;

	int n = 0, m = 0;
	double h = 0.0, k = 0.0;
	double inv_h2 = 0.0, inv_k2 = 0.0;
	double tau = 0.0;
	int last_iterations = 0; // количество итераций
	double final_eps = 0.0; // достигнутая точность
	double r_0 = 0.0; // начальная невязка
	double r_n = 0.0; // невязка в конце


	double error = 1e10; // начальная ошибка
	int iter = 0; // начальное число итераций

	std::vector<double> v;
	std::vector<double> r;
	std::vector<double> Ar;
	std::vector<double> f_grid;


	std::vector<double> u; // точное решение тестовой задачи
	std::vector<double> v2; // удвоенная сетка для основной задачи
	std::vector<double> diff_v_and_u;
	
	

	void calculate_f_grid_test();

	void prepare_v_and_i_test();


	double Nu1_main(double y); // левая граница sin^2(pi*y)
	double Nu2_main(double y); // правая граница 0
	double Nu3_main(double x); // нижняя гранция ch(x^2-3x)-1
	double Nu4_main(double x); // верхняя граница 0

	double delta_u(double x, double y); // sin^2(x*y^2)
	double f_main(double x, double y); // ch(x-y)
	void calculate_f_grid_main();

	void prepare_v_and_i_main();

	static double compare(const DirihleVPuassone& solver_low, const DirihleVPuassone& solver_high);

	double get_test_error(double& max_x, double& max_y);

	double compare_with_half_step(const DirihleVPuassone& solver_high, double& max_x, double& max_y);

	double get_chebyshov_norma_for_vector(const std::vector<double>& v1); // считаем норму невязки по чебышеву на текщем шаге

	double get_evklid_norma_for_vector(const std::vector<double>& v1);


	double f_test(double x, double y);

	double Nu1_test(double y);
	double Nu2_test(double y);
	double Nu3_test(double x);
	double Nu4_test(double x);



	void choosing_approximation(StartApproximation type); ///> выбираем начальное приближение


	// MMN 



	void calculate_Ar();
	void calculate_Ar_TBB();
	void calculate_r();

	double calculate_epsilon1();
	void calculate_delta_u();
	std::vector<double> calculate_vec_diff(const std::vector<double>& v1, const std::vector<double>& v2);

	std::vector<double> get_subsampled_v2(const std::vector<double>& v_high, int n_low, int m_low);


	std::vector<double> reshape_to_half_nodes(const std::vector<double>& vector, int n, int m);

	static void solver_iterator(DirihleVPuassone &solver, double Err, int N_max);
};


