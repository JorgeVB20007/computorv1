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
	EQUAL
};

typedef struct s_expression
{
	float	value;
	int		degree;
	int		next_operation;
	int		prev_operation;
	struct s_expression *inside;
	struct s_expression *prev;
	struct s_expression *next;
} t_expression;

// char	legal_chars = {'+', '-', '*', '/', '.', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''}

typedef struct s_item
{
	int		type;
	double	numvalue;
	int		value;
} t_item;

#endif
