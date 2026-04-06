#include "DirihleVPuassone.h"
#include <math.h>
#include <iostream>


DirihleVPuassone::DirihleVPuassone(double a_t, double b_t, double c_t, double d_t, int n_t, int m_t)
	: a(a_t), b(b_t), c(c_t), d(d_t), n(n_t), m(m_t)
{
	h = (b - a) / n;
	k = (d - c) / m;


	inv_h2 = 1.0 / (h * h);
	inv_k2 = 1.0 / (k * k);

	int total_nodes = (n + 1) * (m + 1);

	v.assign(total_nodes, 0.0);
	r.assign(total_nodes, 0.0);
	Ar.assign(total_nodes, 0.0);
	f_grid.assign(total_nodes, 0.0);
}



void DirihleVPuassone::calculate_f_grid_test()
{
	for (int j = 0; j < m+1; j++) {
		double y = c + j * k;
		for (int i = 0; i < n+1; i++) {
			double x = a + i * h;
			f_grid[j * (n+1) + i] = f_test(x, y);
		}
	}

}

void DirihleVPuassone::prepare_v_and_i_test()
{


	calculate_f_grid_test();

	for (int j = 0; j <= m; j++) {
		double y = c + j * k;
		for (int i = 0; i <= n; i++) {
			double x = a + i * h;
			int idx = j * (n + 1) + i;

			if (i == 0)      v[idx] = Nu1_test(y);
			else if (i == n) v[idx] = Nu2_test(y);
			else if (j == 0) v[idx] = Nu3_test(x);
			else if (j == m) v[idx] = Nu4_test(x);
			else {
				v[idx] = 0.0;
			}
		}
	}
}





double DirihleVPuassone::Nu1_main(double y)
{

	double s = sin(pi * y);
	return s * s;


}

double DirihleVPuassone::Nu2_main(double y)
{
	return 0.0;
}

double DirihleVPuassone::Nu3_main(double x)
{
	return (cosh(x*x-3.0*x)-1.0);
}

double DirihleVPuassone::Nu4_main(double x)
{
	return 0.0;
}

double DirihleVPuassone::delta_u(double x, double y)
{
	return sin(x * y * y) * sin(x * y * y);
}

double DirihleVPuassone::f_main(double x, double y)
{

	return cosh(x-y);
}

double DirihleVPuassone::f_test(double x, double y)
{

	return (( - 2.0) * ((y * y * y * y + 4.0 * x * x * y * y) * cos(2.0 * x * y * y) + x * sin(2.0 * x * y * y)));

}

double DirihleVPuassone::Nu1_test(double y)
{
	return 0.0;
}


double DirihleVPuassone::Nu2_test(double y) {
	double s = sin(3.0 * y * y);
	return s * s;
}

double DirihleVPuassone::Nu3_test(double x)
{
	return 0.0;
}

double DirihleVPuassone::Nu4_test(double x)
{
	double s = sin(x);
	return s * s;
}








void DirihleVPuassone::calculate_Ar() {
	const int row_step = n + 1;
	const double c_coeff = 2.0 * (inv_h2 + inv_k2);
	double* __restrict ar_ptr = Ar.data();
	const double* __restrict r_ptr = r.data();
#pragma omp for
	for (int j = 1; j < m; j++) {
		int row = j * row_step;
		for (int i = 1; i < n; i++) {
			ar_ptr[row + i] = c_coeff * r_ptr[row + i]
				- inv_h2 * (r_ptr[row + i - 1] + r_ptr[row + i + 1])
				- inv_k2 * (r_ptr[row - row_step + i] + r_ptr[row + row_step + i]);
		}
	}
}

void DirihleVPuassone::calculate_r() {
	const int row_step = n + 1;
	double* __restrict r_ptr = r.data();
	const double* __restrict v_ptr = v.data();
	const double* __restrict f_ptr = f_grid.data();
#pragma omp for
	for (int j = 1; j < m; j++) {
		const int row = j * row_step;
		for (int i = 1; i < n; i++) {
			double Lapl = (2.0 * inv_h2 + 2.0 * inv_k2) * v_ptr[row + i]
				- inv_h2 * (v_ptr[row + i - 1] + v_ptr[row + i + 1])
				- inv_k2 * (v_ptr[row - row_step + i] + v_ptr[row + row_step + i]);
			r_ptr[row + i] = Lapl - f_ptr[row + i];
		}
	}
}





