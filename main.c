#include "parser.h"


int main()
{
	char *hi = strdup("3 + -a + 3 * (2 * [-2 - 4] * 3a + 1) * 3 +-8.42 = 4.2 + 1 + 5a + 7 * 4a^2 + 9");
	char *cpy = hi;
	char var;

	advance(&hi);
	
	printf("- %d\n", check_parentheses(&cpy));
	printf("%s\n", hi);
	var = get_variable_char(hi);
	if (var == -1)
	{
		printf("Error: more than one variable was given.\n");
		return (1);
	}
	else
		printf("Variable: %c\n", var);
	t_item	*items = typeit(hi);

	if (find_illegal_stuff(items))
	{
		free(items);
		return (1);
	}

	make_negatives(items);
	t_item *no_mult = append_multiplications(items);


	typeprinter(items);
	printf("\n~~~~~~~~~~~~~~~~~~\n\n");
	typeprinter(no_mult);
	printf("\n~~~~~~~~~~~~~~~~~~\n\n");

	t_group_item	*grouped = make_groups(&no_mult);

	groupprinter(grouped);

	printf("\nMERGING MULTIPLICATIONS\n\n");
	merge_multiplication_expression_and_parenthesis(grouped);
	printf("\nAPPLIED\n\n");
	groupprinter(grouped);
	grouped = nothingness_cleanup(grouped);
	groupprinter(grouped);
	printf("\nAPPLY OPERATIONS\n\n");
	multiply_expression_by_parenthesis(grouped);
	printf("\nAPPLIED\n\n");
	groupprinter(grouped);

	printf("\nELIMINATE PARENTHESES\n\n");
	eliminate_parentheses(grouped);
	groupprinter(grouped);
	grouped = nothingness_cleanup(grouped);
	groupprinter(grouped);

	printf("\nADDING UP\n\n");
	add_everything_up(grouped);
	groupprinter(grouped);
	grouped = nothingness_cleanup(grouped);
	groupprinter(grouped);
	
	double	*second_deg_equation = get_equation_values(grouped);
	free(grouped);



	printf("RESULT: %fx^2 + %fx + %f\n", second_deg_equation[2], second_deg_equation[1], second_deg_equation[0]);
	int	solutions = get_second_degree_solutions(second_deg_equation);

	if (solutions == -1)
		printf("Equation results in imaginary numbers.\n");
	else if (solutions == 0)
		printf("Equation divides by 0.\n");
	else if (solutions == 1)
	{
		printf("Only solution: %f\n", get_second_degree_first_solution(second_deg_equation));
	}
	else
	{
		printf("First solution: %f\n", get_second_degree_first_solution(second_deg_equation));
		printf("Second solution: %f\n", get_second_degree_second_solution(second_deg_equation));
	}


	printf("\n~~~~~~~~~~~~~~~~~~\n\n");
}