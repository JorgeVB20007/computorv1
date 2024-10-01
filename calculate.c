#include "parser.h"

t_group_item	*grouped_item(t_item *item)
{
	t_group_item	*result = malloc(sizeof(t_group_item));

	result->type = item->type;
	result->value = item->value;
	result->multiplier = 0.0;
	result->exponent = 0.0;

	return (result);
}

t_group_item	*nothingness_cleanup(t_group_item *old)
{
	int				new_size = 1;
	t_group_item	*aux = old;
	t_group_item	*new = NULL;
	t_group_item	*new_aux = NULL;

	while (aux->type != THEEND)
	{
		if (aux->type != NOTHING)
			new_size++;
		aux++;
	}
	aux = old;
	new = malloc(sizeof(t_group_item) * new_size);
	new_aux = new;
	while (aux->type != THEEND)
	{
		if (aux->type != NOTHING)
		{
			memcpy(new_aux, aux, sizeof(t_group_item));
			new_aux++;
		}
		aux++;
	}
	memcpy(new_aux, aux, sizeof(t_group_item));
	free(old);
	return (new);
}

void	eliminate_parentheses(t_group_item *target)
{
	while (target->type != THEEND)
	{
		if (target->type == PARENTHESIS)
			target->type = NOTHING;
		target++;
	}
}

t_group_item	*append_to_group(t_group_item *old, t_group_item *new, int size)
{
	t_group_item	*result;

	result = realloc(old, (size + 1) * sizeof(t_group_item));
	memcpy(&(result[size]), new, sizeof(t_group_item));
	free(new);
	return (result);
}

double	power_stacking(t_item **items)
{
	double	currentvalue = (*items)->numvalue;

	if ((*items + 1)->type == OPERATOR && (*items + 1)->value == POW)
	{
		if ((*items + 2)->type == NUMBER)
		{
			*items += 2;
			return (pow(currentvalue, power_stacking(items)));
		}
		else
		{
			dprintf(2, "Error: An expression other than a number was found after an exponent.\n");
			exit(1);
		}
	}
	else
		return(currentvalue);
}




t_group_item	*make_groups(t_item **items)
{
	t_group_item	*result = malloc(0);
	int				size = 0;
	int				is_division = 0;

	while ((*items)->type != THEEND)
	{
		if ((*items)->type == PARENTHESIS || (*items)->type == EQUAL || (*items)->type == OPERATOR)
		{
			result = append_to_group(result, grouped_item(*items), size);

			size++;
			(*items)++;
		}
		else if ((*items)->type == NUMBER || (*items)->type == VARIABLE)
		{
			result = realloc(result, sizeof(t_group_item) * (size + 1));
			result[size].type = EXPRESSION;
			result[size].exponent = 0.0;
			result[size].multiplier = 1.0;
			while (1)
			{
				if ((*items)->type == NUMBER)
				{
					if (is_division)
						result[size].multiplier /= power_stacking(items);
					else
						result[size].multiplier *= power_stacking(items);
				}
				else if ((*items)->type == VARIABLE)
				{
					result[size].value = (*items)->value;
					if ((*items + 1)->type == OPERATOR && (*items + 1)->value == POW)
					{
						if ((*items + 2)->type == NUMBER)
						{
							(*items) += 2;
							if (is_division)
								result[size].exponent -= power_stacking(items);
							else
								result[size].exponent += power_stacking(items);
						}
						else
						{
							dprintf(2, "Error: An expression other than a number was found after an exponent.\n");
							exit(1);
						}
					}
					else
					{
						if (is_division)
							result[size].exponent -= 1;
						else
							result[size].exponent += 1;
					}
				}
				(*items)++;
				if ((*items)->type == OPERATOR && ((*items)->value == MUL || (*items)->value == DIV))
				{
					if ((*items + 1)->type == PARENTHESIS)
						break ;

					if ((*items)->value == DIV)
						is_division = 1;
					(*items)++;
				}
				else
				{
					is_division = 0;
					break ;
				}
			}
			size++;
		}
		//result = realloc();
	}
	result = realloc(result, sizeof(t_group_item) * (size + 1));
	result[size].type = THEEND;
	return (result);
}

/*
*	Multiplier will be applied first.
*/
static void		apply_to_all_members_in_parenthesis(t_group_item multiplier, t_group_item *items, const int is_division)
{
	int	relative_scope = 1;
	int	already_multiplied = 0;

	while (relative_scope)
	{
		if (items->type == EXPRESSION && !already_multiplied)
		{
			if (items->exponent == 0.0)
				items->value = multiplier.value;
			if (is_division)
			{
				items->multiplier = multiplier.multiplier / items->multiplier;
				items->exponent = multiplier.exponent - items->exponent;
			}
			else
			{
				items->multiplier = multiplier.multiplier * items->multiplier;
				items->exponent = multiplier.exponent + items->exponent;
			}
			if ((items + 1)->type == OPERATOR && \
			((items + 1)->value == MUL || (items + 1)->value == DIV))
			{
				already_multiplied = relative_scope;
			}
		}
		else if (items->type == PARENTHESIS)
		{
			if (get_parenthesis_type(items->value) == OPENING)
				relative_scope++;
			else
				relative_scope--;
		}
		else if (items->type == OPERATOR && (items->value == SUM || items->value == SUB))
		{
			if (already_multiplied >= relative_scope)
				already_multiplied = 0;
		}
		if (already_multiplied > relative_scope)
			already_multiplied = 0;
		items++;
	}
}

