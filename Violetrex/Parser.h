#pragma once
#include "Lexer.h"
#define ERROR_NODE -1
#define TOKEN 0
#define NUM_NODE 1
#define BIN_OP_NODE 2
#define UN_OP_NODE 3
#define VAR_ASSIGN_NODE 4
#define VAR_NODE 5
#define NULL_NODE 6
#define BOOLEAN_NODE 7
#define CONDITIONAL_NODE 8

#define CONDITIONAL_STATEMENT 0
#define BOOLEAN_STATEMENT 1
#define ADD_SUB 2
#define PROD_QUO 3

typedef struct NODE{
	int nodeType;
	void *left, *right, *else_block;
	int* block_lengths;
	Token* val;
	int leftType, rightType, valType, isVarNode;
}Node;

Node* construct_Node(Token* val, int nodeType){
	Node* node = (Node*)malloc(sizeof(Node));
	node->nodeType = nodeType;
	node->val = val;
	node->valType = val->type;
	return node;
}

Node* ConditionalNode(
	Node** conditional_statements, 
	Node*** blocks, int* block_lengths, int no_of_blocks,
	Token* val, Node** else_block, int else_block_length
){
	if(val->type == TT_IF){
		Node* node = construct_Node(val, CONDITIONAL_NODE);
		//conditional statements will be stored in the left and block on the right
		node->left = conditional_statements;
		node->leftType = no_of_blocks;
		node->right = blocks;
		node->block_lengths = block_lengths;
		node->rightType = no_of_blocks;
		node->else_block = else_block;
		//We'll use isVarNode for else block length
		node->isVarNode = else_block_length;
		return node;
	}
	return NULL;
}

Node* NullNode(Token* token)
{
	if(token->type == TT_NULL){
		return construct_Node(token, NULL_NODE);
	}
	return NULL;
}

Node* BooleanNode(Token* token)
{
	if(token->type == TT_TRUE || token->type == TT_FALSE){
		return construct_Node(token, BOOLEAN_NODE);
	}
	return NULL;
}

Node* NumNode(Token* token)
{
	if(token->type == TT_INT || token->type == TT_FLOAT){
		return construct_Node(token, NUM_NODE);
	}
	return NULL;
}

Node* BinOpNode(
	void* left, int leftType, Token* val,
	void* right, int rightType
)
{
	Node* node = construct_Node(val, BIN_OP_NODE);
	node->left = left;
	node->leftType = leftType;
	node->right = right;
	node->rightType = rightType;
	return node;
}

Node* UnOpNode(Token* token, void* right, int rightType)
{
	Node* node = construct_Node(token, UN_OP_NODE);
	node->right = right;
	node->rightType = rightType;
	return node;
}

Node* ErrorNode(Token* token)
{
	return construct_Node(token, ERROR_NODE);
}

Node* VarAssignNode(Token* left, Token* val, void* right, int rightType, int isVarNode)
{
	if(left->type != TT_VAR)
		return ErrorNode(
			make_error(
				SyntaxError("Variable name Eg. x", left->line_no, left->col_no),
				left->line_no, left->col_no
			)
		);
	
	Node* node = construct_Node(val, VAR_ASSIGN_NODE);
	node->left = left, node->leftType = left->type;
	node->right = right, node->rightType = rightType;
	node->isVarNode = isVarNode;
	return node;
}

Node* VarNode(Token* val)
{
	return val->type != TT_VAR ? ErrorNode(
			make_error(
				SyntaxError("Variable name Eg. x", val->line_no, val->col_no),
				val->line_no, val->col_no
			)
		) : construct_Node(val, VAR_NODE);
}

