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

	v.assign(total_nodes, 0.0); // вектор v заполняем нулями
	r.assign(total_nodes, 0.0); // вектор невязок r заполняем нулями
	Ar.assign(total_nodes, 0.0); // вектор произведение Ar заполняем нулями
	f_grid.assign(total_nodes, 0.0); // правая часть f_grid заполняем нулями
	u.assign(total_nodes, 0.0);
	v2.assign(total_nodes, 0.0);
	diff_v_and_u.assign(total_nodes, 0.0);
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


void DirihleVPuassone::choosing_approximation(StartApproximation type) {
    int cols = n + 1;
    int rows = m + 1;

    switch (type) {
        case AVERAGE: {
			std::cout << "Выбрано AVERAGE" << std::endl;
            double sum = 0.0;
            for (int i = 0; i <= n; ++i) {
                sum += v[0 * cols + i];
                sum += v[m * cols + i];
            }
            for (int j = 1; j < m; ++j) {
                sum += v[j * cols + 0];
                sum += v[j * cols + n];
            }
            
            double avg = sum / (2.0 * (n + 1) + 2.0 * (m - 1));

            for (int j = 1; j < m; ++j)
                for (int i = 1; i < n; ++i)
                    v[j * cols + i] = avg;
            break;
        }

        case INTERP_X:
			std::cout << "Выбрано INTERP_X" << std::endl;
            for (int j = 1; j < m; ++j) {
                double left = v[j * cols + 0];
                double right = v[j * cols + n];
                for (int i = 1; i < n; ++i) {
                    v[j * cols + i] = left + (right - left) * (double)i / n;
                }
            }
            break;

        case INTERP_Y:
			std::cout << "Выбрано INTERP_Y" << std::endl;
            for (int i = 1; i < n; ++i) {
                double bottom = v[0 * cols + i];
                double top = v[m * cols + i];
                for (int j = 1; j < m; ++j) {
                    v[j * cols + i] = bottom + (top - bottom) * (double)j / m;
                }
            }
            break;

        case ZERO:
			std::cout << "Выбрано ZERO" << std::endl;
        default:
            for (int j = 1; j < m; ++j)
                for (int i = 1; i < n; ++i)
                    v[j * cols + i] = 0.0;
            break;
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

void DirihleVPuassone::calculate_delta_u()
{
	const int row_step = n + 1;
	double* __restrict u_ptr = u.data();

#pragma omp parallel for
	for (int j = 0; j <= m; j++) {
		const int row_offset = j * row_step;
		const double y = c + k * j;
		for (int i = 0; i <= n; i++) {
			const double x = a + h * i;

			u_ptr[row_offset + i] = delta_u(x, y);
		}
	}
}

std::vector<double> DirihleVPuassone::calculate_vec_diff(const std::vector<double>& v1, const std::vector<double>& v2)
{
	
	if (v1.size() != v2.size()) {
		
		std::cout << "Ошибка, размеры векторов при сравнении разные: v1.size() = " << v1.size() << ", v2.size() = " << v2.size() << std::endl;
		return std::vector<double>(0);
	}

	std::vector<double> result(v1.size());

	const int row_step = n + 1;


	for (int j = 0; j <= m; j++) {
		for (int i = 0; i <= n; i++) {
			int idx = row_step * j + i;
			result[idx] = v1[idx] - v2[idx];
		}
	}

	return result;
}

std::vector<double> DirihleVPuassone::get_subsampled_v2(const std::vector<double>& v_high, int n_low, int m_low)
{

	std::vector<double> result((n_low + 1) * (m_low + 1));

	int n_high = 2 * n_low;
	int row_step_high = n_high + 1;
	int row_step_low = n_low + 1;

	for (int j = 0; j <= m_low; j++) {
		for (int i = 0; i <= n_low; i++) {

			int idx_low = j * row_step_low + i;


			int idx_high = (2 * j) * row_step_high + (2 * i);

			result[idx_low] = v_high[idx_high];
		}
	}

	return result;
}



std::vector<double> DirihleVPuassone::reshape_to_half_nodes(const std::vector<double>& vector, int n, int m)
{
	std::vector<double> result((n + 1) * (m + 1));

	int oldW = 2 * m + 1;
	std::cout << "Vector size = " << vector.size() << " target n, m: " << n << " " << m << std::endl;

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= m; j++) {
			size_t old_idx = static_cast<size_t>(2 * i) * oldW + (2 * j);
			size_t new_idx = static_cast<size_t>(i) * (m + 1) + j;

			if (old_idx < vector.size() && new_idx < result.size()) {
				result[new_idx] = vector[old_idx];
			}
			else {
				std::cerr << "Out of bounds at i=" << i << " j=" << j << std::endl;
			}
		}
	}
	return result;
}
void DirihleVPuassone::solver_iterator(DirihleVPuassone& solver, double eps_limit, int n_max) {
	double current_residual = 1e10; // Невязка ||R||
	double current_delta_v = 1e10;  // Приращение ||v_new - v_old||
	int current_iter = 0;
	const int total = (solver.n + 1) * (solver.m + 1);

	double g_ar_r = 0, g_ar_ar = 0;
	double l_ar_r = 0, l_ar_ar = 0;
	double l_max_r = 0;
	double l_max_delta = 0;

#pragma omp parallel shared(current_residual, current_delta_v, current_iter) firstprivate(eps_limit, n_max)
	{
		// ВАЖНО: Условие проверяется всеми, но обновляется внутри barrier
		while (current_delta_v > eps_limit && current_iter < n_max) {

			solver.calculate_r();
			solver.calculate_Ar();

#pragma omp single
			{
				l_ar_r = 0.0;
				l_ar_ar = 0.0;
			}

			const double* ar_p = solver.Ar.data();
			const double* r_p = solver.r.data();

#pragma omp for reduction(+:l_ar_r, l_ar_ar)
			for (int i = 0; i < total; i++) {
				l_ar_r += ar_p[i] * r_p[i];
				l_ar_ar += ar_p[i] * ar_p[i];
			}

			// Вычисляем шаг tau
			double tau_local = l_ar_r / (l_ar_ar + 1e-25);
			double* v_p = solver.v.data();

#pragma omp single
			{
				l_max_r = 0.0;
				l_max_delta = 0.0;
			}

			// Обновляем решение И считаем сразу две нормы
#pragma omp for reduction(max:l_max_r, l_max_delta)
			for (int i = 0; i < total; i++) {
				double delta = tau_local * r_p[i]; // На сколько изменилось решение
				v_p[i] -= delta;

				double abs_r = std::fabs(r_p[i]);
				double abs_d = std::fabs(delta);

				if (abs_r > l_max_r) l_max_r = abs_r;
				if (abs_d > l_max_delta) l_max_delta = abs_d;
			}

#pragma omp single
			{
				current_residual = l_max_r;
				current_delta_v = l_max_delta;
				current_iter++;

				if (current_iter % 5000 == 0) {
					std::cout << "Iter: " << current_iter
						<< " | Max Delta: " << std::scientific << current_delta_v << std::endl;
				}
			}
			
		}
	}

	solver.last_iterations = current_iter;
	solver.r_n = current_residual; // Сохраняем невязку
	solver.final_eps = current_delta_v; // Сохраняем ПРИРАЩЕНИЕ (точность метода)

	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "FINISH: " << current_iter << " iterations." << std::endl;
	std::cout << "Final r_n: " << current_residual << std::endl;
	std::cout << "Final eps: " << current_delta_v << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
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


 double DirihleVPuassone::get_test_error(double& max_x, double& max_y) {
	 double max_diff = 0.0;
	 max_x = a;
	 max_y = c;

	 for (int j = 0; j <= m; j++) {
		 double y = c + j * k;
		 for (int i = 0; i <= n; i++) {
			 double x = a + i * h;
			 int idx = j * (n + 1) + i;

			 // Используем аналитическое решение u*(x,y)
			 double u_exact = delta_u(x, y);
			 double current_diff = std::abs(u_exact - v[idx]);

			 if (current_diff > max_diff) {
				 max_diff = current_diff;
				 max_x = x;
				 max_y = y;
			 }
		 }
	 }
	 return max_diff;
 }


 double DirihleVPuassone::compare_with_half_step(const DirihleVPuassone& solver_high, double& max_x, double& max_y) {
	 double max_diff = 0.0;
	 max_x = a;
	 max_y = c;

	 // solver_high должна иметь n_h = 2*n, m_h = 2*m
	 for (int j = 0; j <= m; j++) {
		 double y = c + j * k;
		 for (int i = 0; i <= n; i++) {
			 double x = a + i * h;

			 int idx_low = j * (n + 1) + i;
			 int idx_high = (2 * j) * (solver_high.n + 1) + (2 * i);

			 double diff = std::abs(v[idx_low] - solver_high.v[idx_high]);

			 if (diff > max_diff) {
				 max_diff = diff;
				 max_x = x;
				 max_y = y;
			 }
		 }
	 }
	 return max_diff;
 }

 double DirihleVPuassone::get_chebyshov_norma_for_vector(const std::vector<double> &v1) {

	 double max_ret = 0.0;
#pragma omp parallel for reduction(max:max_ret)
	 for (int i = 0; i < v1.size(); i++) {
		 double val = std::abs(v1[i]);
		 if (val > max_ret) {
			 max_ret = val;
		 }
	 }
	 return max_ret;
 }

 double DirihleVPuassone::get_evklid_norma_for_vector(const std::vector<double>& v1) {

	 double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
	 for (int i = 0; i < (int)v1.size(); ++i) {
		 sum += v1[i] * v1[i];
	 }
	 return std::sqrt(sum);
 }