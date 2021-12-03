#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TT_NULL -3
#define TT_ERROR -2
#define TT_EOF -1
#define TT_ADD 0
#define TT_SUB 1
#define TT_MUL 2
#define TT_DIV 3

#define TT_INT 4
#define TT_FLOAT 5

#define TT_EQ 6
#define TT_EQUALS 7
#define TT_NOT_EQUALS 8
#define TT_LESS_THAN 9
#define TT_LESS_THAN_EQ 10
#define TT_GREATER_THAN 11
#define TT_GREATER_THAN_EQ 12

#define TT_LPAREN 13
#define TT_RPAREN 14

#define TT_INCREMENT 15
#define TT_DECREMENT 16
#define TT_PRODUCT_INCREMENT 17
#define TT_PRODUCT_DECREMENT 18

#define TT_VAR 19
#define TT_IF 20
#define TT_ELSEIF 21
#define TT_ELSE 22

#define TT_BLOCK_OPEN 23
#define TT_BLOCK_CLOSE 24

#define TT_TRUE 25
#define TT_FALSE 26
#define TT_AND 27
#define TT_OR 28
#define TT_NOT 29

char* keywords_for_Violetrex_syntax[] = {
								"if", "elseif", "elif", "else",
								"True", "False", "true", "false",
								"null", "None", "Null", "and", "or",
								"not"
							};
int keyword_token_for_Violetrex_syntax[] = {
								TT_IF, TT_ELSEIF, TT_ELSEIF, TT_ELSE,
								TT_TRUE, TT_FALSE, TT_TRUE, TT_FALSE,
								TT_NULL, TT_NULL, TT_NULL, TT_AND, TT_OR,
								TT_NOT
							};
#define KEYWORDS keywords_for_Violetrex_syntax
#define KEYWORDS_TOKEN keyword_token_for_Violetrex_syntax
#define KEYWORDS_SIZE 14

int keyword_token(char* word){
	int i = 0;
	while(i < KEYWORDS_SIZE && strcmp(word, KEYWORDS[i]) != 0)
		i++;
	return i < KEYWORDS_SIZE ? KEYWORDS_TOKEN[i] : -1;
}
/*
Hashed values are here
*/

// This is for function

#define STD_VAR_NAME_SIZE_LIMIT 30
#define ALPHABETS ""
#define IS_ALPHABET(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define IS_ALPHANUMERIC(c) (IS_ALPHABET(c) || (c >= '0' && c <= '9'))
#define IS_ALLOWED_IN_VAR_NAME(c) (IS_ALPHANUMERIC(c) || c == '_')

#define T_OPERATOR_SIZE 8
char T_OPERATOR_KEYS[T_OPERATOR_SIZE] = "+-*/=<>!";

#define T_OPERATOR_REVERSE_SIZE 18
int T_OPERATOR_REVERSE_KEYS[] = {
	TT_ADD, TT_SUB, TT_MUL, TT_DIV,
	TT_EQ, TT_EQUALS, TT_NOT_EQUALS, 
	TT_LESS_THAN, TT_LESS_THAN_EQ, 
	TT_GREATER_THAN, TT_GREATER_THAN_EQ, 
	TT_INCREMENT, TT_DECREMENT,
	TT_PRODUCT_INCREMENT, TT_PRODUCT_DECREMENT,
	TT_AND, TT_OR, TT_NOT
};

#define IN_ASSIGNMENT_OPERATORS(x) ( \
	( \
		x == TT_EQ || x == TT_INCREMENT || x == TT_DECREMENT || \
		x == TT_PRODUCT_INCREMENT || x == TT_PRODUCT_DECREMENT \
	) \
)

#define IN_BOOLEAN_OPERATORS(x) ( \
	x == TT_EQUALS || x == TT_NOT_EQUALS || x == TT_LESS_THAN || \
	x == TT_LESS_THAN_EQ || x == TT_GREATER_THAN || x == TT_GREATER_THAN_EQ \
)

#define IN_NULL_OPERATORS(x) ( \
	x == TT_EQUALS || x == TT_NOT_EQUALS || \
	x == TT_AND || x == TT_OR || x == TT_NOT \
)

#define IN_CONDITIONAL_OPERATORS(x) (x == TT_AND || x == TT_OR)

