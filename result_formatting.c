#include "parser.h"

static void	second_degree_result(double *params, char variablechar)
{
	printf("%g%c^2 + %g%c + %g = 0\n\n", params[2], variablechar, params[1], variablechar, params[0]);
	
	int	solutions = get_second_degree_solutions(params);

	printf("> Applied QUADRATIC FORMULA\n\n");
	if (solutions == -1)
		printf("\033[1m--- Equation results in imaginary numbers ---\033[0m\n");
	else if (solutions == 1)
	{
		printf("> Solution:\n");
		printf("\033[1m--- %c = %f ---\033[0m\n", variablechar, get_second_degree_first_solution(params));
	}
	else
	{
		printf("> Solutions:\n");
		printf("\033[1m--- %c(1) = %f ---\033[0m\n", variablechar, get_second_degree_first_solution(params));
		printf("\033[1m--- %c(2) = %f ---\033[0m\n", variablechar, get_second_degree_second_solution(params));
	}

}

static void	first_degree_result(double *params, char variablechar)
{
	printf("%g%c + %g = 0  -->  ", params[1], variablechar, params[0]);
	printf("%g%c = %g \n\n", params[1], variablechar, -1.0 * params[0]);
	if (params[1] != 1.0)
		printf("> Divided both sides by %g.\n\n", params[1]);
	printf("> Solution:\n");
	printf("\033[1m--- %c = %g ---\033[0m\n", variablechar, -1 * params[0] / params[1]);
}

static void	zero_degree_result(double *params)
{
	printf("%g = 0\n\n", params[0]);
	printf("> Solution:\n");
	if (params[0] == 0.0)
		printf("\033[1m--- The equation is \033[1;32mTRUE\033[0m\033[1m ---\033[0m\n");
	else
		printf("\033[1m--- The equation is \033[1;31mFALSE\033[0m\033[1m ---\033[0m\n");
}



void	result_formatter(double *params, char variablechar)
{
	printf("> Final equation \033[1m");

	if (params[2] == 0.0 && params[1] == 0.0)
	{
		printf("(0th degree)\033[0m:\n  ");
		zero_degree_result(params);
	}
	else if (params[2] == 0.0)
	{
		printf("(1st degree):\033[0m\n  ");
		first_degree_result(params, variablechar);
	}
	else
	{
		printf("(2nd degree):\033[0m\n  ");
		second_degree_result(params, variablechar);
	}



}
