#include "parser.h"



int main(int argc, char **argv)
{
	char	variablechar = 0;
	
	if (argc > 2)
		error_manager("Multiple arguments provided.");
	else if (argc < 2)
		error_manager("No arguments provided.");


	printf("> Equation recieved:\n  %s\n\n", argv[1]);

	t_item *items = parser(argv[1], &variablechar);

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






	printf("> Eliminating redundant parentheses and solving basic operations\n");

	int changes_made = 1;
	int prev_changes;
	while (changes_made)
	{
		changes_made = 0;
		prev_changes = 0;

		changes_made += eliminate_lonely_parentheses(grouped);
		grouped = nothingness_cleanup(grouped);
		print_group_if_necessary(grouped, changes_made, prev_changes);
		prev_changes = changes_made;


		changes_made += merge_expressions_in_parentheses_mul_div(grouped);
		grouped = nothingness_cleanup(grouped);
		print_group_if_necessary(grouped, changes_made, prev_changes);
		prev_changes = changes_made;

		changes_made += merge_expressions_in_parentheses_sum_sub(grouped);
		grouped = nothingness_cleanup(grouped);
		print_group_if_necessary(grouped, changes_made, prev_changes);
		prev_changes = changes_made;
	}



	grouped = merge_multiplication_expression_and_parenthesis(grouped);
	grouped = nothingness_cleanup(grouped);

	printf("\n> Merged expressions that multiplied/divided each other\n  ");
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
////	last_operations(grouped);
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
