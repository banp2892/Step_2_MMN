#pragma once
#include <vector>

class DirihleVPuassone {
	const double pi = 3.141592653589793;

public:
	double a = 0.0, b = 3.0;
	double c = 0.0, d = 1.0;

	int n = 0, m = 0;
	double h = 0.0, k = 0.0;
	double inv_h2 = 0.0, inv_k2 = 0.0;


	std::vector<double> v;
	std::vector<double> r;
	std::vector<double> Ar;
	std::vector<double> f_grid;
	
	void preparation(double a_temp, double b_temp, double c_temp, double d_temp, int n_temp, int m_temp);

	void calculate_f_grid_test();

	void prepare_v_and_i_test();

	double calculate_v_i_j(std::vector<double>& vhod, int i, int j); // заменяем 4 точечным шаблоном

	double scalar_mul(std::vector<double>& vector1, std::vector<double>& vector2);





	double Nu1_main(double y); // левая граница sin^2(pi*y)
	double Nu2_main(double y); // правая граница 0
	double Nu3_main(double x); // нижняя гранция ch(x^2-3x)-1
	double Nu4_main(double x); // верхняя граница 0

	double delta_u(double x, double y); // sin^2(x*y^2)
	double f_main(double x, double y); // ch(x-y)



	double f_test(double x, double y);

	double Nu1_test(double y);
	double Nu2_test(double y);
	double Nu3_test(double x);
	double Nu4_test(double x);
};


