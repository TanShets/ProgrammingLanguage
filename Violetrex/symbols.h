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

#define TT_WHILE 30
#define TT_FOR 31
#define TT_TO 32
#define TT_BREAK 33
#define TT_CHANGE 34

#define TT_FUNCTION 35
#define TT_COMMA 36
#define TT_RETURN 37

#define TT_STRING 38

#define TT_EXPONENT 39

char* keywords_for_Violetrex_syntax[] = {
								"False", "None", "Null", "True", "and",
								"break", "change", "elif", "else",
								"elseif", "false", "fn", "for",
								"function", "if", "not", "null",
								"or", "return", "to", "true", "while"
							};
int keyword_token_for_Violetrex_syntax[] = {
								TT_FALSE, TT_NULL, TT_NULL, TT_TRUE, TT_AND,
								TT_BREAK, TT_CHANGE, TT_ELSEIF, TT_ELSE,
								TT_ELSEIF, TT_FALSE, TT_FUNCTION, TT_FOR,
								TT_FUNCTION, TT_IF, TT_NOT, TT_NULL,
								TT_OR, TT_RETURN, TT_TO, TT_TRUE, TT_WHILE
							};
#define KEYWORDS keywords_for_Violetrex_syntax
#define KEYWORDS_TOKEN keyword_token_for_Violetrex_syntax
#define KEYWORDS_SIZE 22

#define INPUT_FN -100
#define PRINT_FN -101
#define STRING_FN -102
#define INT_FN -103
#define FLOAT_FN -104

char* default_function_names[] = {
							"float", "input", "int", "print", "str", 
							"string"
						};

int default_function_codes[] = {
							FLOAT_FN, INPUT_FN, INT_FN, PRINT_FN, STRING_FN, 
							STRING_FN
						};
#define DEFAULT_FUNCTION_NAMES default_function_names
#define DEFAULT_FUNCTION_CODES default_function_codes
#define DEFAULT_FUNCTION_SIZE 6

int get_function_code(char* name, char* arr[], int start, int n){
	int mid = start + n / 2;
	int cmp;
	if(n == 1 && strcmp(arr[start], name) != 0)
		return -1;
	else if(name[0] == arr[mid][0]){
		cmp = strcmp(name, arr[mid]);
		switch(cmp){
			case 0:
				return mid;
			case 1:
				return get_function_code(name, arr, mid, n - n / 2);
			case -1:
				return get_function_code(name, arr, start, n / 2);
		}
	}
	else if(name[0] < arr[mid][0])
		return get_function_code(name, arr, start, n / 2);
	else
		return get_function_code(name, arr, mid, n - n / 2);
}

int function_code_getter(char* word){
	int index = get_function_code(word, DEFAULT_FUNCTION_NAMES, 0, DEFAULT_FUNCTION_SIZE);
	return index == -1 ? index : DEFAULT_FUNCTION_CODES[index];
}

#define GET_FUNCTION_CODE(x) function_code_getter(x)

int keyword_token(char* word){
	int index = get_function_code(word, KEYWORDS, 0, KEYWORDS_SIZE);
	return index == -1 ? index : KEYWORDS_TOKEN[index];
	// int i = 0;
	// while(i < KEYWORDS_SIZE && strcmp(word, KEYWORDS[i]) != 0)
	// 	i++;
	// return i < KEYWORDS_SIZE ? KEYWORDS_TOKEN[i] : -1;
}
/*
Hashed values are here
*/

// This is for function

#define STD_VAR_NAME_SIZE_LIMIT 30
#define STD_STRING_SIZE_LIMIT 16
#define ALPHABETS ""
#define IS_ALPHABET(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define IS_ALPHANUMERIC(c) (IS_ALPHABET(c) || (c >= '0' && c <= '9'))
#define IS_ALLOWED_IN_VAR_NAME(c) (IS_ALPHANUMERIC(c) || c == '_')

#define T_OPERATOR_SIZE 9
char T_OPERATOR_KEYS[T_OPERATOR_SIZE] = "+-*/=<>!^";

#define T_OPERATOR_REVERSE_SIZE 20
int T_OPERATOR_REVERSE_KEYS[] = {
	TT_ADD, TT_SUB, TT_MUL, TT_DIV,
	TT_EQ, TT_EQUALS, TT_NOT_EQUALS, 
	TT_LESS_THAN, TT_LESS_THAN_EQ, 
	TT_GREATER_THAN, TT_GREATER_THAN_EQ, 
	TT_INCREMENT, TT_DECREMENT,
	TT_PRODUCT_INCREMENT, TT_PRODUCT_DECREMENT,
	TT_AND, TT_OR, TT_NOT, TT_TO, TT_EXPONENT
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
		case '^':
			val = TT_EXPONENT;
			length = 1;
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

#define IS_EOA(index, size) (index == TT_EOF || index >= size)
// EOA = End of Array

void expand_file_data(char** line, int* filesize){
    int old_filesize = *filesize;
    *filesize *= 2;
    char* new_line = (char*)calloc(*filesize, sizeof(char));
    strncpy(new_line, *line, old_filesize);
    char* temp = *line;
    *line = new_line;
    free(temp);
}

void str_reverse(char* word){
	int n = strlen(word);
	char temp;
	for(int i = 0; i < n / 2; i++){
		temp = word[i];
		word[i] = word[n - i - 1];
		word[n - i - 1] = temp;
	}
}

char* num_to_str(int num){
	char* answer = (char*)calloc(10, sizeof(char));
	int curr_size = 0, max_size = 10;
	int temp_num = num, temp;
	while(temp_num > 0){
		temp = temp_num % 10;
		answer[curr_size] = (char)('0' + temp);
		curr_size++;
		if(curr_size == max_size)
			expand_file_data(&answer, &max_size);
		temp_num /= 10;
	}
	if(num == 0)
		answer[0] = '0';
	str_reverse(answer);
	return answer;
}

#define INT_TO_STR(x) num_to_str(x)