#pragma once
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "symbols.h"
#include "errors.h"

#define SIZE 10000

typedef struct TOKEN{
	void* val;
	int type;
	int isSingle;
	int line_no, col_no;
} Token;

void print_token(Token* token)
{
	if(
		T_OPERATOR_DETECTOR(token->type) != -1 &&  
		(token->type == TT_LPAREN || token->type == TT_RPAREN)
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
			case TT_VAR:{
				char* temp = (char*)token->val;
				printf("(%d -> '%s')", token->type, temp);
				break;
			}
			case TT_ERROR:{
				printError((Error*)token->val);
				break;
			}
			case TT_IF:{
				char* temp = (char*)token->val;
				printf("(%d -> '%s')", token->type, temp);
				break;
			}
			case TT_ELSEIF:{
				char* temp = (char*)token->val;
				printf("(%d -> '%s')", token->type, temp);
				break;
			}
			case TT_ELSE:{
				char* temp = (char*)token->val;
				printf("(%d -> '%s')", token->type, temp);
				break;
			}
			case TT_TRUE:{
				char* temp = (char*)token->val;
				printf("(%d -> %s)", token->type, temp);
				break;
			}
			case TT_FALSE:{
				char* temp = (char*)token->val;
				printf("(%d -> %s)", token->type, temp);
				break;
			}
			case TT_NULL:{
				char* temp = (char*)token->val;
				printf("(%d -> %s)", token->type, temp);
				break;
			}
			case TT_STRING:{
				char* temp = (char*)token->val;
				printf("(%d -> %s)", token->type, temp);
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

Token* make_error(Error* error, int line_no, int col_no);

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
		token->line_no = *line_no, token->col_no = *col_no;
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
	token->line_no = *line_no, token->col_no = *col_no;
	//printf("Flag4\n");
	return token;
}

Token* make_operator(char* line, int* start, int* line_no, int* col_no)
{
	int end = *start + 1;
	char* operator = (char*)malloc(3 * sizeof(char));
	operator[0] = line[*start];
	int length = 1;
	if(end < strlen(line) && strchr(T_OPERATOR_KEYS, line[end]) != NULL)
	{
		operator[1] = line[end];
		length++;
	}
	operator[length] = '\0';
	Token* token = (Token*)malloc(sizeof(Token));
	token->val = operator, token->type = T_OPERATOR(operator, length);
	token->line_no = *line_no, token->col_no = *col_no;
	*start += length;
	return token;
}

void expand_string(char** s, int curr_size, int* max_size){
	// char* new_string = (char*)calloc(curr_size * 2, sizeof(char));
	*s = (char*)realloc(*s, 2 * curr_size * sizeof(char));
	// memcpy(new_string, *s, curr_size * sizeof(char));
	// char* temp = *s;
	// *s = new_string;
	// free(temp);
	*max_size *= 2;
}

Token* make_variable(char* line, int* start, int* line_no, int* col_no)
{
	char* word = (char*)calloc(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
	int length = strlen(line);
	int temp_length = 1;
	int max_length = STD_VAR_NAME_SIZE_LIMIT;
	char temp_char;
	if(IS_ALPHABET(line[*start]))
		word[0] = line[*start];
	else{
		temp_char = line[*start];
		return make_error(
			IllegalCharacterError(*line_no, *col_no, 1, &temp_char),
			*line_no, *col_no
		);
	}

	move(line_no, col_no, line, start, length);

	while(*start < length && *start != TT_EOF && IS_ALLOWED_IN_VAR_NAME(line[*start])){
		word[temp_length] = line[*start];
		temp_length++;
		if(temp_length >= max_length)
			expand_string(&word, temp_length, &max_length);
		move(line_no, col_no, line, start, length);
	}
	Token* token = (Token*)malloc(sizeof(Token));
	int token_val = keyword_token(word);
	// printf("Flag 2\n");
	token->val = word, token->line_no = *line_no;
	token->type = TT_VAR, token->col_no = *col_no;
	if(token_val != -1)
		token->type = token_val;
	return token;
}

Token* make_error(Error* error, int line_no, int col_no){
	Token* token = (Token*)malloc(sizeof(Token));
	token->val = error, token->type = TT_ERROR;
	token->line_no = line_no, token->col_no = col_no;
	return token;
}

Token* make_string(char* line, int* start, int* line_no, int* col_no){
	char* word = (char*)calloc(STD_STRING_SIZE_LIMIT, sizeof(char));
	int length = strlen(line);
	int temp_length = 0;
	int max_length = STD_STRING_SIZE_LIMIT;
	char temp_char;
	if(line[*start] != '\'' && line[*start] != '"'){
		char* unexpected_val = (char*)malloc(sizeof(char));
		*unexpected_val = line[*start];
		return make_error(
			IllegalCharacterError(
				*line_no, *col_no, 2, unexpected_val, "'\""
			),
			*line_no, *col_no
		);
	}

	char stopper = line[*start];
	int isBackslashed = 0;

	move(line_no, col_no, line, start, length);

	while(
		*start < length && *start != TT_EOF && 
		(line[*start] != stopper || isBackslashed)
	){
		if(line[*start] == '\\')
			isBackslashed = 1;
		else if(line[*start] == stopper)
			isBackslashed = 0;
		word[temp_length] = line[*start];
		temp_length++;
		if(temp_length >= max_length){
			expand_string(&word, temp_length, &max_length);
		}
		move(line_no, col_no, line, start, length);
	}

	word[temp_length] = '\0';

	if(*start == length || *start == TT_EOF)
		return make_error(EOFError(*line_no, *col_no), *line_no, *col_no);
	
	Token* token = (Token*)malloc(sizeof(Token));
	// printf("Flag 2\n");
	token->val = word, token->line_no = *line_no;
	token->type = TT_VAR, token->col_no = *col_no;
	token->isSingle = stopper == '\'' ? 1 : 2;
	token->type = TT_STRING;
	move(line_no, col_no, line, start, length);
	return token;
}

void delete_array_of_pointers(void** arr, int t_size)
{
	free(arr);
}

void expand_tokens(Token*** tokens, int* initial_size)
{
	// Token** new_token_array = (Token**)calloc(2 * (*initial_size), sizeof(Token*));
	
	// Token** temp_space = *tokens;
	// memcpy(new_token_array, *tokens, *initial_size * sizeof(Token*));

	// *tokens = new_token_array;
	// delete_array_of_pointers((void**)temp_space, *initial_size);
	*tokens = (Token**)realloc(*tokens, 2 * (*initial_size) * sizeof(Token*));
	*initial_size *= 2;
}

Token** Lexer(char* line, int* curr_size, int* t_size, int* line_no, int* col_no)
{
	Token** tokens = (Token**)calloc(SIZE, sizeof(Token*));
	Token* temp_token;
	Error* error;
	*t_size = SIZE;
	int length = strlen(line);
	int i = 0;
	bool isFloatNum = false;
	char* temp_char;
	Token** temp_ptr;
	//printf("1\n");
	char c;
	int count = 0;
	while(i != TT_EOF && i < length)
	{
		c = line[i];

		if(c >= '0' && c <= '9'){
			tokens[*curr_size] = make_number(line, &i, line_no, col_no);
			(*curr_size)++;
		}
		else if(strchr(T_OPERATOR_KEYS, c) != NULL && c != '\0'){
			tokens[*curr_size] = make_operator(line, &i, line_no, col_no);
			(*curr_size)++;
		}
		else if(
			c == '(' || c == ')' || c == '{' || c == '}' || 
			c == '[' || c == ']'
		){
			tokens[*curr_size] = (Token*)malloc(sizeof(Token));
			temp_char = (char*)malloc(sizeof(char));
			*temp_char = line[i];
			tokens[*curr_size]->val = temp_char;
			tokens[*curr_size]->type = T_BRACKET(line[i]);
			tokens[*curr_size]->line_no = *line_no;
			tokens[*curr_size]->col_no = *col_no;
			move(line_no, col_no, line, &i, length);
			(*curr_size)++;
		}
		else if(IS_ALPHABET(c)){
			temp_token = make_variable(line, &i, line_no, col_no);
			if(temp_token->type == TT_ERROR)
			{
				temp_ptr = (Token**)malloc(sizeof(Token*));
				*temp_ptr = temp_token;
				return temp_ptr;
			}
			else{
				tokens[*curr_size] = temp_token;
				(*curr_size)++;
			}
		}
		else if(strchr(T_SPACING_KEYS, c) != NULL && c != '\0'){
			move(line_no, col_no, line, &i, length);
		}
		else if(c == ','){
			tokens[*curr_size] = (Token*)malloc(sizeof(Token));
			temp_char = (char*)malloc(sizeof(char));
			*temp_char = line[i];
			tokens[*curr_size]->val = temp_char;
			tokens[*curr_size]->type = TT_COMMA;
			tokens[*curr_size]->line_no = *line_no;
			tokens[*curr_size]->col_no = *col_no;
			move(line_no, col_no, line, &i, length);
			(*curr_size)++;
		}
		else if(c == '\'' || c == '"'){
			temp_token = make_string(line, &i, line_no, col_no);
			if(temp_token->type == TT_ERROR)
			{
				temp_ptr = (Token**)malloc(sizeof(Token*));
				*temp_ptr = temp_token;
				return temp_ptr;
			}
			else{
				tokens[*curr_size] = temp_token;
				(*curr_size)++;
			}
		}
		else{
			if(curr_size != NULL){
				curr_size = (int*)malloc(sizeof(int));
				*curr_size = 1;
			}
			error = IllegalCharacterError(*line_no, *col_no, 1, &line[i]);
			temp_token = make_error(error, *line_no, *col_no);
			temp_ptr = (Token**)malloc(sizeof(Token*));
			*temp_ptr = temp_token;
			return temp_ptr;
		}

		if(*curr_size == *t_size){
			temp_ptr = &tokens[0];
			expand_tokens(&temp_ptr, t_size);
		}
	}

	return tokens;
}