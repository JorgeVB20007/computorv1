#include "parser.h"

/*
*	0 == BAD
*	1 == GOOD
*/
static int check_parentheses(char **input)
{
	char	parenthesis_type;

	parenthesis_type = 0;
	if (**input == '(' || **input == '[' || **input == '{')
	{
		parenthesis_type = **input;
		(*input)++;
	}
	while (**input)
	{
		if (**input == '(' || **input == '[' || **input == '{')
		{
			if (!check_parentheses(input))
				return (0);
		}
		if (**input == ')' || **input == ']' || **input == '}')
		{
			if (parenthesis_type == 0)
			{
				write(2, "Error: Parenthesis closure doesn't match any opening parenthesis.\n", 66);
				return (0);
			}
			else
			{
				if ((parenthesis_type == '(' && **input == ')') || (parenthesis_type == '[' && **input == ']') || (parenthesis_type == '{' && **input == '}'))
				{
					(*input)++;
					return (1);
				}
				write(2, "Error: Trying to close '", 24);
				write(2, &parenthesis_type, 1);
				write(2, "' with '", 8);
				write(2, *input, 1);
				write(2, "'.\n", 3);
				return (0);
			}
		}
		(*input)++;

	}
	if (!parenthesis_type)
		return (1);
	else
	{
		write(2, "Error: At least one parenthesis wasn't closed properly.\n", 56);
		return (0);
	}
}

/*
*	[+, -, *, /, ^]
*/
static int	is_operation(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
		return (1);
	return (0);
}

static int	is_parenthesis(char c)
{
	if (c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}')
		return (1);
	return (0);
}

static int	is_closing_parenthesis(char c)
{
	if (c == ']' || c == ')' || c == '}')
		return (1);
	return (0);
}

static int is_illegal(char c)
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

static char get_variable_char(char *input)
{
	char	result = 0;

	while (*input)
	{
		if (isalpha(*input) && *input != result)
		{
			if (!result)
				result = *input;
			else
				return (-1);
		}
		input++;
	}
	return (result);
}

static void advance(char **pointer)
{
	while (**pointer == ' ')
	{
		(*pointer)++;
	}
}

t_item *typeit(char *input)
{
	t_item *retval = NULL;
	t_item	toadd;
	int		size = 0;
	char	var = get_variable_char(input);
	int		equalised = 0;
	while (*input)
	{
		advance(&input);
		if (!*input)
			break;
		if (isdigit(*input))
		{
			double	value = 0.0;
			
			while (isdigit(*input))
			{
				value *= 10.0;
				value += (float)((*input) - '0');
				input++;
			}
			if (*input == PERIOD)
			{
				input++;
				double	decimals = 0.0;
				while (isdigit(*input))
				{
					decimals *= 10.0;
					decimals += (float)((*input) - '0');
					input++;
				}
				while (decimals >= 1.0)
				{
					decimals /= 10.0;
				}
				value += decimals;
			}
			if (*input == PERIOD)
			{
				if (retval)
					free(retval);
				dprintf(2, "Error: Used multiple period signs (%c) for a single number.\n", PERIOD);
				return (NULL);
			}
			toadd.type = NUMBER;
			toadd.value = 0;
			toadd.numvalue = value;
			input--;
		}
		else if (is_operation(*input))
		{
			toadd.numvalue = 0.0;
			toadd.type = OPERATOR;
			if (*input == '+')
				toadd.value = SUM;
			else if (*input == '-')
				toadd.value = SUB;
			else if (*input == '*')
				toadd.value = MUL;
			else if (*input == '/')
				toadd.value = DIV;
			else if (*input == '^')
				toadd.value = POW;
		}
		else if (*input == var)
		{
			toadd.numvalue = 0.0;
			toadd.type = VARIABLE;
			toadd.value = *input;
		}
		else if (is_parenthesis(*input))
		{
			toadd.numvalue = 0.0;
			toadd.type = PARENTHESIS;
			toadd.value = *input;
		}
		else if (*input == '=')
		{
			if (equalised)
			{
				if (retval)
					free(retval);
				dprintf(2, "Error: Used multiple equal signs (=).\n");
				return (NULL);
			}
			toadd.numvalue = 0.0;
			toadd.value = 0;
			toadd.type = EQUAL;
			equalised = 1;
		}
		else if (is_illegal(*input))
		{
			if (retval)
				free(retval);
			dprintf(2, "Error: WTF is this char?? --> %c <--.\n", *input);
			return (NULL);
		}
		else
		{
			if (retval)
				free(retval);
			dprintf(2, "Error: An error ocurred when trying to parse the char %c.\n", *input);
			return (NULL);
		}
		retval = realloc(retval, sizeof(t_item) * ++size);
		memcpy(&(retval[size - 1]), &toadd, sizeof(t_item));
		input++;
	}

	toadd.numvalue = 0.0;
	toadd.value = 0;
	toadd.type = THEEND;
	retval = realloc(retval, sizeof(t_item) * ++size);
	memcpy(&(retval[size - 1]), &toadd, sizeof(t_item));
	return (retval);
}

