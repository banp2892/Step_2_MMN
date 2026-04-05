#include "MMN.h"


double MMN::calculate_v_i_j(std::vector<double>& vhod, int i, int j)
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

double MMN::scalar_mul(std::vector<double>& vector1, std::vector<double>& vector2)
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

void MMN::solve() { // считаем r, Ar, tao


}


void MMN::calculate_Av() {



}

void MMN::calculate_r(std::vector<double> &r) {

	for (int j = 1; j < m; j++) {
		for (int i = 1; i < n; i++) {
			int idx = j * (n + 1) + i;
			calculate_v_i_j(r,)
		}
	}


}