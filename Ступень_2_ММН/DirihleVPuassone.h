#pragma once
#include <vector>

class DirihleVPuassone // вариант 8 
{

	const double pi = 3.141592653589793;

public:

	std::vector<std::vector<double>> v; // вектор v участвующий в подсчете текущей точки v ij
	std::vector<std::vector<double>> r; // невязка
	std::vector<std::vector<double>> Ar; // скалярное произведение невязки r на матрицу A
	std::vector<std::vector<double>> f_right; // правая часть для текущей строки?

	double calculate_v_i_j(std::vector<std::vector<double>>& vhod, int i, int j); // заменяем 4 точечным шаблоном

	double scalar_mul(std::vector<std::vector<double>> vector1, std::vector<std::vector<double>> vector2);







	double a, b; // границы x [0,3]
	double c, d; // границы y [0,1]

	int n, m = 0; // количество разбиений по осям x, y соответственно
	// узлов будет n+1, m+1
	double h, k = 0.0; // шаги по сеткам x и y соответственно

	double E_met = 0.0;
	int N_max = 0;
	










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


