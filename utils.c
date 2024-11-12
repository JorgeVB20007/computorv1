#include "parser.h"

/*
*	[+, -, *, /, ^]
*/
int	is_operation(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
		return (1);
	return (0);
}

int	is_parenthesis(char c)
{
	if (c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}')
		return (1);
	return (0);
}

int	get_parenthesis_type(char c)
{
	if (c == ']' || c == ')' || c == '}')
		return (CLOSING);
	return (OPENING);
}

int is_illegal(char c)
{
	if (isalpha(c))
		return (0);
	if (isdigit(c))
		return (0);
	if (is_operation(c))
		return (0);
	if (is_parenthesis(c))
		return (0);
	if (c == PERIOD)
		return (0);
	return (1);
}

void error_manager(const char *reason)
{
	dprintf(2, "\033[1;31mError:\033[0m %s\n", reason);
	exit(1);
}

int	is_exponent_out_of_bounds(t_group_item *items)
{
	while (items->type != THEEND)
	{
		if (items->type == EXPRESSION && (items->exponent < 0.0 || items->exponent > 2.0))
			return (1);
		items++;
	}

	return (0);
}
