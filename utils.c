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