void expand_arrays(
	Node*** statements, Node**** blocks, int** block_lengths, int* no_of_blocks
){
	int old_no_of_blocks = *no_of_blocks;
	*no_of_blocks *= 2;
	Node** new_statements = (Node**)calloc(*no_of_blocks, sizeof(Node*));
	Node*** new_blocks = (Node***)calloc(*no_of_blocks, sizeof(Node**));
	int* new_block_lengths = (int*)calloc(*no_of_blocks, sizeof(int));
	memset(new_block_lengths, 0, *no_of_blocks * sizeof(int));
	Node** temp1 = *statements;
	Node*** temp2 = *blocks;
	int* temp3 = *block_lengths;

	memcpy(new_statements, *statements, old_no_of_blocks * sizeof(Node*));
	memcpy(new_blocks, *blocks, old_no_of_blocks * sizeof(Node**));
	memcpy(new_block_lengths, *block_lengths, old_no_of_blocks * sizeof(int));

	*statements = new_statements;
	*blocks = new_blocks;
	*block_lengths = new_block_lengths;
	free(temp1);
	free(temp2);
	free(temp3);
}

void expand_block(Node*** block, int* block_size){
	int old_block_size = *block_size;
	*block_size *= 2;
	Node** old_block = *block;
	Node** new_block = (Node**)calloc(*block_size, sizeof(Node*));
	memcpy(new_block, *block, old_block_size * sizeof(Node*));
	*block = new_block;
	free(old_block);
}

Node* Parser(Token** tokens, int size, int* curr_index, int isVarNode);

Node* addsub(Token** tokens, int size, int* curr_index);

Node* boolean_statement(Token** tokens, int size, int* curr_index);

