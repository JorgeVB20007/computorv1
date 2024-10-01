#include "parser.h"

static char	operator_symbol(int	num)
{
	if (num == SUM)
		return ('+');
	else if (num == SUB)
		return ('-');
	else if (num == MUL)
		return ('*');
	else if (num == DIV)
		return ('/');
	else if (num == POW)
		return ('^');
	else
		return ('?');
}

void	typeprinter(t_item *types)
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
		else if (types[idx].type == NOTHING)
			printf("\033[0;90m> Type: Nothing\033[0m\n");
		else
			printf(">> Weird type: %d | Value: %d, NValue: %f", types[idx].type, types[idx].value, types[idx].numvalue);
		idx++;
	}
	if (types[idx].type == THEEND)
		printf("> Type: \033[1;97m----- The End -----\033[0m\n\n");
}

void single_item_printer(t_item *type)
{
	if (type->type == NUMBER)
		printf("> Type: \033[0;33mNBR\033[0m | Value: %f <", type->numvalue);
	else if (type->type == PARENTHESIS)
		printf("> Type: \033[0;34mPAR\033[0m | Value: %c <", type->value);
	else if (type->type == OPERATOR)
	{
		if (type->value == SUM)
			printf("> Type: \033[0;32mOPR\033[0m | Value: SUM (+) <");
		else if (type->value == SUB)
			printf("> Type: \033[0;32mOPR\033[0m | Value: SUB (-) <");
		else if (type->value == MUL)
			printf("> Type: \033[0;32mOPR\033[0m | Value: MUL (*) <");
		else if (type->value == DIV)
			printf("> Type: \033[0;32mOPR\033[0m | Value: DIV (/) <");
		else if (type->value == POW)
			printf("> Type: \033[0;32mOPR\033[0m | Value: POW (^) <");
		else
			printf("> Type: \033[0;32mOPR\033[0m | Value: IDK (?) <");
	}
	else if (type->type == VARIABLE)
		printf("> Type: \033[0;35mVAR\033[0m | Value: %c <", type->value);
	else if (type->type == EQUAL)
		printf("> Type: \033[1;36mEQL\033[0m | Value: = <");
	else if (type->type == NOTHING)
		printf("\033[0;90m> Type: Nothing\033[0m <");
	else if (type->type == THEEND)
		printf("> Type: \033[1;97mThe End\033[0m <");
	else
		printf(">> Weird type: %d | Value: %d, NValue: %f <<", type->type, type->value, type->numvalue);
}


void	groupprinter(t_group_item *items)
{
	int	idx = 0;

	while (items[idx].type != THEEND)
	{
		if (items[idx].type == UNKNOWN)
			printf("\033[0;90m(unknown) ");
		else if (items[idx].type == NOTHING)
			printf("\033[0;90m(nothing) ");
		else if (items[idx].type == NUMBER)
			printf("\033[0;31m(number) ");
		else if (items[idx].type == VARIABLE)
			printf("\033[0;31m(variable) ");
		else if (items[idx].type == PARENTHESIS)
		{
			printf("\033[0;34m");
			if (get_parenthesis_type(items[idx].value) == OPENING)
				printf("(");
			else
				printf(")");
		}
		else if (items[idx].type == OPERATOR)
			printf("\033[0;32m%c", operator_symbol(items[idx].value));
		else if (items[idx].type == EQUAL)
			printf("\033[1;36m=");
		else if (items[idx].type == EXPRESSION)
		{
			printf("\033[0;33m");
			if (items[idx].multiplier == -1.0 && items[idx].exponent != 0.0)
				printf("-");
			else if (items[idx].multiplier != 1.0)
				printf("%g", items[idx].multiplier);
			else if (items[idx].exponent == 0.0)
				printf("%g", items[idx].multiplier);
			if (items[idx].exponent > 1.0)
				printf("%c^%g", items[idx].value, items[idx].exponent);
			else if (items[idx].exponent == 1.0)
				printf("%c", items[idx].value);
		}
		printf("\033[0m ");
		idx++;
	}
	printf("\n");
}

void	single_groupprinter(t_group_item item)
{
	if (item.type == UNKNOWN)
		printf("\033[0;90m(unknown) ");
	else if (item.type == NOTHING)
		printf("\033[0;90m(nothing) ");
	else if (item.type == NUMBER)
		printf("\033[0;31m(number) ");
	else if (item.type == VARIABLE)
		printf("\033[0;31m(variable) ");
	else if (item.type == PARENTHESIS)
	{
		printf("\033[0;34m");
		if (get_parenthesis_type(item.value) == OPENING)
			printf("(");
		else
			printf(")");
	}
	else if (item.type == OPERATOR)
		printf("\033[0;32m%c", operator_symbol(item.value));
	else if (item.type == EQUAL)
		printf("\033[1;36m=");
	else if (item.type == EXPRESSION)
	{
		printf("\033[0;33m");
		if (item.multiplier == -1.0 && item.exponent != 0.0)
			printf("-");
		else if (item.multiplier != 1.0)
			printf("%g", item.multiplier);
		else if (item.exponent == 0.0)
			printf("%g", item.multiplier);
		if (item.exponent > 1.0)
			printf("%c^%g", item.value, item.exponent);
		else if (item.exponent == 1.0)
			printf("%c", item.value);
	}
	printf("\033[0m ");

}