static t_group_item	*get_parenthesis_end(t_group_item *items)
{
	int	relative_scope = 1;

	while (relative_scope)
	{
		if (items->type == PARENTHESIS)
		{
			if (get_parenthesis_type(items->value) == OPENING)
				relative_scope++;
			else
				relative_scope--;
		}
		else if (items->type == THEEND)
		{
			dprintf(2, "Error finding close parenthesis.\n");
			exit(1);
		}
		items++;
	}
	return (items);
}

void	merge_multiplication_expression_and_parenthesis(t_group_item *items)
{
	t_group_item	*prev_expression = NULL;
	t_group_item	*parenthesis_end = NULL;

	while (items->type != THEEND)
	{
		if (items->type == EXPRESSION && (items + 1)->type == OPERATOR && ((items + 1)->value == MUL || (items + 1)->value == DIV))
		{
			prev_expression = items;
			items++;
		}
		else if ((items)->type == PARENTHESIS && get_parenthesis_type((items)->value) == OPENING)
		{
			parenthesis_end = get_parenthesis_end(items + 1);
			if (parenthesis_end->type == OPERATOR && (parenthesis_end->value == MUL || parenthesis_end->value == DIV) && (parenthesis_end + 1)->type == EXPRESSION)
			{
				if ((parenthesis_end + 1)->exponent > 0.0)
					prev_expression->value = (parenthesis_end + 1)->value;
				if (parenthesis_end->value == MUL)
				{
					prev_expression->exponent += (parenthesis_end + 1)->exponent;
					prev_expression->multiplier *= (parenthesis_end + 1)->multiplier;
				}
				else
				{
					prev_expression->exponent -= (parenthesis_end + 1)->exponent;
					prev_expression->multiplier /= (parenthesis_end + 1)->multiplier;
				}
				(parenthesis_end + 1)->type = NOTHING;
				parenthesis_end->type = NOTHING;
			}
		}
		items++;
	}
}

void	multiply_expression_by_parenthesis(t_group_item *items)
{
	while (items->type != THEEND)
	{
		if (items->type == EXPRESSION && (items + 1)->type == OPERATOR && \
		((items + 1)->value == MUL || (items + 1)->value == DIV) && \
		(items + 2)->type == PARENTHESIS && get_parenthesis_type((items + 2)->value) == OPENING)
		{
			apply_to_all_members_in_parenthesis(*items, items + 3, (items + 1)->value == DIV);
			items->type = NOTHING;
			items++;
			items->type = NOTHING;
		}




		items++;
	}

}

void	add_everything_up(t_group_item *items)
{
	t_group_item	*items_aux = items;
	int				do_times = 2;

	while (do_times--)
	{
		while (items->type != EQUAL && items->type != THEEND)
		{
			if (items->type == EXPRESSION)
			{
				items_aux = items;
				while (items_aux->type != EQUAL && items_aux->type != THEEND)
				{
					if (items_aux->type == OPERATOR && (items_aux + 1)->type == EXPRESSION && (items_aux + 1)->exponent == items->exponent)
					{
						if (items_aux->value == SUM)
							items->multiplier += (items_aux + 1)->multiplier;
						else if (items_aux->value == SUB)
							items->multiplier -= (items_aux + 1)->multiplier;
						else
						{
							dprintf(2, "ERROR: Operations other than + and - were found at this step.\n");
							exit (1);
						}
						items_aux->type = NOTHING;
						items_aux++;
						items_aux->type = NOTHING;
					}
					items_aux++;
				}
			}
			items++;
		}
		items++;
	}
}

double	*get_equation_values(t_group_item	*items)
{
	double			*result = calloc(sizeof(double), 3);
	int				searching_degree = 2;
	t_group_item	*items_aux = items;
	int				passed_equal;

	while (searching_degree >= 0)
	{
		passed_equal = 0;
		items_aux = items;
		while (items_aux->type != THEEND)
		{
			if (items_aux->type == EXPRESSION)
			{
				if (items_aux->exponent == (double)searching_degree)
				{
					if (passed_equal)
						result[searching_degree] -= items_aux->multiplier;
					else
						result[searching_degree] += items_aux->multiplier;
				}
			}
			else if (items_aux->type == EQUAL)
				passed_equal = 1;
			items_aux++;
		}
		searching_degree--;;
	}
	return (result);
}


int	get_second_degree_solutions(double *params)
{
	if (params[2] == 0)
		return (0);
	else if (pow(params[1], 2) == 4.0 * params[2] * params[0])
		return (1);
	else if (pow(params[1], 2) < 4.0 * params[2] * params[0])
		return (-1);
	else
		return (2);
}

double get_second_degree_first_solution(double *params)
{
	return ((params[1] * -1.0 + sqrtf(pow(params[1], 2.0) - 4 * params[2] * params[0])) / (2.0 * params[2]));
}


double get_second_degree_second_solution(double *params)
{
	return ((params[1] * -1.0 - sqrtf(pow(params[1], 2.0) - 4 * params[2] * params[0])) / (2.0 * params[2]));
}