double DirihleVPuassone::calculate_epsilon1() {
    double max_diff = 0.0;

    for (int j = 0; j <= m; j++) {
        double y = c + j * k;
        for (int i = 0; i <= n; i++) {
            double x = a + i * h;
            int idx = j * (n + 1) + i;

			double u_exact = delta_u(x, y);;
            
            double current_diff = std::abs(u_exact - v[idx]);

            if (current_diff > max_diff) {
                max_diff = current_diff;
            }
        }
    }
    return max_diff;
}

void DirihleVPuassone::solver_iterator(DirihleVPuassone& solver, double eps_limit, int n_max) {
	double current_error = 1e10;
	int current_iter = 0;

	double global_ar_r = 0, global_ar_ar = 0;
	double local_max_r = 0;
	double local_ar_r = 0, local_ar_ar = 0;

#pragma omp parallel shared(current_error, current_iter, global_ar_r, global_ar_ar) firstprivate(eps_limit, n_max)
	{
		while (current_error > eps_limit && current_iter < n_max) {

			solver.calculate_r();
#pragma omp barrier 

			solver.calculate_Ar();
#pragma omp barrier 

			local_ar_r = 0;
			local_ar_ar = 0;
			const double* ar_p = solver.Ar.data();
			const double* r_p = solver.r.data();
			int total = (solver.n + 1) * (solver.m + 1);

#pragma omp for reduction(+:local_ar_r, local_ar_ar)
			for (int i = 0; i < total; i++) {
				local_ar_r += ar_p[i] * r_p[i];
				local_ar_ar += ar_p[i] * ar_p[i];
			}

#pragma omp master
			{
				global_ar_r = local_ar_r;
				global_ar_ar = local_ar_ar;
			}
#pragma omp barrier

			double tao = global_ar_r / (global_ar_ar + 1e-20);
			local_max_r = 0;
			double* v_p = solver.v.data();

#pragma omp for reduction(max:local_max_r)
			for (int i = 0; i < total; i++) {
				v_p[i] -= tao * r_p[i];
				double a_r = std::fabs(r_p[i]);
				if (a_r > local_max_r) local_max_r = a_r;
			}

#pragma omp master
			{
				current_error = local_max_r;
				current_iter++;
			}
#pragma omp barrier 
		}
	}

	solver.last_iterations = current_iter;
	solver.final_eps = current_error;
	std::cout << "FINISH: " << current_iter << " iterations. Error: " << current_error << std::endl;
}


void DirihleVPuassone::calculate_f_grid_main()
{
	for (int j = 0; j <= m; j++) {
		double y = c + j * k;
		for (int i = 0; i <= n; i++) {
			double x = a + i * h;
			f_grid[j * (n + 1) + i] = f_main(x, y);
		}
	}
}

void DirihleVPuassone::prepare_v_and_i_main()
{
	calculate_f_grid_main();

	for (int j = 0; j <= m; j++) {
		double y = c + j * k;
		for (int i = 0; i <= n; i++) {
			double x = a + i * h;
			int idx = j * (n + 1) + i;

			if (i == 0)      v[idx] = Nu1_main(y);
			else if (i == n) v[idx] = Nu2_main(y);
			else if (j == 0) v[idx] = Nu3_main(x);
			else if (j == m) v[idx] = Nu4_main(x);
			else {
				v[idx] = 0.0;
			}
		}
	}
}


 double DirihleVPuassone::compare(const DirihleVPuassone& solver_low, const DirihleVPuassone& solver_high) {
	double max_diff = 0.0;

	int n_l = solver_low.n;
	int m_l = solver_low.m;
	int n_h = solver_high.n;
	int m_h = solver_high.m;

	if (n_h != 2 * n_l || m_h != 2 * m_l) {
		std::cerr << "Ошибка: сетки не кратны 2! Сравнение невозможно." << std::endl;
		return -1.0;
	}

	const std::vector<double>& v_low = solver_low.v;
	const std::vector<double>& v_high = solver_high.v;

	for (int j = 0; j <= m_l; j++) {
		for (int i = 0; i <= n_l; i++) {

			int idx_low = j * (n_l + 1) + i;

			int idx_high = (2 * j) * (n_h + 1) + (2 * i);

			double diff = std::abs(v_low[idx_low] - v_high[idx_high]);

			if (diff > max_diff) {
				max_diff = diff;
			}
		}
	}

	return max_diff;
}

