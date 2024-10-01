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
		if (**input == '=' && parenthesis_type != 0)
		{
			write(2, "Error: Equal sign '=' found inside parentheses.\n", 48);
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

// static int are_there_parentheses(const t_item *items)
// {
// 	while (items->type != THEEND)
// 	{
// 		if (items->type == PARENTHESIS)
// 			return (1);
// 		items++;
// 	}
// 	return (0);
// }

// static t_item *simplify(t_item *thelong)
// {
	
// }

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
	{
		dprintf(2, "Error: Operation isn't preceeded by an expression.\n");
		return (1);
	}
	while (items->type != THEEND)
	{
		if (items->type == OPERATOR && (((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value)) == CLOSING || (items + 1)->type == THEEND || (items + 1)->type == EQUAL))
		{
			dprintf(2, "Error: Operation isn't followed by an expression.\n");
			return (1);
		}
		else if (items->type == PARENTHESIS && get_parenthesis_type((items)->value) == OPENING && (items + 1)->type == OPERATOR && ((items + 1)->value == MUL || (items + 1)->value == DIV))
		{
			dprintf(2, "Error: Operation isn't preceeded by an expression.\n");
			return (1);
		}
		else if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING && (items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == CLOSING)
		{
			dprintf(2, "Error: Parentheses aren't holding any expression.\n");
			return (1);
		}
		else if (items->type == OPERATOR && (items + 1)->type == OPERATOR)
		{
			if ((items + 1)->value == MUL || (items + 1)->value == DIV)
			{
				dprintf(2, "Error: Two consecutive operators found (for exponents, use '^').\n");
				return (1);
			}
			else if ((items + 2)->type == OPERATOR)
			{
				dprintf(2, "Error: Three or more consecutive operators found (double + or - with no parentheses are not allowed).\n");
				return (1);
			}
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
					{
						printf(">> %f turned to %f\n", (items + 2)->numvalue, (items + 2)->numvalue * -1.0f);
						(items + 2)->numvalue *= -1.0;
					}
					else
					{
						printf(">> %f stayed the same.\n", (items + 2)->numvalue);
					}
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



/*
t_expression *turn2expressions(t_item *items)
{
	t_expression	*result;
	t_expression	*new_item;
	t_expression	*last = NULL;
	if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		items++;

	while (items->type != THEEND && get_parenthesis_type(items->value) == OPENING)
	{
		new_item = malloc(sizeof(t_expression));
		new_item->value = 0.0f;
		new_item->degree = 0;
		new_item->next_operation = IDK;
		new_item->prev_operation = IDK;
		new_item->inside = NULL;
		new_item->prev = NULL;
		new_item->next = NULL;

		if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		{
			new_item->inside = turn2expressions(items);
			int index = 1;
			while (index)
			{
				items++;
				if (items->type == PARENTHESIS)
				{
					if (get_parenthesis_type(items->value) == CLOSING)
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
			else if (items->type == VARIABLE || items->type == NUMBER || ((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == CLOSING))
				new_item->next_operation = MUL;
		}
		else if (items->type == NUMBER)
		{
			new_item->value = items->value;
			if ((items + 1)->type == VARIABLE || ((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == CLOSING))
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
			if ((items + 1)->type == VARIABLE || (items + 1)->type == NUMBER || ((items + 1)->type == PARENTHESIS && get_parenthesis_type((items + 1)->value) == CLOSING))
				new_item->next_operation = MUL;
		}
		new_item->prev = last;
		new_item->prev_operation = last->next_operation;
		last->next = new_item;

		items++;
	}
}
*/

/*
t_group *form_group(t_item *items, t_group *previous)
{
	t_group *result = new_group();
	t_group *current = result;
	double	currentval;
	double	currentexp;

	current->prev = previous;
	if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		items++;
	
	while (!(items->type == PARENTHESIS && get_parenthesis_type(items->value) == CLOSING) && items->type != THEEND)
	{
		if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		{
			result->inside = form_group(items, current);
		}
		currentval = 1.0f;
		currentexp = 0.0f;
		t_item *aux_items = items;

		while (1)
		{
			if (items->type == OPERATOR && items->value == SUB)
			{
				currentval *= -1.0f;
				items++;
			}
			if (items->type == NUMBER)
			{
				currentval *= items->numvalue;
			}
			//else if (items->type == )
		}
	}
	return (result);
}
*/




int main()
{

}

