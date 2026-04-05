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

double DirihleVPuassone::calculate_v_i_j(std::vector<double>& vhod, int i, int j)
{

	int center_idx = j * (n + 1) + i;

	double v_left = vhod[j * (n + 1) + (i - 1)];
	double v_right = vhod[j * (n + 1) + (i + 1)];
	double v_down = vhod[(j - 1) * (n + 1) + i];
	double v_up = vhod[(j + 1) * (n + 1) + i];
	double v_curr = vhod[center_idx];

	double res = (2.0 * inv_h2 + 2.0 * inv_k2) * v_curr - inv_h2 * (v_left + v_right) - inv_k2 * (v_up + v_down);

	return res;
}

double DirihleVPuassone::scalar_mul(std::vector<double>& vector1, std::vector<double>& vector2)
{

	double sum = 0.0;
	for (int j = 1; j < m; j++) {
		for (int i = 1; i < n; i++) {
			int id = j * (n + 1) + i;
			sum += vector2[id] * vector1[id];
		}
	}


	return sum;
}

double DirihleVPuassone::solve() {

	calculate_r();

	calculate_Ar();

	double ar_r = scalar_mul(Ar, r);
	double ar_ar = scalar_mul(Ar, Ar);

	if (std::abs(ar_ar) < 1e-16) return 0.0; // проверка деления на ноль

	double tao = ar_r / ar_ar;

	double sum_r2 = 0.0;
	for (int j = 1; j < m; j++) {
		for (int i = 1; i < n; i++) {
			int idx = j * (n + 1) + i;

			v[idx] = v[idx] - tao * r[idx];

			sum_r2 += r[idx] * r[idx];
		}
	}

	return sqrt(sum_r2);
}


void DirihleVPuassone::calculate_Ar() {
#pragma omp parallel
	for (int j = 1; j < m; j++) {
		for (int i = 1; i < n; i++) {
			int idx = j * (n + 1) + i;
			Ar[idx] = calculate_v_i_j(r, i, j);
		}
	}


}

void DirihleVPuassone::calculate_r() {
#pragma omp parallel
	for (int j = 1; j < m; j++) {
		for (int i = 1; i < n; i++) {
			int idx = j * (n + 1) + i;
			r[idx] = calculate_v_i_j(v, i, j) - f_grid[idx];
		}
	}


}

