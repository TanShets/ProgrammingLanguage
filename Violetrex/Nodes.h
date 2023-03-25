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
#define TO_NODE 9
#define LOOP_NODE 10
#define BREAK_NODE 11
#define FUNCTION_DEFINITION_NODE 12
#define RETURN_NODE 13
#define FUNCTION_CALL_NODE 14
#define STRING_NODE 15
#define ARRAY_NODE 16
#define INDEX_NODE 17
#define CLASS_DEFINITION_NODE 18
#define STATIC_FUNCTION_DEFINITION_NODE 19
#define OBJECT_NODE 20
#define THIS_NODE 21
#define IMPORT_NODE 22

#define CONDITIONAL_STATEMENT 0
#define BOOLEAN_STATEMENT 1
#define ADD_SUB 2
#define PROD_QUO 3
#define EXPONENT 4
#define NUM_TO_NUM 5

#define DEFAULT_NO_OF_FUNCTION_PARAMETERS 5
#define DEFAULT_NO_OF_IMPORTS 5

#define ASSERT_NOT_EOF(curr_index, size, tokens) ({\
	if(IS_EOA(curr_index, size)){\
		return ErrorNode(\
			make_error(\
				EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),\
				tokens[size - 1]->line_no, tokens[size - 1]->col_no\
			)\
		);\
	}\
})

typedef struct NODE{
	int nodeType;
	void *left, *right, *else_block;
	int* block_lengths;
	Token* val;
	int leftType, rightType, valType, isVarNode, else_block_Type;
	char* prefix;
}Node;

Node* construct_Node(Token* val, int nodeType){
	// Node* node = (Node*)malloc(sizeof(Node));
	Node* node = (Node*)allocate_ptr_for_size(sizeof(Node));
	node->nodeType = nodeType;
	node->val = val;
	node->valType = val->type;
	node->left = NULL, node->right = NULL, node->else_block = NULL;
	node->block_lengths = NULL, node->leftType = TT_ERROR;
	node->rightType = TT_ERROR, node->isVarNode = 0;
	node->else_block_Type = TT_ERROR;
	node->prefix = NULL;
	return node;
}

Node* StaticFunctionDefinitionNode(Token* val, Node* right){
	if(val->type != TT_STATIC)
		return NULL;
	Node* node = construct_Node(val, STATIC_FUNCTION_DEFINITION_NODE);
	node->right = right;
	return node;
}

Node* ThisNode(Token* val){
	return val->type == TT_THIS ? construct_Node(val, THIS_NODE) : NULL;
}

Node* ObjectNode(Token* val, Node* object_name, Node* property_called){
	Node* node = construct_Node(val, OBJECT_NODE);
	node->left = object_name;
	node->leftType = object_name->nodeType;
	node->right = property_called;
	node->rightType = property_called->nodeType;
	return node;
}

Node* ClassDefinitionNode(
	Token* val, Node** superclasses, int no_of_superclasses, Node** body, 
	int no_of_nodes, Node* class_name
){
	if(val->type != TT_CLASS)
		return NULL;
	Node* node = construct_Node(val, CLASS_DEFINITION_NODE);
	node->left = superclasses;
	node->leftType = no_of_superclasses;
	node->right = body;
	node->rightType = no_of_nodes;
	node->else_block = class_name;
	return node;
}

Node* BreakNode(Token* val){
	return construct_Node(val, BREAK_NODE);
}

Node* ReturnNode(Token* val, Node* return_val, int return_type){
	Node* node = construct_Node(val, RETURN_NODE);
	node->right = return_val;
	node->rightType = return_type;
	return node;
}

Node* ArrayNode(Token* val, Node** keys, Node** values, int length){
	Node* node = construct_Node(val, ARRAY_NODE);
	node->left = keys;
	node->right = values;
	node->rightType = length;
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

Node* ToNode(
	Node* left, int leftType, Token* val, Node* right, int rightType
){
	Node* node = construct_Node(val, TO_NODE);
	// a to b is on the left
	node->left = left, node->leftType = leftType;
	node->right = right, node->rightType = rightType;
	// change is on the right, if default, change is NumNode, else a VarAssign node
	return node;
}

Node* LoopNode(Token* val, Node* condition, int condition_type, Node** block, int size_of_block){
	Node* node = construct_Node(val, LOOP_NODE);
	node->left = condition, node->leftType = condition_type;
	node->right = block, node->rightType = size_of_block;
	return node;
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

Node* StringNode(Token* token)
{
	if(token->type == TT_STRING){
		return construct_Node(token, STRING_NODE);
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

Node* VarAssignNode(void* left, Token* val, void* right, int leftType, int rightType, int isVarNode)
{
	if(
		leftType != TT_VAR && leftType != TT_CHANGE && 
		leftType != INDEX_NODE && leftType != VAR_NODE && 
		leftType != OBJECT_NODE
	)
		return ErrorNode(
			make_error(
				SyntaxError(
					"Variable name Eg. x", 
					((Token*)left)->line_no, 
					((Token*)left)->col_no
				),
				((Token*)left)->line_no, ((Token*)left)->col_no
			)
		);
	
	Node* node = construct_Node(val, VAR_ASSIGN_NODE);
	node->left = left, node->leftType = leftType;
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

Node* IndexNode(Node* starter, Node** indices, int count){
	Node* node = construct_Node(starter->val, INDEX_NODE);
	node->left = starter;
	node->leftType = starter->nodeType;
	node->right = indices;
	node->rightType = count;
	return node;
}

Node* FunctionDefinitionNode(
	Token* val, Node** parameters, int no_of_parameters,
	Node** block, int no_of_blocks, int isNamed, Token* name
){
	if(val->type != TT_FUNCTION)
		return NULL;
	Node* node = construct_Node(val, FUNCTION_DEFINITION_NODE);
	node->left = parameters, node->leftType = no_of_parameters;
	node->right = block, node->rightType = no_of_blocks;
	node->else_block_Type = isNamed;
	node->else_block = name;
	return node;
}

Node* FunctionCallNode(Token* val, Node** parameters, int no_of_parameters){
	Node* node = construct_Node(val, FUNCTION_CALL_NODE);
	node->left = parameters;
	node->leftType = no_of_parameters;
	return node;
}

Node* ImportNode(Node** imports, Token* val, Node* source, int no_of_imports, int isDefaultSource){
    if(val->type != TT_IMPORT){
        return ErrorNode(
            make_error(
                SyntaxError("Expected 'import' keyword", val->line_no, val->col_no),
                val->line_no, val->col_no
            )
        );
    }

    Node* node = construct_Node(val, IMPORT_NODE);
    node->leftType = no_of_imports;
    node->left = imports;
    node->rightType = isDefaultSource;
    node->right = node->rightType ? NULL : source;
    return node;
}