Node* value(Token** tokens, int size, int* curr_index)
{
	if(*curr_index == TT_EOF || *curr_index == size)
		return ErrorNode(
			make_error(
				EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),
				tokens[size - 1]->line_no, tokens[size - 1]->col_no
			)
		);
	
	// print_token(tokens[*curr_index]);
	// printf(" -v \n");
	// printf("%d -> %d\n", tokens[*curr_index]->type, *curr_index);
	Node* node;
	switch(tokens[*curr_index]->type){
		case TT_LPAREN:{
			(*curr_index)++;
			if(*curr_index != TT_EOF && *curr_index != size){
				node = Parser(tokens, size, curr_index, 1); //addsub(tokens, size, curr_index);
				if(tokens[*curr_index]->type == TT_RPAREN){
					(*curr_index)++;
					return node;
				}
				else if(*curr_index == TT_EOF || *curr_index == size){
					return ErrorNode(
						make_error(
							EOFError(
								tokens[size - 1]->line_no, 
								tokens[size - 1]->col_no
							),
							tokens[size - 1]->line_no,
							tokens[size - 1]->col_no
						)
					);
				}
				else{
					return ErrorNode(
						make_error(
							SyntaxError(
								"')'", tokens[*curr_index]->line_no,
								tokens[*curr_index]->col_no
							), 
							tokens[*curr_index]->line_no, 
							tokens[*curr_index]->col_no
						)
					);
				}
			}
			else{
				return ErrorNode(
					make_error(
						EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),
						tokens[size - 1]->line_no, tokens[size - 1]->col_no
					)
				);
			}
			break;
		}
		case TT_INT:
			node = NumNode(tokens[*curr_index]);
			(*curr_index)++;
			// printf("Ye");
			return node;
			break;
		case TT_FLOAT:
			node = NumNode(tokens[*curr_index]);
			(*curr_index)++;
			// printf("Ye");
			return node;
			break;
		case TT_SUB:{
			Token* val = tokens[*curr_index];
			(*curr_index)++;
			if(*curr_index < size && *curr_index != TT_EOF){
				node = value(tokens, size, curr_index);

				if(node != NULL && node->nodeType != ERROR_NODE){
					return UnOpNode(val, node, node->nodeType);
				}
			}
			break;
		}
		case TT_VAR:{
			node = VarNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
			break;
		}
		case TT_NULL:{
			node = NullNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
			break;
		}
		case TT_NOT:{
			Token* val = tokens[*curr_index];
			(*curr_index)++;
			if(*curr_index < size && *curr_index != TT_EOF){
				node = value(tokens, size, curr_index);

				if(node != NULL && node->nodeType != ERROR_NODE){
					return UnOpNode(val, node, node->nodeType);
				}
			}
			break;
		}
		case TT_TRUE:{
			node = BooleanNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
			break;
		}
		case TT_FALSE:{
			node = BooleanNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
			break;
		}
		case TT_IF:{
			Token* val_token = tokens[*curr_index];
			int no_of_blocks = 4, count = 0, flag = 0;
			int inner_block_count = 0;
			Node** statements = (Node**)calloc(no_of_blocks, sizeof(Node*));
			Node*** blocks = (Node***)calloc(no_of_blocks, sizeof(Node**));
			int* block_lengths = (int*)calloc(no_of_blocks, sizeof(int));
			memset(block_lengths, 0, no_of_blocks * sizeof(int));
			Node* temp_statement;
			Node** temp_block;
			Node** else_block = NULL;
			int else_block_length = -1;

			while(
				*curr_index != TT_EOF && *curr_index < size && 
				(
					tokens[*curr_index]->type == TT_IF || 
					tokens[*curr_index]->type == TT_ELSEIF
				)
			){
				if(tokens[*curr_index]->type == TT_ELSEIF && flag == 0)
					return ErrorNode(
						make_error(
							SyntaxError(
								"if before elseif",
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->col_no : tokens[*curr_index]->col_no
							),
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						)
					);
				else if(tokens[*curr_index]->type == TT_IF && flag == 0)
					flag = 1;
				else if(tokens[*curr_index]->type == TT_IF){
					flag = 2;
					break;
				}
				
				(*curr_index)++;
				temp_statement = Parser(tokens, size, curr_index, 0);
				inner_block_count = 4;
				if(temp_statement->nodeType == ERROR_NODE)
					return temp_statement;
				else if(temp_statement->nodeType == VAR_ASSIGN_NODE)
					return ErrorNode(
						make_error(
							SyntaxError(
								"==", temp_statement->val->line_no,
								temp_statement->val->col_no
							),
							temp_statement->val->line_no, 
							temp_statement->val->col_no
						)
					);
				statements[count] = temp_statement;

				if(
					*curr_index == TT_EOF || *curr_index >= size || 
					tokens[*curr_index]->type != TT_BLOCK_OPEN
				)
					return ErrorNode(
						make_error(
							SyntaxError(
								"{",
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->col_no : tokens[*curr_index]->col_no
							),
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						)
					);
				else
					(*curr_index)++;
				
				temp_block = (Node**)calloc(inner_block_count, sizeof(Node*));
				while(
					*curr_index != TT_EOF && *curr_index < size && 
					tokens[*curr_index]->type != TT_BLOCK_CLOSE
				){
					temp_statement = Parser(tokens, size, curr_index, 0);
					temp_block[block_lengths[count]] = temp_statement;
					block_lengths[count]++;

					if(block_lengths[count] == inner_block_count)
						expand_block(&temp_block, &inner_block_count);
				}

				if(
					*curr_index == TT_EOF || *curr_index >= size || 
					tokens[*curr_index]->type != TT_BLOCK_CLOSE
				)
					return ErrorNode(
						make_error(
							SyntaxError(
								"}",
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->col_no : tokens[*curr_index]->col_no
							),
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						)
					);
				else
					(*curr_index)++;
				
				blocks[count] = temp_block;
				count++;

				if(count == no_of_blocks)
					expand_arrays(&statements, &blocks, &block_lengths, &no_of_blocks);
			}
			if(
				*curr_index != TT_EOF && *curr_index < size && 
				tokens[*curr_index]->type == TT_ELSE && flag != 2
			){
				(*curr_index)++;
				if(
					*curr_index == TT_EOF || *curr_index >= size || 
					tokens[*curr_index]->type != TT_BLOCK_OPEN
				)
					return ErrorNode(
						make_error(
							SyntaxError(
								"{",
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->col_no : tokens[*curr_index]->col_no
							),
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						)
					);
				else
					(*curr_index)++;
				
				inner_block_count = 4;
				else_block_length = 0;
				temp_block = (Node**)calloc(inner_block_count, sizeof(Node*));
				while(
					*curr_index != TT_EOF && *curr_index < size && 
					tokens[*curr_index]->type != TT_BLOCK_CLOSE
				){
					temp_statement = Parser(tokens, size, curr_index, 0);
					temp_block[else_block_length] = temp_statement;
					else_block_length++;

					if(else_block_length == inner_block_count)
						expand_block(&temp_block, &inner_block_count);
				}

				if(
					*curr_index == TT_EOF || *curr_index >= size || 
					tokens[*curr_index]->type != TT_BLOCK_CLOSE
				)
					return ErrorNode(
						make_error(
							SyntaxError(
								"}",
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
								*curr_index == TT_EOF || *curr_index >= size ? 
								tokens[size - 1]->col_no : tokens[*curr_index]->col_no
							),
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						)
					);
				else
					(*curr_index)++;
				
				else_block = temp_block;
			}

			return ConditionalNode(
				statements, blocks, block_lengths,
				no_of_blocks, val_token, else_block, else_block_length
			);

			break;
		}
		default:{
			return ErrorNode(
				make_error(
					IllegalCharacterError(
						tokens[*curr_index]->line_no, tokens[*curr_index]->col_no,
						1, (char*)tokens[*curr_index]->val
					),
					tokens[*curr_index]->line_no, tokens[*curr_index]->col_no
				)
			);
		}
	}
}

