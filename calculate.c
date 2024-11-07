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

int	eliminate_lonely_parentheses(t_group_item *target)
{
	int				items_counter = 0;
	int				context = 0;
	t_group_item	*aux = target;
	int retval = 0;

	while (target->type != THEEND)
	{
		if (target->type == PARENTHESIS && get_parenthesis_type(target->value) == OPENING)
		{
			aux = target + 1;
			context = 1;
			items_counter = 0;
			while (context)
			{
				if (aux->type == EXPRESSION)
					items_counter++;
				else if (aux->type == PARENTHESIS)
				{
					if (get_parenthesis_type(aux->value) == OPENING)
						context++;
					else
						context--;
				}
				if (context)
					aux++;
			}
			if (items_counter <= 1)
			{
				target->type = NOTHING;
				aux->type = NOTHING;
				retval++;
			}
		}
		target++;
	}
	return (retval);
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
			return (my_pow(currentvalue, power_stacking(items)));
		}
		else
			error_manager("An expression other than a number was found after an exponent.");
		return (0.0);
	}
	else
		return(currentvalue);
}

void	simplify_inside_parentheses(t_group_item *items)
{
	t_group_item	*aux;
	int				context = 1;

	while (context && items->type != EQUAL)
	{
		if (items->type == EXPRESSION)
		{
			aux = items;
			context = 1;
			while (aux->type != EQUAL && context)
			{
				if (aux->type == PARENTHESIS)
				{
					if (get_parenthesis_type(aux->value) == OPENING)
						context++;
					else
						context--;
				}
				if (aux->type == OPERATOR && (aux + 1)->type == EXPRESSION)
				
				aux++;
			}
		} 
		items++;
	}
	if (!context)
		return;
	
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
					{
						if (power_stacking(items) == 0.0)
							error_manager("A division by 0 was encountered.");
						result[size].multiplier /= power_stacking(items);
					}
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
							error_manager("An expression other than a number was found after an exponent.");
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
					else
						is_division = 0;
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
				if (items->multiplier == 0.0)
					error_manager("A division by 0 was encountered.");
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

/*
*	Starts already inside the expression.
*	Returns the location right after the closing parenthesis.
*/
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
			error_manager("(FATAL) error finding closing parenthesis.");
		items++;
	}
	return (items);
}

static t_group_item	*multiplybyone_safety(t_group_item *items)
{
	t_group_item	*itemscpy = items;
	t_group_item	*new_items = NULL;
	t_group_item	*new_itemscpy = NULL;
	int				size = 1;

	while (itemscpy->type != THEEND)
	{
		size++;
		itemscpy++;
	}
	itemscpy = items;
	while (itemscpy->type != THEEND)
	{
		if (itemscpy->type == EXPRESSION && (itemscpy + 1)->type == OPERATOR && \
		((itemscpy + 1)->value == MUL || (itemscpy + 1)->value == DIV) && \
		(itemscpy + 2)->type == PARENTHESIS && get_parenthesis_type(itemscpy->value) == OPENING)
		{
			itemscpy += 2;
		}
		else if (itemscpy->type == PARENTHESIS && get_parenthesis_type(itemscpy->value) == OPENING)
		{
			size += 2;
		}
		itemscpy++;
	}

	itemscpy = items;
	new_items = calloc(sizeof(t_group_item), size);
	new_itemscpy = new_items;

	while (itemscpy->type != THEEND)
	{
		if (itemscpy->type == EXPRESSION && (itemscpy + 1)->type == OPERATOR && \
		((itemscpy + 1)->value == MUL || (itemscpy + 1)->value == DIV) && \
		(itemscpy + 2)->type == PARENTHESIS && get_parenthesis_type(itemscpy->value) == OPENING)
		{
			memcpy(new_itemscpy, itemscpy, sizeof(t_group_item) * 3);
			itemscpy += 2;
			new_itemscpy += 2;
		}
		else if (itemscpy->type == PARENTHESIS && get_parenthesis_type(itemscpy->value) == OPENING)
		{
			new_itemscpy->type = EXPRESSION;
			new_itemscpy->multiplier = 1.0;
			new_itemscpy->exponent = 0.0;
			new_itemscpy++;
			new_itemscpy->type = OPERATOR;
			new_itemscpy->value = MUL;
			new_itemscpy++;
			new_itemscpy->type = PARENTHESIS;
			new_itemscpy->value = itemscpy->value;
		}
		else
			memcpy(new_itemscpy, itemscpy, sizeof(t_group_item));
		new_itemscpy++;
		itemscpy++;
	}
	memcpy(new_itemscpy, itemscpy, sizeof(t_group_item));
	free(items);
	return (new_items);
}

