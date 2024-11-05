#include "parser.h"

/*
*	Exits when error, continues when check succeeded.
*/
void check_parentheses(char **input)
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
			check_parentheses(input);
		}
		if (**input == '=' && parenthesis_type != 0)
			error_manager("Equal sign '=' found inside parentheses.");
		if (**input == ')' || **input == ']' || **input == '}')
		{
			if (parenthesis_type == 0)
				error_manager("Parenthesis closure doesn't match any opening parenthesis.");
			else
			{
				if ((parenthesis_type == '(' && **input == ')') || (parenthesis_type == '[' && **input == ']') || (parenthesis_type == '{' && **input == '}'))
				{
					(*input)++;
					return ;
				}
				write(2, "Error: Trying to close '", 24);
				write(2, &parenthesis_type, 1);
				write(2, "' with '", 8);
				write(2, *input, 1);
				write(2, "'.\n", 3);
				return ;
			}
		}
		(*input)++;

	}
	if (parenthesis_type)
		error_manager("At least one parenthesis wasn't closed properly.");
}

char get_variable_char(char *input)
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
				value += (double)((*input) - '0');
				input++;
			}
			if (*input == PERIOD)
			{
				input++;
				double	decimals = 0.0;
				while (isdigit(*input))
				{
					decimals *= 10.0;
					decimals += (double)((*input) - '0');
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
				error_manager("Used multiple period signs for a single number.");
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
				error_manager("Used multiple equal signs (=).");
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
			exit (1);
			return (NULL);
		}
		else
		{
			if (retval)
				free(retval);
			dprintf(2, "Error: An error ocurred when trying to parse the char %c.\n", *input);
			exit(1);
			return (NULL);
		}
		retval = realloc(retval, sizeof(t_item) * ++size);
		memcpy(&(retval[size - 1]), &toadd, sizeof(t_item));
		input++;
	}
	if (!equalised)
		error_manager("No equal sign (=) found.");

	toadd.numvalue = 0.0;
	toadd.value = 0;
	toadd.type = THEEND;
	retval = realloc(retval, sizeof(t_item) * ++size);
	memcpy(&(retval[size - 1]), &toadd, sizeof(t_item));
	return (retval);
}


/*
*	0 = GOOD
*	1 = BAD

?	Finds:
?	
?	
?	
?	
?	
*/
int find_illegal_stuff(t_item *items)
{
	if (items->type == OPERATOR && (items->value == MUL || items->value == DIV))
		error_manager("Operation isn't preceeded by an expression.");
	while (items->type != THEEND)
	{
		if (items->type == OPERATOR && (((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value)) == CLOSING || (items + 1)->type == THEEND || (items + 1)->type == EQUAL))
			error_manager("Operation isn't followed by an expression.");
		else if (items->type == PARENTHESIS && get_parenthesis_type((items)->value) == OPENING && (items + 1)->type == OPERATOR && ((items + 1)->value == MUL || (items + 1)->value == DIV))
			error_manager("Operation isn't preceeded by an expression.");
		else if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING && (items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == CLOSING)
			error_manager("Parentheses aren't holding any expression.");
		else if (items->type == OPERATOR && (items + 1)->type == OPERATOR)
		{
			if ((items + 1)->value == MUL || (items + 1)->value == DIV)
				error_manager("Two consecutive operators found (for exponents, use '^').");
			else if ((items + 2)->type == OPERATOR)
				error_manager("Three or more consecutive operators found (double + or - with no parentheses are not allowed).");
		}
		else if (items->type == NUMBER && (items + 1)->type == NUMBER)
		{
			dprintf(2, "Error: Two consecutive numbers found (%g and %g).\n", items->numvalue, (items + 1)->numvalue);
			return (1);
		}
		items++;
	}
	return (0);
}

void make_negatives(t_item *items)
{
	int	was_operator = 0;
	int	apply_negative = 0;

	if (items->type == OPERATOR)
	{
		was_operator = 1;
		if (items->value == SUB)
			apply_negative = 1;
		if ((items + 1)->type == NUMBER)
		{
			items->type = NOTHING;
			items++;
			if (apply_negative)
				items->numvalue *= -1.0f;
		}
		else if ((items + 1)->type == VARIABLE || ((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == OPENING))
		{
			if (apply_negative)
			{
				items->type = NUMBER;
				items->numvalue = -1.0;
				items->value = 0;
			}
			else
				items->type = NOTHING;
			items++;
		}
	}

	was_operator = 0;
	apply_negative = 0;
	(void)was_operator;

	while (items->type != THEEND)
	{
		if (items->type == OPERATOR || (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING))
		{
			if ((items + 1)->type == OPERATOR)
			{
				if ((items + 2)->type == NUMBER)
				{
					if ((items + 1)->value == SUB)
						(items + 2)->numvalue *= -1.0;
					(items + 1)->type = NOTHING;
				}
				else if ((items + 2)->type == VARIABLE || ((items + 2)->type == PARENTHESIS && get_parenthesis_type((items + 2)->value) == OPENING))
				{
					if ((items + 1)->value == SUB)
					{
						(items + 1)->type = NUMBER;
						(items + 1)->value = 0;
						(items + 1)->numvalue = -1.0;
					}
					else
					{
						(items + 1)->type = NOTHING;
					}
				}
			}
		}
		items++;
	}
}

t_item *append_multiplications(t_item *items)
{
	t_item	*result = malloc(0);
	int		size = 0;

	while (items->type != THEEND)
	{
		if (items->type == OPERATOR || items->type == EQUAL)
		{
			result = realloc(result, sizeof(t_item) * (size + 1));

			memcpy(&(result[size]), items, sizeof(t_item));
			size++;
		}
		else if (items->type == NUMBER || items->type == VARIABLE || (items->type == PARENTHESIS && get_parenthesis_type(items->value) == CLOSING))
		{
			if ((items + 1)->type == NUMBER || (items + 1)->type == VARIABLE || ((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == OPENING))
			{
				result = realloc(result, sizeof(t_item) * (size + 2));
				memcpy(&(result[size]), items, sizeof(t_item));
				result[size + 1].type = OPERATOR;
				result[size + 1].value = MUL;
				result[size + 1].numvalue = 0;
				size += 2;
			}
			else
			{
				result = realloc(result, sizeof(t_item) * (size + 1));
				memcpy(&(result[size]), items, sizeof(t_item));
				size++;
			}
		}
		else if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		{
			result = realloc(result, sizeof(t_item) * (size + 1));
			memcpy(&(result[size]), items, sizeof(t_item));
			size++;
		}
		items++;
	}
	result = realloc(result, sizeof(t_item) * (size + 1));

	memcpy(&(result[size]), items, sizeof(t_item));
	return (result);
}




t_item *parser(char *line, char *variablechar)
{
	advance(&line);

	*variablechar = get_variable_char(line);
	if (*variablechar == -1)
		error_manager("More than one variable was given.");
	t_item	*items = typeit(line);

	if (find_illegal_stuff(items))
	{
		free(items);
		exit (1);
	}
	return (items);
}

// void	check_equal_validity(t_group_item *items)
// {

// }
