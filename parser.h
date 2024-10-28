#ifndef PARSER_H
# define PARSER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

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
void	multiply_expression_by_parenthesis(t_group_item *items);
void	eliminate_parentheses(t_group_item *target);
void	eliminate_lonely_parentheses(t_group_item *target);
void	add_everything_up(t_group_item *items);
double	*get_equation_values(t_group_item	*items);
void	apply_final_negatives(t_group_item *items);

int	get_second_degree_solutions(double *params);
double get_second_degree_first_solution(double *params);
double get_second_degree_second_solution(double *params);

void	result_formatter(double *params, char variablechar);

#endif