int T_OPERATOR_REVERSE_DETECTION(int num, int start, int length)
{
	int mid = start + (length / 2);
	// if(num == TT_MUL)
	// 	printf("Mid here: %d -> %d vs %d, l = %d\n", mid, T_OPERATOR_REVERSE_KEYS[mid], num, length);
	
	if(T_OPERATOR_REVERSE_KEYS[mid] != num && (length == 1 || length == 0))
		return -1;

	if(num == T_OPERATOR_REVERSE_KEYS[mid])
		return mid;
	else if(num < T_OPERATOR_REVERSE_KEYS[mid]){
		//printf("%d vs %d\n", num, T_OPERATOR_REVERSE_KEYS[mid]);
		return T_OPERATOR_REVERSE_DETECTION(num, start, length / 2);
	}
	else
		return T_OPERATOR_REVERSE_DETECTION(num, mid + 1, length / 2);
}

#define T_OPERATOR_DETECTOR(num) T_OPERATOR_REVERSE_DETECTION(num, 0, T_OPERATOR_REVERSE_SIZE)

char T_SPACING_KEYS[3] = " \n";

int T_OPERATOR(char* c, int length)
{
	int val, added_val;
	switch(*c)
	{
		case '+':
			val = TT_ADD;
			added_val = 15;
			break;
		case '-':
			val = TT_SUB;
			added_val = 15;
			break;
		case '*':
			val = TT_MUL;
			added_val = 15;
			break;
		case '/':
			val = TT_DIV;
			added_val = 15;
			break;
		case '=':
			val = TT_EQ;
			added_val = 1;
			break;
		case '<':
			val = TT_LESS_THAN;
			added_val = 1;
			break;
		case '>':
			val = TT_GREATER_THAN;
			added_val = 1;
			break;
		case '!':
			val = 0;
			added_val = TT_NOT_EQUALS;
			break;
		default:
			return TT_EOF;
	}

	if(length > 1)
	{
		return c[1] == '=' ? val + added_val : TT_EOF;
	}

	return val;
}

int T_BRACKET(char c){
	switch(c){
		case '(':
			return TT_LPAREN;
		case ')':
			return TT_RPAREN;
		case '{':
			return TT_BLOCK_OPEN;
		case '}':
			return TT_BLOCK_CLOSE;
		default:
			return TT_EOF;
	}
}

char* T_OPERATOR_REVERSE(int n){
	char* op = (char*)calloc(5, sizeof(char));
	switch(n){
		case TT_ADD:
			op[0] = '+', op[1] = '\0';
			break;
		case TT_SUB:
			op[0] = '-', op[1] = '\0';
			break;
		case TT_MUL:
			op[0] = '*', op[1] = '\0';
			break;
		case TT_DIV:
			op[0] = '/', op[1] = '\0';
			break;
		case TT_EQ:
			op[0] = '=', op[1] = '\0';
			break;
		case TT_INCREMENT:
			op[0] = '+', op[1] = '=', op[2] = '\0';
			break;
		case TT_DECREMENT:
			op[0] = '-', op[1] = '=', op[2] = '\0';
			break;
		case TT_PRODUCT_INCREMENT:
			op[0] = '*', op[1] = '=', op[2] = '\0';
			break;
		case TT_PRODUCT_DECREMENT:
			op[0] = '/', op[1] = '=', op[2] = '\0';
			break;
		case TT_EQUALS:
			op[0] = '=', op[1] = '=', op[2] = '\0';
			break;
		case TT_NOT_EQUALS:
			op[0] = '!', op[1] = '=', op[2] = '\0';
			break;
		case TT_LESS_THAN:
			op[0] = '<', op[1] = '\0';
			break;
		case TT_LESS_THAN_EQ:
			op[0] = '<', op[1] = '=', op[2] = '\0';
			break;
		case TT_GREATER_THAN:
			op[0] = '>', op[1] = '\0';
			break;
		case TT_GREATER_THAN_EQ:
			op[0] = '>', op[1] = '=', op[2] = '\0';
			break;
	}
	return op;
}

void move(int* line_no, int* col_no, char* line, int* index, int length)
{
	if(*index == length - 1){
		*index = TT_EOF;
		return;
	}

	if(line[*index] == '\n'){
		(*line_no)++;
		*col_no = 0;
	}

	(*col_no)++;
	(*index)++;
}