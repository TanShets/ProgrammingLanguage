#include "Lexer.h"

#define ERROR_NODE -1
#define TOKEN 0
#define NUM_NODE 1
#define BIN_OP_NODE 2
#define UN_OP_NODE 3

#define ADD_SUB 0
#define PROD_QUO 1

typedef struct NODE{
	int nodeType;
	void *left, *right;
	Token* val;
	int leftType, rightType, valType;
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

Node* addsub(Token** tokens, int size, int* curr_index);

Node* value(Token** tokens, int size, int* curr_index)
{
	if(*curr_index == TT_EOF || *curr_index == size)
		return NULL;
	
	// print_token(tokens[*curr_index]);
	// printf(" -v \n");
	// printf("%d -> %d\n", tokens[*curr_index]->type, *curr_index);
	if(tokens[*curr_index]->type == TT_LPAREN){
		(*curr_index)++;
		if(*curr_index != TT_EOF && *curr_index != size){
			Node* node = addsub(tokens, size, curr_index);
			if(tokens[*curr_index]->type == TT_RPAREN){
				(*curr_index)++;
				return node;
			}
		}
	}
	else if(
		tokens[*curr_index]->type == TT_INT || 
		tokens[*curr_index]->type == TT_FLOAT
	){
		// printf("Ye");
		Node* node = NumNode(tokens[*curr_index]);
		(*curr_index)++;
		// printf("Ye");
		return node;
	}
	else if(tokens[*curr_index]->type == TT_SUB){
		Token* val = tokens[*curr_index];
		(*curr_index)++;
		if(*curr_index < size && *curr_index != TT_EOF){
			Node* node = value(tokens, size, curr_index);

			if(node != NULL && node->nodeType != ERROR_NODE){
				return UnOpNode(val, node, node->nodeType);
			}
		}
	}
	return NULL;
}

Node* prodquo(Token** tokens, int size, int* curr_index);

Node* operation(Token** tokens, int size, int* curr_index, int type_of_operation)
{
	if(*curr_index == TT_EOF || *curr_index == size)
		return NULL;
	
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

	while(
		*curr_index < size && *curr_index != TT_EOF && 
		ops[T_OPERATOR_DETECTOR(tokens[*curr_index]->type)] == 1
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

Node* Parser(Token** tokens, int size, int* curr_index)
{
	// printf("yay\n");
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
			printf("Error\n");
			break;
		case NUM_NODE:
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
		default:
			return;
	}
	if(isTotal == 0)
		printf("\n");
}