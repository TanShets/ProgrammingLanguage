#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "symbols.h"

#define SIZE 10000

typedef struct TOKEN{
	void* val;
	int type;
	int line_no, col_no;
} Token;

void print_token(Token* token)
{
	// if(token->type == TT_MUL)
	// 	printf("Type: %d %d\n", token->type, T_OPERATOR_DETECTOR(token->type));
	if(
		T_OPERATOR_DETECTOR(token->type) != -1 || 
		token->type == TT_LPAREN || token->type == TT_RPAREN
	)
		printf("(%d -> '%s')", token->type, (char*)token->val);
	else{
		switch(token->type)
		{
			case TT_INT:{
				int* temp = (int*)token->val;
				printf("(%d -> %d)", token->type, *temp);
				break;
			}
			case TT_FLOAT:{
				double* temp = (double*)token->val;
				printf("(%d -> %lf)", token->type, *temp);
				break;
			}
			default:
				printf("(%d)", token->type);
		}
	}
}

void print_token_array(Token** tokens, int length)
{
	printf("[");
	for(int i = 0; i < length; i++)
	{
		print_token(tokens[i]);
		if(i < length - 1)
			printf(", ");
	}
	printf("]\n");
}

void* str_to_num(char* line, int start, int end, bool isFloat)
{
	int end_pos = end != TT_EOF ? end : strlen(line);
	int length = end_pos - start;
	int factor = length - 1;
	if(isFloat)
	{
		double* answer = (double*)malloc(sizeof(double));
		double num = 0;
		int index = start + 1;
		while(index < end_pos && line[index] != '.')
			index++;
		factor = index - start - 1;
		num = 0;
		for(int i = start; i < end_pos; i++)
		{
			//printf("%c\n", line[i]);
			if(line[i] == '.')
			{
				factor = -1;
			}
			else
			{
				//printf("%lf %lf\n", num, (double)((line[i] - '0') * pow((double)10, (double)factor)));
				num = num + (double)((line[i] - '0') * pow((double)10, (double)factor));
				//printf("NUm: %f\n", *num);
				factor--;
			}
		}

		//printf("Answer: %lf\n", num);
		*answer = num;
		return answer;
	}
	else
	{
		int* num = (int*)malloc(sizeof(int));
		*num = 0;
		for(int i = start; i < end_pos; i++)
		{
			*num += (int)((line[i] - '0') * pow(10, factor));
			factor--;
		}
		return num;
	}
}

Token* make_number(char* line, int* start, int* line_no, int* col_no)
{
	Token* token = (Token*)malloc(sizeof(Token));
	int init_start = *start;
	bool isCorrect = true;
	int length = strlen(line);
	//int end = *start + 1;
	bool isFloat = false;

	if(line[*start] < '0' || line[*start] > '9'){
		token->val = NULL, token->type = TT_EOF;
		return token;
	}

	//(*start)++;
	move(line_no, col_no, line, start, length);

	while(
		*start < strlen(line) && 
		(
			(line[*start] >= '0' && line[*start] <= '9') || 
			(line[*start] == '.' && !isFloat)
		)
	)
	{
		if(line[*start] == '.')
			isFloat = true;
		//(*start)++;
		move(line_no, col_no, line, start, length);
	}
	
	void* num = str_to_num(line, init_start, *start, isFloat);
	//*start = end;
	//printf("Flag4\n");
	token->val = num, token->type = isFloat ? TT_FLOAT : TT_INT;
	//printf("Flag4\n");
	return token;
}

Token* make_operator(char* line, int* start, int* line_no, int* col_no)
{
	int end = *start + 1;
	char* operator = (char*)malloc(2 * sizeof(char));
	operator[0] = line[*start];
	int length = 1;
	if(end < strlen(line) && strchr(T_OPERATOR_KEYS, line[end]) != NULL)
	{
		operator[1] = line[end];
		length++;
	}

	Token* token = (Token*)malloc(sizeof(Token));
	token->val = operator, token->type = T_OPERATOR(operator, length);
	*start += length;
	return token;
}

void delete_array_of_pointers(void** arr, int t_size)
{
	free(arr);
}

void expand_tokens(Token*** tokens, int* initial_size)
{
	Token** new_token_array = (Token**)calloc(2 * (*initial_size), sizeof(Token*));
	
	Token** temp_space = *tokens;
	memcpy(new_token_array, *tokens, *initial_size * sizeof(Token*));

	*tokens = new_token_array;
	delete_array_of_pointers((void**)temp_space, *initial_size);
	*initial_size *= 2;
}

Token** Lexer(char* line, int* curr_size, int* t_size, int* line_no, int* col_no)
{
	Token** tokens = (Token**)calloc(SIZE, sizeof(Token*));
	*t_size = SIZE;
	int length = strlen(line);
	int i = 0;
	bool isFloatNum = false;
	char* temp_char;
	Token temp_token;
	Token** temp_ptr;
	//printf("1\n");
	while(i != TT_EOF)
	{
		//printf("i = %d, char = '%c'\n", i, line[i]);

		if(line[i] >= '0' && line[i] <= '9'){
			tokens[*curr_size] = make_number(line, &i, line_no, col_no);
			(*curr_size)++;
		}
		else if(strchr(T_OPERATOR_KEYS, line[i]) != NULL){
			//printf("flag2 '%c' %d\n", *strchr(T_OPERATOR_KEYS, line[i]), i);
			tokens[*curr_size] = make_operator(line, &i, line_no, col_no);
			(*curr_size)++;
		}
		else if(line[i] == '(' || line[i] == ')'){
			tokens[*curr_size] = (Token*)malloc(sizeof(Token));
			temp_char = (char*)malloc(sizeof(char));
			*temp_char = line[i];

			tokens[*curr_size]->val = temp_char;
			tokens[*curr_size]->type = T_PARENTHESIS(line[i]);
			move(line_no, col_no, line, &i, length);
			(*curr_size)++;
		}
		else if(strchr(T_SPACING_KEYS, line[i]) != NULL){
			//printf("flag3\n");
			move(line_no, col_no, line, &i, length);
		}
		else
			return NULL;

		if(*curr_size == *t_size){
			temp_ptr = &tokens[0];
			expand_tokens(&temp_ptr, t_size);
		}
	}

	return tokens;
}