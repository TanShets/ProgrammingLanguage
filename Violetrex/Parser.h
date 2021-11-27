#pragma once
#include "Lexer.h"
#define ERROR_NODE -1
#define TOKEN 0
#define NUM_NODE 1
#define BIN_OP_NODE 2
#define UN_OP_NODE 3
#define VAR_ASSIGN_NODE 4
#define VAR_NODE 5

#define ADD_SUB 0
#define PROD_QUO 1

typedef struct NODE{
	int nodeType;
	void *left, *right;
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

Node* Parser(Token** tokens, int size, int* curr_index, int isVarNode);

Node* addsub(Token** tokens, int size, int* curr_index);

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
	if(tokens[*curr_index]->type == TT_LPAREN){
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
	}
	else if(
		tokens[*curr_index]->type == TT_INT || 
		tokens[*curr_index]->type == TT_FLOAT
	){
		// printf("Ye");
		node = NumNode(tokens[*curr_index]);
		(*curr_index)++;
		// printf("Ye");
		return node;
	}
	else if(tokens[*curr_index]->type == TT_SUB){
		Token* val = tokens[*curr_index];
		(*curr_index)++;
		if(*curr_index < size && *curr_index != TT_EOF){
			node = value(tokens, size, curr_index);

			if(node != NULL && node->nodeType != ERROR_NODE){
				return UnOpNode(val, node, node->nodeType);
			}
		}
	}
	else if(tokens[*curr_index]->type == TT_VAR){
		node = VarNode(tokens[*curr_index]);
		(*curr_index)++;
		return node;
	}
	else{
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
	}

	if(left->nodeType == ERROR_NODE)
		return left;

	while(
		*curr_index < size && *curr_index != TT_EOF && 
		ops[T_OPERATOR_DETECTOR(tokens[*curr_index]->type)] == 1 && 
		left->nodeType != ERROR_NODE
	){
		if(*curr_index == TT_EOF || *curr_index == size)
			return left;
		
		type = T_OPERATOR_DETECTOR(tokens[*curr_index]->type); 
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
	return addsub(tokens, size, curr_index);
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
			break;
		}
		default:
			return;
	}
	if(isTotal == 0)
		printf("\n");
}