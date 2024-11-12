#include "parser.h"


static void do_it(char *argv)
{
	char	variablechar = 0;

	printf("> Equation recieved:\n  %s\n\n", argv);

	t_item *items = parser(argv, &variablechar);

	printf("> Equation as types:\n  ");
	type_line_printer(items);
	printf("\n");

	make_negatives(items);
	t_item *no_mult = append_multiplications(items);

	printf("> Formed negative numbers and assumed hidden multiplications \033[3;90m(1 - 5x -> 1 + -5 * x)\033[0m\n  ");
	type_line_printer(no_mult);
	printf("\n");


	t_group_item	*grouped = make_groups(&no_mult);

	printf("> Numbers and operations merged represented as single expressions \033[3;90m(5 * x -> 5 times a variable)\033[0m\n  ");
	groupprinter(grouped);
	printf("\n");

	free(items);




	printf("> Eliminating redundant parentheses and solving basic operations \033[3;90m(5 * 3 - [4] -> 5 * 3 - 4 -> 15 - 4 -> 11)\033[0m\n");

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

	printf("\n> Merged expressions that multiplied/divided each other \033[3;90m(3 * [expression] * 2 -> 6 * [expression])\033[0m\n  ");
	groupprinter(grouped);
	printf("\n");
	


	multiply_expression_by_parenthesis(grouped);
	grouped = nothingness_cleanup(grouped);

	printf("> Merged expressions that multiplied/divided others inside parentheses \033[3;90m(5x * [x + 2] -> 5x^2 + 10x)\033[0m\n  ");
	groupprinter(grouped);
	printf("\n");


	int something_changed = 1;
	while (something_changed)
	{
		something_changed = 0;
		grouped = multiply_parentheses_by_themselves(grouped, &something_changed);
		grouped = nothingness_cleanup(grouped);
		sort_items(grouped);

		something_changed += merge_expressions_in_parentheses_sum_sub(grouped);
		grouped = nothingness_cleanup(grouped);
		sort_items(grouped);

		check_and_fix_illegal_divisions(grouped, &something_changed);
		grouped = nothingness_cleanup(grouped);
		sort_items(grouped);

		something_changed += eliminate_lonely_parentheses(grouped);
		grouped = nothingness_cleanup(grouped);

		multiply_expression_by_parenthesis(grouped);
		grouped = nothingness_cleanup(grouped);
	}
	

	printf("> Multiplied and divided long expressions in parentheses \033[3;90m([x + 1] * [x - 2] -> [x^2 - x - 2])\033[0m\n  ");
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
	free(second_deg_equation);
}



int main(int argc, char **argv)
{
	
	if (argc > 2)
		error_manager("Multiple arguments provided.");
	else if (argc < 2)
	{
		while (1)
		{
			write(1, "Enter your equation: ", 21);
			char	addchar = 1;
			char	*param = calloc(1, sizeof(char));
			int		size = 1;

			while (addchar != 0 && addchar != '\n')
			{
				read(1, &addchar, 1);
				if (addchar == '\n')
					break;
				size++;
				param = realloc(param, size);
				param[size - 2] = addchar;
				param[size - 1] = 0;
			}
			do_it(param);
			free(param);
			write(1, "\n", 1);
		}
	}
	else
	{
		do_it(argv[1]);
	}
}
