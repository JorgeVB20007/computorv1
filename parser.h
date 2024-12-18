#ifndef PARSER_H
# define PARSER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

#define PERIOD '.'

enum e_ops
{
	IDK,
	SUM,
	SUB,
	MUL,
	DIV,
	POW
};

enum e_types
{
	UNKNOWN,
	THEEND,
	NOTHING,
	NUMBER,
	VARIABLE,
	PARENTHESIS,
	OPERATOR,
	EQUAL,
	EXPRESSION
};

enum e_parenthesis_type
{
	OPENING,
	CLOSING
};


typedef struct s_group_item
{
	int		type;
	int		value;
	double	multiplier;
	double	exponent;
} t_group_item;


typedef struct s_item
{
	int		type;
	double	numvalue;
	int		value;
} t_item;

t_item *parser(char *line, char *variablechar);
void check_parentheses(char **input);
char get_variable_char(char *input);
void make_negatives(t_item *items);
t_item *append_multiplications(t_item *items);


void	typeprinter(t_item *types);
void	type_line_printer(t_item *types);
void	single_item_printer(t_item *type);
void	groupprinter(t_group_item *items);
void	bugprinter(t_group_item *items);
void	print_group_if_necessary(t_group_item *item, int old_changes, int new_changes);
void	single_groupprinter(t_group_item item);

int		is_operation(char c);
int		is_parenthesis(char c);
int		get_parenthesis_type(char c);
int		is_illegal(char c);
void	error_manager(const char *reason);
int		is_exponent_out_of_bounds(t_group_item *items);

t_group_item	*make_groups(t_item **items);
t_group_item	*merge_multiplication_expression_and_parenthesis(t_group_item *items);
//void			apply_operation_back(t_group_item *items);
t_group_item	*nothingness_cleanup(t_group_item *old);
void			multiply_expression_by_parenthesis(t_group_item *items);
t_group_item	*multiply_parentheses_by_themselves(t_group_item *items, int *something_changed);
void			eliminate_parentheses(t_group_item *target);
int				eliminate_lonely_parentheses(t_group_item *target);
void			add_everything_up(t_group_item *items);
double			*get_equation_values(t_group_item	*items);
void			apply_final_negatives(t_group_item *items);
int				merge_expressions_in_parentheses_sum_sub(t_group_item *items);
int				merge_expressions_in_parentheses_mul_div(t_group_item *items);
void			sort_items(t_group_item *items);
void			check_and_fix_illegal_divisions(t_group_item *items, int *something_changed);


int	get_second_degree_solutions(double *params);
double get_second_degree_first_solution(double *params);
double get_second_degree_second_solution(double *params);

void	result_formatter(double *params, char variablechar);

double	my_pow(double multiplier, int exponent);
double	my_sqrt(double number);

#endif