/*
*	Turns `x * (2 + 3)` into `x * (5)`.
*/
int	merge_expressions_in_parentheses_sum_sub(t_group_item *items)
{
	t_group_item	*aux;
	t_group_item	*aux2;
	int				context = 1;
	int				context2 = 1;
	int				retval = 0;

	apply_final_negatives(items);

	while (items->type != THEEND)
	{
		if (items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING)
		{
			aux = items + 1;
			context = 1;
			while (context)
			{
				if (aux->type == PARENTHESIS)
				{
					if (get_parenthesis_type(aux->value) == OPENING)
					{
						aux = get_parenthesis_end(aux + 1) - 1;
					}
					else
						context = 0;
				}
				else if (aux->type == EXPRESSION)
				{
					context2 = 1;
					aux2 = aux + 1;
					while (context2)
					{
						if (aux2->type == OPERATOR && (aux2->value == MUL || aux2->value == DIV) && (aux2 + 1)->type == EXPRESSION)
							aux2++;
						else if (aux2->type == EXPRESSION && !((aux2 + 1)->type == OPERATOR && ((aux2 + 1)->value == MUL || (aux2 + 1)->value == DIV)))
						{
							if (aux2->exponent == aux->exponent)
							{
								aux->multiplier += aux2->multiplier;
								aux2->type = NOTHING;
								(aux2 - 1)->type = NOTHING;
								retval++;
							}

						}
						else if (aux2->type == PARENTHESIS && get_parenthesis_type(aux2->value) == OPENING)
							aux2 = get_parenthesis_end(aux2 + 1) - 1;
						else if (aux2->type == PARENTHESIS && get_parenthesis_type(aux2->value) == CLOSING)
							context2 = 0;
						aux2++;
					}
				}
				aux++;
			}
		}
		items++;
	}
	return (retval);
}

/*
*	Turns `x * (2 * 3)` into `x * (6)`.
*/
int	merge_expressions_in_parentheses_mul_div(t_group_item *items)
{
	t_group_item	*aux;
	t_group_item	*aux2;
	int				context = 1;
	int				context2 = 1;
	int				operation = IDK; 
	int				retval = 0;
	int				first = 1;

	apply_final_negatives(items);

	while (items->type != THEEND)
	{
		if ((items->type == PARENTHESIS && get_parenthesis_type(items->value) == OPENING) || first)
		{
			aux = items + !first;
			context = 1;
			while (context)
			{
				if (aux->type == PARENTHESIS)
				{
					if (get_parenthesis_type(aux->value) == OPENING)
					{
						aux = get_parenthesis_end(aux + 1) - 1;
					}
					else
						context = 0;
				}
				else if (aux->type == EXPRESSION)
				{
					context2 = 1;
					aux2 = aux + 1;
					while (context2)
					{
						if (aux2->type == EXPRESSION)
						{
							if (aux->exponent == 0.0)
								aux->value = aux2->value;
							if (operation == MUL)
							{
								aux->multiplier *= aux2->multiplier;
								aux->exponent += aux2->exponent;
								aux2->type = NOTHING;
								(aux2 - 1)->type = NOTHING;
								retval++;
							}
							else if (operation == DIV)
							{
								if (aux2->multiplier == 0.0)
									error_manager("A division by 0 was encountered.");
								aux->multiplier /= aux2->multiplier;
								aux->exponent -= aux2->exponent;
								aux2->type = NOTHING;
								(aux2 - 1)->type = NOTHING;
								retval++;
							}
							else
								error_manager("Unknown operation when trying to multiply expressions inside parentheses.");

						}
						else if (aux2->type == PARENTHESIS && get_parenthesis_type(aux2->value) == OPENING)
							aux2 = get_parenthesis_end(aux2 + 1) - 1;
						else if (aux2->type == PARENTHESIS && get_parenthesis_type(aux2->value) == CLOSING)
							context2 = 0;
						else if (aux2->type == THEEND || aux2->type == EQUAL)
							context2 = 0;
						operation = IDK;
						if (aux2->type == OPERATOR)
						{
							operation = aux2->value;
							if (operation == SUM || operation == SUB)
								break;
						}
						aux2++;
					}
				}
				else if (aux->type == EQUAL || aux->type == THEEND)
					context = 0;
				aux++;
			}
		}
		first = 0;
		if (items->type == EQUAL)
			first = 1;
		items++;
	}
	return (retval);
}

/*
*	Turns `2 * expression * 4` into `8 * expression`.
*/
t_group_item	*merge_multiplication_expression_and_parenthesis(t_group_item *items)
{
	t_group_item	*prev_expression = NULL;
	t_group_item	*parenthesis_end = NULL;
	t_group_item	*new_items;

	items = multiplybyone_safety(items);
	new_items = items;

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
					if ((parenthesis_end + 1)->multiplier == 0.0)
						error_manager("A division by 0 was encountered.");
					prev_expression->exponent -= (parenthesis_end + 1)->exponent;
					prev_expression->multiplier /= (parenthesis_end + 1)->multiplier;
				}
				(parenthesis_end + 1)->type = NOTHING;
				parenthesis_end->type = NOTHING;
			}
		}
		items++;
	}
	return (new_items);
}

