#include "parser.h"

int main(int argc, char **argv)
{
	int	is_stdin_input = argc == 1;
	char	*line = NULL;
	char	variablechar = 0;
	
	line = argv[0];
	if (argc > 2)
		error_manager("Multiple arguments provided.");

	if (!is_stdin_input)
		line = argv[1];
	else
		// line = strdup("(x+1)(x-3) = 42");	// TODO <--- HEY
		// line = strdup("1 * (x + 1) / x = 42");	// TODO <--- HEY
		line = strdup("x + x = 0");	// TODO <--- HEY


	printf("> Equation recieved:\n  %s\n\n", line);

	t_item *items = parser(line, &variablechar);

	printf("> Equation as types:\n  ");
	type_line_printer(items);
	printf("\n");

	make_negatives(items);
	t_item *no_mult = append_multiplications(items);

	printf("> Formed negative numbers and assumed hidden multiplications\n  ");
	type_line_printer(no_mult);
	printf("\n");


	t_group_item	*grouped = make_groups(&no_mult);

	printf("> Numbers and operations merged represented as single expressions\n  ");
	groupprinter(grouped);
	printf("\n");




	eliminate_lonely_parentheses(grouped);
	grouped = nothingness_cleanup(grouped);

	printf("> Eliminated useless parentheses\n  ");
	groupprinter(grouped);
	printf("\n");
	
	merge_expressions_in_parentheses(grouped);
	grouped = nothingness_cleanup(grouped);




	printf("> Operated on expressions inside parentheses\n  ");
	groupprinter(grouped);
	printf("\n");

	grouped = merge_multiplication_expression_and_parenthesis(grouped);
	grouped = nothingness_cleanup(grouped);

	printf("> Merged expressions that multiplied/divided each other\n  ");
	groupprinter(grouped);
	printf("\n");
	


	multiply_expression_by_parenthesis(grouped);
	grouped = nothingness_cleanup(grouped);


	printf("> Merged expressions that multiplied/divided others inside parentheses\n  ");
	groupprinter(grouped);
	printf("\n");

	eliminate_parentheses(grouped);
	grouped = nothingness_cleanup(grouped);

	printf("> Removed unnecessary parentheses\n  ");
	groupprinter(grouped);
	printf("\n");


	add_everything_up(grouped);
	grouped = nothingness_cleanup(grouped);
	apply_final_negatives(grouped);

	

	printf("> Reducing all expressions\n  ");
	groupprinter(grouped);
	printf("\n");
	if (is_exponent_out_of_bounds(grouped))
		error_manager("The exponent of an expression is out of bounds.");
	
	double	*second_deg_equation = get_equation_values(grouped);
	free(grouped);

	result_formatter(second_deg_equation, variablechar);

}