Node* prodquo(Token** tokens, int size, int* curr_index);

Node* operation(Token** tokens, int size, int* curr_index, int type_of_operation)
{
	if(*curr_index == TT_EOF || *curr_index == size){
		return ErrorNode(
			make_error(
				EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),
				tokens[size - 1]->line_no, tokens[size - 1]->col_no
			)
		);
	}
	
	int ops[100] = {0};
	int first_boolean_statement = 0;
	switch(type_of_operation)
	{
		case ADD_SUB:{
			ops[TT_ADD] = 1;
			ops[TT_SUB] = 1;
			break;
		}
		case PROD_QUO:{
			ops[TT_MUL] = 1;
			ops[TT_DIV] = 1;
			break;
		}
		case BOOLEAN_STATEMENT:{
			ops[TT_EQUALS] = 1;
			ops[TT_NOT_EQUALS] = 1;
			ops[TT_LESS_THAN] = 1;
			ops[TT_LESS_THAN_EQ] = 1;
			ops[TT_GREATER_THAN] = 1;
			ops[TT_GREATER_THAN_EQ] = 1;
			break;
		}
		case CONDITIONAL_STATEMENT:{
			ops[TT_AND] = 1;
			ops[TT_OR] = 1;
			break;
		}
	}

	Node *left = NULL, *right = NULL;
	int type;
	Token* val = NULL;
	switch(type_of_operation){
		case ADD_SUB:{
			left = prodquo(tokens, size, curr_index);
			break;
		}
		case PROD_QUO:{
			left = value(tokens, size, curr_index);
			break;
		}
		case BOOLEAN_STATEMENT:{
			left = addsub(tokens, size, curr_index);
			break;
		}
		case CONDITIONAL_STATEMENT:{
			left = boolean_statement(tokens, size, curr_index);
			break;
		}
	}
	// printf("Flag1 %d\n", T_OPERATOR_DETECTOR(tokens[*curr_index]->type));
	if(left->nodeType == ERROR_NODE)
		return left;

	while(
		*curr_index < size && *curr_index != TT_EOF && 
		T_OPERATOR_DETECTOR(tokens[*curr_index]->type) != -1 && 
		ops[tokens[*curr_index]->type] == 1 && 
		left->nodeType != ERROR_NODE
	){
		if(*curr_index == TT_EOF || *curr_index == size)
			return left;
		
		type = tokens[*curr_index]->type;
		if(ops[type] == 1)
			val = tokens[*curr_index];
		else
			return left;
		(*curr_index)++;
		switch(type_of_operation){
			case ADD_SUB:{
				right = prodquo(tokens, size, curr_index);
				break;
			}
			case PROD_QUO:{
				right = value(tokens, size, curr_index);
				break;
			}
			case BOOLEAN_STATEMENT:{
				if(first_boolean_statement == 0){
					ops[TT_EQUALS] = 0;
					ops[TT_NOT_EQUALS] = 0;
					ops[TT_LESS_THAN] = 0;
					ops[TT_LESS_THAN_EQ] = 0;
					ops[TT_GREATER_THAN] = 0;
					ops[TT_GREATER_THAN_EQ] = 0;
					first_boolean_statement = 1;
				}
				right = addsub(tokens, size, curr_index);
				break;
			}
			case CONDITIONAL_STATEMENT:{
				right = boolean_statement(tokens, size, curr_index);
				break;
			}
		}

		if(right->nodeType == ERROR_NODE)
			return right;

		left = BinOpNode((void*)left, left->nodeType, val, (void*)right, right->nodeType);
	}

	return left;
}