static void	typeprinter(t_item *types)
{
	if (!types)
		return ;
	int	idx = 0;
	while (types[idx].type != THEEND)
	{
		if (types[idx].type == NUMBER)
			printf("> Type: \033[0;33mNBR\033[0m | Value: %f\n", types[idx].numvalue);
		else if (types[idx].type == PARENTHESIS)
			printf("> Type: \033[0;34mPAR\033[0m | Value: %c\n", types[idx].value);
		else if (types[idx].type == OPERATOR)
		{
			if (types[idx].value == SUM)
				printf("> Type: \033[0;32mOPR\033[0m | Value: SUM (+)\n");
			else if (types[idx].value == SUB)
				printf("> Type: \033[0;32mOPR\033[0m | Value: SUB (-)\n");
			else if (types[idx].value == MUL)
				printf("> Type: \033[0;32mOPR\033[0m | Value: MUL (*)\n");
			else if (types[idx].value == DIV)
				printf("> Type: \033[0;32mOPR\033[0m | Value: DIV (/)\n");
			else if (types[idx].value == POW)
				printf("> Type: \033[0;32mOPR\033[0m | Value: POW (^)\n");
		}
		else if (types[idx].type == VARIABLE)
			printf("> Type: \033[0;35mVAR\033[0m | Value: %c\n", types[idx].value);
		else if (types[idx].type == EQUAL)
			printf("> Type: \033[1;36mEQL\033[0m | Value: =\n");
		else
			printf(">> Weird type: %d | Value: %d, NValue: %f", types[idx].type, types[idx].value, types[idx].numvalue);
		idx++;
	}
}

static int are_there_parentheses(const t_item *items)
{
	while (items->type != THEEND)
	{
		if (items->type == PARENTHESIS)
			return (1);
		items++;
	}
	return (0);
}

// static t_item *simplify(t_item *thelong)
// {
	
// }

/*
*	0 = GOOD
*	1 = BAD
*/
int find_illegal_stuff(t_item *items)
{
	if (items->type == OPERATOR)
	{
		dprintf(2, "Error: Operation isn't preceeded by an expression.\n");
		return (1);
	}
	while (items->type != THEEND)
	{
		if (items->type == OPERATOR && (((items + 1)->type == PARENTHESIS && is_closing_parenthesis((items + 1)->value) || (items + 1)->type == THEEND)))
		{
			dprintf(2, "Error: Operation isn't followed by an expression.\n");
			return (1);
		}
		else if (items->type == PARENTHESIS && !is_closing_parenthesis((items)->value) && (((items + 1)->type == OPERATOR)
		{
			dprintf(2, "Error: Operation isn't preceeded by an expression.\n");
			return (1);
		}
		else if (items->type == PARENTHESIS && (((items + 1)->type == PARENTHESIS && is_closing_parenthesis((items + 1)->value))
		{
			dprintf(2, "Error: Parentheses aren't holding any expression.\n");
			return (1);
		}
		else if (items->type == OPERATOR && (items + 1)->type == OPERATOR)
		{
			dprintf(2, "Error: Two consecutive operators found.\n");
			return (1);
		}
		else if (items->type == NUMBER && (items + 1)->type == NUMBER)
		{
			dprintf(2, "Error: Two consecutive numbers found (%d and %d).\n", items->value, (items + 1)->value);
			return (1);
		}
		items++;
	}
	return (0);
}


void turn2expressions(t_item *items)
{
	t_expression	*result;
	t_expression	*new_item;
	t_expression	*last = NULL;
	if (items->type == PARENTHESIS && !is_closing_parenthesis(items->value))
		items++;

	while (items->type != THEEND && !is_closing_parenthesis(items->value))
	{
		new_item = malloc(sizeof(t_expression));
		new_item->value = 0.0f;
		new_item->degree = 0;
		new_item->next_operation = IDK;
		new_item->prev_operation = IDK;
		new_item->inside = NULL;
		new_item->prev = NULL;
		new_item->next = NULL;

		if (items->type == PARENTHESIS && !is_closing_parenthesis(items->value))
		{
			new_item->inside = turn2expressions(items);
			int index = 1;
			while (index)
			{
				items++;
				if (items->type == PARENTHESIS)
				{
					if (is_closing_parenthesis(items->value))
						index--;
					else
						index++;
				}
			}
			items++;
			if (items->type == OPERATOR)
			{
				new_item->next_operation = items->type;
				items++;
			}
			else if (items->type == VARIABLE || items->type == NUMBER || ((items + 1)->type == PARENTHESIS && is_closing_parenthesis((items + 1)->value)))
				new_item->next_operation = MUL;
		}
		else if (items->type == NUMBER)
		{
			new_item->value = items->value;
			if ((items + 1)->type == VARIABLE || ((items + 1)->type == PARENTHESIS && is_closing_parenthesis((items + 1)->value)))
				new_item->next_operation = MUL;
			else if ((items + 1)->type == OPERATOR)
			{
				new_item->next_operation = (items + 1)->value;
				items++;
			}
		}
		else if (items->type == VARIABLE)
		{
			new_item->value = 1.0f;
			new_item->degree = 1;
			if ((items + 1)->type == VARIABLE || (items + 1)->type == NUMBER || ((items + 1)->type == PARENTHESIS && is_closing_parenthesis((items + 1)->value)))
				new_item->next_operation = MUL;
		}
		new_item->prev = last;
		new_item->prev_operation = last->next_operation;
		last->next = new_item;

		items++;
	}
}




int main()
{
	char *hi = strdup("2a * 2 + (2 * [2 - 4]) * 8.42 = 0");
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

	


	typeprinter(items);
}