void	apply_final_negatives(t_group_item *items)
{
	while (items->type != THEEND)
	{
		if (items->type == OPERATOR && items->value == SUB && (items + 1)->type == EXPRESSION)
		{
			items->value = SUM;
			(items + 1)->multiplier *= -1.0;
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
			if ((items + 1)->value == DIV)
			{
				if (!((items + 4)->type == PARENTHESIS && get_parenthesis_type((items + 4)->value) == CLOSING) || ((items + 3)->type == EXPRESSION && (items + 3)->exponent > items->exponent))
				{
					error_manager("A division was found that results in variables with negative exponents.");
				}

			}
			apply_to_all_members_in_parenthesis(*items, items + 3, (items + 1)->value == DIV);
			items->type = NOTHING;
			items++;
			items->type = NOTHING;
		}




		items++;
	}

}

t_group_item	*create_equation_copy(t_group_item *orig)
{
	t_group_item	*cpy = orig;
	t_group_item	*result;
	int				size = 1;

	while ((cpy++)->type != THEEND)
		size++;
	result = calloc(size, sizeof(t_group_item));
	if (!result)
		error_manager("calloc failed.");
//	memcpy(result, orig, size * sizeof(t_group_item));
	return (result);
}


//TODO		------- LEFT IT HERE -------
//			(x + 1)(x - 2) -> (x^2 + -2x + x + -2)
//!	PROBLEM	((x + 1) * (x + 2)) * (x + 1)   (maybe?)

t_group_item	*multiply_parentheses_by_themselves(t_group_item *items)
{
	t_group_item	*aux;
	t_group_item	*aux2;
	t_group_item	*aux2_iter;
	t_group_item	*copy = create_equation_copy(items);
	t_group_item	*auxcopy;
	int				context;
	int				context2;

	aux = items;
	auxcopy = copy;
	while (aux->type != THEEND)
	{
		auxcopy->exponent = aux->exponent;
		auxcopy->multiplier = aux->multiplier;
		auxcopy->type = aux->type;
		auxcopy->value = aux->value;
		auxcopy++;
		if (aux->type == PARENTHESIS && get_parenthesis_type(aux->value) == OPENING)
		{
			aux2 = get_parenthesis_end(aux + 1);
			if (aux2->type == OPERATOR && aux2->value == MUL && (aux2 + 1)->type == PARENTHESIS && get_parenthesis_type((aux2 + 1)->value) == OPENING)
			{
				aux++;
				aux2 += 2;
				context = 1;
				while (context)
				{
					if (aux->type == PARENTHESIS)
					{
						if (get_parenthesis_type(aux->value) == OPENING)
							context++;
						else
							context--;
					}
					else if (aux->type == EXPRESSION)
					{
						aux2_iter = aux2;
						context2 = 1;
						while (context2)
						{
							if (aux2_iter->type == PARENTHESIS)
							{
								if (get_parenthesis_type(aux2_iter->value) == OPENING)
									context2++;
								else
									context2--;
							}
							else if (aux2_iter->type == EXPRESSION)
							{
								auxcopy->type = EXPRESSION;
								if (aux->exponent > 0)
									auxcopy->value = aux->value;
								else
									auxcopy->value = aux2_iter->value;
								auxcopy->exponent = aux->exponent + aux2_iter->exponent;
								auxcopy->multiplier = aux->multiplier * aux2_iter->multiplier;
								auxcopy++;
								if (!((aux2_iter + 1)->type == PARENTHESIS && (aux + 1)->type == PARENTHESIS))
								{
									auxcopy->type = OPERATOR;
									auxcopy->value = SUM;
									auxcopy++;
								}
							}
							aux2_iter++;
						}
					}
					aux++;
				}
				aux = get_parenthesis_end(aux2) - 2;
			}
		}
		aux++;
	}
	auxcopy->type = THEEND;
	free(items);
	return (copy);
}


// t_group_item	product_of_long_expressions(t_group_item *items)
// {
// 	t_group_item	*items_cpy = items;
// 	t_group_item	*aux_items_cpy = items;
// 	t_group_item	*left

// 	while (items_cpy->type != THEEND)
// 	{
// 		if (items_cpy == PARENTHESIS)
// 		{
// 			aux_items_cpy = get_parenthesis_end(items_cpy + 1);
// 			if (aux_items_cpy->type == OPERATOR && (aux_items_cpy->value == MUL || aux_items_cpy->value == DIV) && (aux_items_cpy + 1)->type == PARENTHESIS && get_parenthesis_type((aux_items_cpy + 1)->value) == OPENING)
// 			{
// 				aux_items_cpy += 2;
// 			}
// 		}
// 		items_cpy++;
// 	}
// }


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
							error_manager("Operations other than + and - were found at this step.");
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
	else if (my_pow(params[1], 2) == 4.0 * params[2] * params[0])
		return (1);
	else if (my_pow(params[1], 2) < 4.0 * params[2] * params[0])
		return (-1);
	else
		return (2);
}

double get_second_degree_first_solution(double *params)
{
	return ((params[1] * -1.0 + my_sqrt(my_pow(params[1], 2.0) - 4 * params[2] * params[0])) / (2.0 * params[2]));
}


double get_second_degree_second_solution(double *params)
{
	return ((params[1] * -1.0 - my_sqrt(my_pow(params[1], 2.0) - 4 * params[2] * params[0])) / (2.0 * params[2]));
}


