#pragma once
#include <vector>

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
	int last_iterations = 0;
	double final_eps = 0.0;

	double error = 1e10;
	int iter = 0;

	std::vector<double> v;
	std::vector<double> r;
	std::vector<double> Ar;
	std::vector<double> f_grid;
	
	

	void calculate_f_grid_test();

	void prepare_v_and_i_test();


	





	double Nu1_main(double y); // лева€ граница sin^2(pi*y)
	double Nu2_main(double y); // права€ граница 0
	double Nu3_main(double x); // нижн€€ гранци€ ch(x^2-3x)-1
	double Nu4_main(double x); // верхн€€ граница 0

	double delta_u(double x, double y); // sin^2(x*y^2)
	double f_main(double x, double y); // ch(x-y)
	void calculate_f_grid_main();

	void prepare_v_and_i_main();

	static double compare(const DirihleVPuassone& solver_low, const DirihleVPuassone& solver_high);


	double f_test(double x, double y);

	double Nu1_test(double y);
	double Nu2_test(double y);
	double Nu3_test(double x);
	double Nu4_test(double x);



	// MMN 
	double calculate_v_i_j(std::vector<double>& vhod, int i, int j);
	double scalar_mul(std::vector<double>& vector1, std::vector<double>& vector2);
	double solve();
	void calculate_Ar();
	void calculate_r();
	double calculate_epsilon1();



	static void solver_iterator(DirihleVPuassone &solver, double Err, int N_max);
};


