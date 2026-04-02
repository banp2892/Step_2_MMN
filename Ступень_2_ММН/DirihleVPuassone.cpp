#include "DirihleVPuassone.h"
#include <math.h>
#include <iostream>

void DirihleVPuassone::preparation(double a_temp, double b_temp, double c_temp, double d_temp, int n_temp, int m_temp)
{

	a = a_temp;
	b = b_temp;
	c = c_temp;
	d = d_temp;
	n = n_temp;
	m = m_temp;
	h = (b_temp - a_temp) / n;
	k = (d_temp - c_temp) / m;

	inv_h2 = 1.0 / (h * h);
	inv_k2 = 1.0 / (k * k);


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

