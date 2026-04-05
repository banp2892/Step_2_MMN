




#include <iostream>
#include "DirihleVPuassone.h"

int main() {
	setlocale(LC_ALL, "Russian");
	int n, m;
	double a, b, c, d;


	std::cout << "Введите n, m, a, b, c, d:" << std::endl;

	std::cin >> n >> m >> a >> b >> c >> d;


	DirihleVPuassone task(a, b, c, d, n, m);
	task.prepare_v_and_i_test();

	double err = 1.0;

	while (err > 1e-7) {
		err = task.solve(); // Делаем итерацию ММН
	}

	return 0;



}