#include "parser.h"

double	my_pow(double multiplier, int exponent)
{
	double result;

	result = multiplier;
	while (--exponent)
		result *= multiplier;
	return (result);
}

double	my_sqrt(double number)
{
	double	precision = 1.0;
	double	result = 0.0;
	int		decimals = 9;

	if (number < 0.0)
		error_manager("Attempted to get the square root of a negative number.");
	while (decimals)
	{
		while (result * result <= number)
			result += precision;
		result -= precision;
		precision /= 10.0;
		decimals--;
	}


	return (result);
}