Node* prodquo(Token** tokens, int size, int* curr_index)
{
	return operation(tokens, size, curr_index, PROD_QUO);
}

Node* addsub(Token** tokens, int size, int* curr_index)
{
	return operation(tokens, size, curr_index, ADD_SUB);
}

Node* boolean_statement(Token** tokens, int size, int* curr_index){
	return operation(tokens, size, curr_index, BOOLEAN_STATEMENT);
}

Node* Parser(Token** tokens, int size, int* curr_index, int isVarNode)
{
	// printf("yay\n");
	int temp_index = *curr_index;
	if(tokens[*curr_index]->type == TT_VAR)
	{
		temp_index++;
		if(temp_index < size && IN_ASSIGNMENT_OPERATORS(tokens[temp_index]->type))
		{
			Token* left = tokens[*curr_index];
			(*curr_index)++;
			Token* val = tokens[*curr_index];
			(*curr_index)++;
			Node* right = addsub(tokens, size, curr_index);
			return right->nodeType == ERROR_NODE ? right : 
				VarAssignNode(left, val, right, right->nodeType, isVarNode);
		}
	}
	return operation(tokens, size, curr_index, CONDITIONAL_STATEMENT);
	// return addsub(tokens, size, curr_index);
}

void printNode(Node* node, int isTotal){
	if(node == NULL)
	{
		printf("\nNode was NULL\n");
		return;
	}
	
	switch(node->nodeType){
		case ERROR_NODE:
			print_token(node->val);
			break;
		case NUM_NODE:
			print_token(node->val);
			break;
		case VAR_NODE:
			print_token(node->val);
			break;
		case UN_OP_NODE:{
			printf("[");
			print_token(node->val);
			printf(" :: ");
			printNode(node->right, 1);
			printf("]");
			break;
		}
		case BIN_OP_NODE:{
			printf("[");
			printNode(node->left, 1);
			printf(" <- ");
			print_token(node->val);
			printf(" -> ");
			printNode(node->right, 1);
			printf("]");
			break;
		}
		case VAR_ASSIGN_NODE:{
			printf("[");
			print_token((Token*)node->left);
			printf(" <= ");
			print_token(node->val);
			printf(" => ");
			printNode(node->right, 1);
			printf("]");
			break;
		}
		default:{
			printf("[Node: %p]\n", node);
			return;
		}
	}
	if(isTotal == 0)
		printf("\n");
}