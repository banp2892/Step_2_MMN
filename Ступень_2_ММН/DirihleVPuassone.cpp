#include "DirihleVPuassone.h"
#include <math.h>
#include <iostream>

double DirihleVPuassone::calculate_v_i_j(std::vector<std::vector<double>>& vhod, int i, int j)
{
	if ((i - 1 < 0) || (i + 1 > n ) || (j - 1) < 0 || (j + 1) > m ) {
		std::cout << "Failed the bordes to calculate v, index: " << i << ", " << j << std::endl;
		return 0.0;
	}

	double v_i_j = 

	return 0.0;
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

