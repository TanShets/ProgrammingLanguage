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

#define CONDITIONAL_STATEMENT 0
#define BOOLEAN_STATEMENT 1
#define ADD_SUB 2
#define PROD_QUO 3
#define EXPONENT 4
#define NUM_TO_NUM 5

#define DEFAULT_NO_OF_FUNCTION_PARAMETERS 5

typedef struct NODE{
	int nodeType;
	void *left, *right, *else_block;
	int* block_lengths;
	Token* val;
	int leftType, rightType, valType, isVarNode, else_block_Type;
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
		leftType != INDEX_NODE && leftType != VAR_NODE
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

void printNode(Node* node, int isTotal);

void expand_arrays(
	Node*** statements, Node**** blocks, int** block_lengths, int* no_of_blocks
){
	int old_no_of_blocks = *no_of_blocks;
	*no_of_blocks *= 2;
	// *statements = (Node**)realloc(*statements, *no_of_blocks * sizeof(Node*));
	// *blocks = (Node***)realloc(*blocks, *no_of_blocks * sizeof(Node**));
	// *block_lengths = (int*)realloc(*block_lengths, *no_of_blocks * sizeof(int));
	*statements = (Node**)reallocate_heap_alloced_ptr(*statements, *no_of_blocks * sizeof(Node*));
	*blocks = (Node***)reallocate_heap_alloced_ptr(*blocks, *no_of_blocks * sizeof(Node**));
	*block_lengths = (int*)reallocate_heap_alloced_ptr(*block_lengths, *no_of_blocks * sizeof(int));
	
	
	// Node** new_statements = (Node**)calloc(*no_of_blocks, sizeof(Node*));
	// Node*** new_blocks = (Node***)calloc(*no_of_blocks, sizeof(Node**));
	// int* new_block_lengths = (int*)calloc(*no_of_blocks, sizeof(int));
	// memset(new_block_lengths, 0, *no_of_blocks * sizeof(int));
	// Node** temp1 = *statements;
	// Node*** temp2 = *blocks;
	// int* temp3 = *block_lengths;

	// memcpy(new_statements, *statements, old_no_of_blocks * sizeof(Node*));
	// memcpy(new_blocks, *blocks, old_no_of_blocks * sizeof(Node**));
	// memcpy(new_block_lengths, *block_lengths, old_no_of_blocks * sizeof(int));

	// *statements = new_statements;
	// *blocks = new_blocks;
	// *block_lengths = new_block_lengths;
	// free(temp1);
	// free(temp2);
	// free(temp3);
}

void expand_block(Node*** block, int* block_size){
	int old_block_size = *block_size;
	*block_size *= 2;
	// *block = (Node**)realloc(*block, *block_size * sizeof(Node*));
	*block = (Node**)reallocate_heap_alloced_ptr(*block, *block_size * sizeof(Node*));

	// Node** old_block = *block;
	// Node** new_block = (Node**)calloc(*block_size, sizeof(Node*));
	// memcpy(new_block, *block, old_block_size * sizeof(Node*));
	// *block = new_block;
	// free(old_block);
}

Node* value(Token** tokens, int size, int* curr_index);

Node* Parser(Token** tokens, int size, int* curr_index, int isVarNode);

Node* addsub(Token** tokens, int size, int* curr_index);

Node* boolean_statement(Token** tokens, int size, int* curr_index);

Node* make_IndexNode(Node* starter, Token** tokens, int size, int* curr_index);

Node* operation(Token** tokens, int size, int* curr_index, int type_of_operation);

Node* make_LoopNode(Token** tokens, int size, int* curr_index){
	int max_block_size = 10, i, block_size = 0;
	Token* token_val = tokens[*curr_index];
	// Node** block = (Node**)calloc(max_block_size, sizeof(Node*));
	Node** block = (Node**)allocate_ptr_array(max_block_size, sizeof(Node*));
	Node* condition = NULL;
	if(tokens[*curr_index]->type == TT_WHILE){
		(*curr_index)++;
		condition = Parser(tokens, size, curr_index, 0);
		if(condition->nodeType == ERROR_NODE)
			return condition;
	}
	else{
		(*curr_index)++;
		if(*curr_index == TT_EOF || *curr_index >= size || tokens[*curr_index]->type != TT_VAR)
			return ErrorNode(
				make_error(
					SyntaxError(
						"variable like 'x'",
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
					),
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
				)
			);
		
		Token* var_token = tokens[*curr_index];
		(*curr_index)++;

		if(*curr_index == TT_EOF || *curr_index >= size || tokens[*curr_index]->type != TT_EQ)
			return ErrorNode(
				make_error(
					SyntaxError(
						"'='",
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
					),
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
				)
			);
		Token* eq_token = tokens[*curr_index];
		(*curr_index)++;
		Node* set_of_values = operation(tokens, size, curr_index, NUM_TO_NUM);
		Token *change_eq, *change_var;
		Node* change_val;
		Node* change_node = NULL;
		if(*curr_index != TT_EOF && *curr_index < size && tokens[*curr_index]->type == TT_CHANGE){
			change_var = tokens[*curr_index];
			(*curr_index)++;
			if(*curr_index == TT_EOF || *curr_index >= size || tokens[*curr_index]->type != TT_EQ)
				return ErrorNode(
					make_error(
						SyntaxError(
							"'='",
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
							*curr_index == TT_EOF || *curr_index >= size ? 
							tokens[size - 1]->col_no : tokens[*curr_index]->col_no
						),
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
						*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
					)
				);
			
			change_eq = tokens[*curr_index];
			(*curr_index)++;

			change_val = Parser(tokens, size, curr_index, 0);
			change_node = VarAssignNode(change_var, change_eq, change_val, change_var->type, change_val->nodeType, 1);
		}
		else{
			// change_var = (Token*)malloc(sizeof(Token));
			change_var = (Token*)allocate_ptr_for_size(sizeof(Token));
			change_var->type = TT_INT;
			// int* new_val_for_change = (int*)malloc(sizeof(int));
			int* new_val_for_change = (int*)allocate_ptr_for_size(sizeof(int));
			*new_val_for_change = 1;
			change_var->val = new_val_for_change;
			change_node = NumNode(change_var);
		}

		// condition = VarAssignNode(var_token, eq_token, set_of_values, set_of_values->nodeType, 1);
		condition = ToNode(set_of_values, set_of_values->nodeType, change_var, change_node, change_node->nodeType);
		condition = VarAssignNode(var_token, eq_token, condition, var_token->type, condition->nodeType, 1);
		if(condition->nodeType == ERROR_NODE)
			return condition;
	}

	if(*curr_index == TT_EOF || *curr_index >= size || tokens[*curr_index]->type != TT_BLOCK_OPEN)
		return ErrorNode(
			make_error(
				SyntaxError(
					"{", 
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
				),
				*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
				*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
			)
		);
	
	(*curr_index)++;
	while(
		*curr_index != TT_EOF && *curr_index < size && tokens[*curr_index]->type != TT_BLOCK_CLOSE
	){
		block[block_size] = Parser(tokens, size, curr_index, 0);
		block_size++;
		if(block_size == max_block_size)
			expand_block(&block, &max_block_size);
	}
	
	if(*curr_index == TT_EOF || *curr_index >= size || tokens[*curr_index]->type != TT_BLOCK_CLOSE)
		return ErrorNode(
			make_error(
				SyntaxError(
					"}", 
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
					*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
				),
				*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->line_no : tokens[*curr_index]->line_no,
				*curr_index == TT_EOF || *curr_index >= size ? tokens[size - 1]->col_no : tokens[*curr_index]->col_no
			)
		);
	
	(*curr_index)++;

	return LoopNode(token_val, condition, condition->nodeType, block, block_size);
}

Node* make_FunctionDefinitionNode(Token** tokens, int size, int* curr_index){
	if(tokens[*curr_index]->type != TT_FUNCTION)
		return NULL;
	
	Token* val_token = tokens[*curr_index];
	(*curr_index)++;
	if(IS_EOA(*curr_index, size))
		return ErrorNode(
			make_error(
				EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),
				tokens[size - 1]->line_no, tokens[size - 1]->col_no
			)
		);
	
	Token* name = NULL;
	int isAssigned = 0;
	if(tokens[*curr_index]->type == TT_VAR){
		name = tokens[*curr_index];
		isAssigned = 1;
		(*curr_index)++;
	}

	Node** parameters;
	int max_parameter_size = DEFAULT_NO_OF_FUNCTION_PARAMETERS;
	int no_of_parameters = 0;
	int temp_type, isFinished = 0;

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_LPAREN){
		return ErrorNode(
			make_error(
				SyntaxError(
					"(", 
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	// parameters = (Node**)calloc(max_parameter_size, sizeof(Node*));
	parameters = (Node**)allocate_ptr_array(max_parameter_size, sizeof(Node*));
	while(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type != TT_RPAREN){
		parameters[no_of_parameters] = value(tokens, size, curr_index);
		no_of_parameters++;
		if(no_of_parameters == max_parameter_size)
			expand_block(&parameters, &max_parameter_size);
		
		if(IS_EOA(*curr_index, size))
			break;
		
		temp_type = tokens[*curr_index]->type;
		if(temp_type == TT_COMMA)
			(*curr_index)++;
		else
			isFinished = 1;
	}

	isFinished = no_of_parameters > 0 ? isFinished : 1;

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_RPAREN){
		return ErrorNode(
			make_error(
				SyntaxError(
					")", IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	else if(tokens[*curr_index]->type == TT_RPAREN && !isFinished){
		return ErrorNode(
			make_error(
				SyntaxError(
					"Another parameter", 
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_BLOCK_OPEN){
		return ErrorNode(
			make_error(
				SyntaxError(
					"{", IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	(*curr_index)++;

	int no_of_blocks = 0, max_block_size = 5;
	// Node** block = (Node**)calloc(max_block_size, sizeof(Node*));
	Node** block = (Node**)allocate_ptr_array(max_block_size, sizeof(Node*));
	while(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type != TT_BLOCK_CLOSE){
		block[no_of_blocks] = Parser(tokens, size, curr_index, 0);
		if(block[no_of_blocks]->nodeType == ERROR_NODE)
			return block[no_of_blocks];
		no_of_blocks++;
		if(no_of_blocks == max_block_size)
			expand_block(&block, &max_block_size);
	}

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_BLOCK_CLOSE){
		return ErrorNode(
			make_error(
				SyntaxError(
					"}", IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	return FunctionDefinitionNode(
			val_token, parameters, no_of_parameters, block, 
			no_of_blocks, isAssigned, name
		);
}

Node* make_ReturnNode(Token** tokens, int size, int* curr_index){
	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_RETURN){
		return ErrorNode(
			make_error(
				EOFError(
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	Token* val_token = tokens[*curr_index];
	(*curr_index)++;
	Node* val = NULL;
	if(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type != TT_BLOCK_CLOSE)
		val = Parser(tokens, size, curr_index, 1);
	else{
		// Token* token = (Token*)malloc(sizeof(Token));
		Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
		token->type = TT_NULL;
		char* word = {"null"};
		token->val = word;
		token->line_no = IS_EOA(*curr_index, size) ? 
			tokens[size - 1]->line_no : tokens[*curr_index]->line_no;
		token->col_no = IS_EOA(*curr_index, size) ? 
			tokens[size - 1]->col_no : tokens[*curr_index]->col_no;
		
		val = NullNode(token);
	}
	return ReturnNode(val_token, val, val->nodeType);
}

Node* make_ArrayNode(Token** tokens, int size, int* curr_index){
	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_ARRAY_OPEN){
		return ErrorNode(
			make_error(
				SyntaxError(
					"[",
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	Token* token_val = tokens[*curr_index];
	token_val->type = TT_ARRAY;
	(*curr_index)++;
	int max_size_of_array = 10, current_size = 0;
	Node **keys, **values;
	// keys = (Node**)calloc(max_size_of_array, sizeof(Node*));
	// values = (Node**)calloc(max_size_of_array, sizeof(Node*));
	keys = (Node**)allocate_ptr_array(max_size_of_array, sizeof(Node*));
	values = (Node**)allocate_ptr_array(max_size_of_array, sizeof(Node*));
	Node *temp_key, *temp_val;
	int isFinished = 0;

	while(
		!IS_EOA(*curr_index, size) && 
		tokens[*curr_index]->type != TT_ARRAY_CLOSE && 
		!isFinished
	){
		temp_key = Parser(tokens, size, curr_index, 0);
		if(temp_key->nodeType == ERROR_NODE)
			return temp_key;

		if(
			!IS_EOA(*curr_index, size) && 
			tokens[*curr_index]->type == TT_ARRAY_KEY_VAL_SEPARATOR
		){
			(*curr_index)++;
			temp_val = Parser(tokens, size, curr_index, 0);
			if(temp_val->nodeType == ERROR_NODE)
				return temp_val;
		}
		else
			temp_val = temp_key, temp_key = NULL;
		
		keys[current_size] = temp_key;
		values[current_size] = temp_val;
		current_size++;
		if(current_size == max_size_of_array){
			expand_block(&keys, &max_size_of_array);
			max_size_of_array /= 2;
			expand_block(&values, &max_size_of_array);
		}

		if(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type == TT_COMMA)
			(*curr_index)++;
		else
			isFinished = 1;
	}

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_ARRAY_CLOSE){
		return ErrorNode(
			make_error(
				SyntaxError(
					"]",
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	else if(tokens[*curr_index]->type == TT_ARRAY_CLOSE && !isFinished){
		return ErrorNode(
			make_error(
				SyntaxError(
					"Another parameter", 
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	return ArrayNode(token_val, keys, values, current_size);
}

Node* make_FunctionCallNode(Token** tokens, int size, int* curr_index, int isVarNode){
	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_VAR){
		return ErrorNode(
			make_error(
				EOFError(
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	
	Token* val_token = tokens[*curr_index];
	(*curr_index)++;
	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_LPAREN){
		return ErrorNode(
			make_error(
				SyntaxError(
					"(",
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	int max_parameter_size = DEFAULT_NO_OF_FUNCTION_PARAMETERS;
	int no_of_parameters = 0;
	// Node** parameters = (Node**)calloc(max_parameter_size, sizeof(Node*));
	Node** parameters = (Node**)allocate_ptr_array(max_parameter_size, sizeof(Node*));
	int temp_type, isFinished = 0;

	while(
		!IS_EOA(*curr_index, size) && tokens[*curr_index]->type != TT_RPAREN && 
		!isFinished
	){
		parameters[no_of_parameters] = Parser(tokens, size, curr_index, isVarNode);
		if(parameters[no_of_parameters]->nodeType == ERROR_NODE)
			return parameters[no_of_parameters];
		no_of_parameters++;
		if(no_of_parameters == max_parameter_size)
			expand_block(&parameters, &max_parameter_size);
		
		if(IS_EOA(*curr_index, size))
			break;
		
		temp_type = tokens[*curr_index]->type;
		if(temp_type == TT_COMMA)
			(*curr_index)++;
		else
			isFinished = 1;
	}

	isFinished = no_of_parameters > 0 ? isFinished : 1;

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_RPAREN){
		return ErrorNode(
			make_error(
				SyntaxError(
					")", IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	else if(tokens[*curr_index]->type == TT_RPAREN && !isFinished){
		return ErrorNode(
			make_error(
				SyntaxError(
					"Another parameter", 
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	Node* node = FunctionCallNode(val_token, parameters, no_of_parameters);
	if(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type == TT_ARRAY_OPEN)
		return make_IndexNode(node, tokens, size, curr_index);
	else
		return node;
}

Node* make_IndexNode(Node* starter, Token** tokens, int size, int* curr_index){
	if(IS_EOA(*curr_index, size)){
		return ErrorNode(
			make_error(
				EOFError(tokens[size - 1]->line_no, tokens[size - 1]->col_no),
				tokens[size - 1]->line_no, tokens[size - 1]->col_no
			)
		);
	}
	else if(
		starter->nodeType != VAR_NODE && 
		starter->nodeType != FUNCTION_CALL_NODE && 
		starter->nodeType != INDEX_NODE
	){
		return ErrorNode(
			make_error(
				SyntaxError(
					"Variable or function call", tokens[*curr_index]->line_no,
					tokens[*curr_index]->col_no
				), tokens[*curr_index]->line_no,
				tokens[*curr_index]->col_no
			)
		);
	}
	else if(tokens[*curr_index]->type != TT_ARRAY_OPEN){
		return ErrorNode(
			make_error(
				SyntaxError(
					"[", tokens[*curr_index]->line_no, tokens[*curr_index]->col_no
				),
				tokens[*curr_index]->line_no, tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;
	int count = 0;
	int max_no_of_indices = 10;
	int isFinished = 0;
	// Node** indices = (Node**)calloc(max_no_of_indices, sizeof(Node*));
	Node** indices = (Node**)allocate_ptr_array(max_no_of_indices, sizeof(Node*));
	Node* temp_node;
	while(
		!IS_EOA(*curr_index, size) && 
		tokens[*curr_index]->type != TT_ARRAY_CLOSE && 
		!isFinished
	){
		temp_node = Parser(tokens, size, curr_index, 0);
		if(temp_node->nodeType == ERROR_NODE)
			return temp_node;
		indices[count] = temp_node;
		count++;

		if(count == max_no_of_indices)
			expand_block(&indices, &max_no_of_indices);
		
		if(!IS_EOA(*curr_index, size)){
			if(tokens[*curr_index]->type == TT_COMMA)
				(*curr_index)++;
			else
				isFinished = 1;
		}
	}

	if(IS_EOA(*curr_index, size) || tokens[*curr_index]->type != TT_ARRAY_CLOSE){
		return ErrorNode(
			make_error(
				SyntaxError(
					"]", 
					IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
					tokens[*curr_index]->line_no,
					IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
					tokens[*curr_index]->col_no
				),
				IS_EOA(*curr_index, size) ? tokens[size - 1]->line_no : 
				tokens[*curr_index]->line_no,
				IS_EOA(*curr_index, size) ? tokens[size - 1]->col_no : 
				tokens[*curr_index]->col_no
			)
		);
	}
	else if(!isFinished){
		return ErrorNode(
			make_error(
				SyntaxError(
					"another index", tokens[*curr_index]->line_no,
					tokens[*curr_index]->col_no
				),
				tokens[*curr_index]->line_no,
				tokens[*curr_index]->col_no
			)
		);
	}

	(*curr_index)++;

	Node* node = IndexNode(starter, indices, count);
	return !IS_EOA(*curr_index, size) && 
			tokens[*curr_index]->type == TT_ARRAY_OPEN ? 
			make_IndexNode(node, tokens, size, curr_index) : node;
}

Node* make_VarNode(Token** tokens, int size, int* curr_index){
	Node* node = VarNode(tokens[*curr_index]);
	(*curr_index)++;
	if(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type == TT_LPAREN){
		(*curr_index)--;
		return make_FunctionCallNode(tokens, size, curr_index, 0);
	}
	else if(!IS_EOA(*curr_index, size) && tokens[*curr_index]->type == TT_ARRAY_OPEN){
		return make_IndexNode(node, tokens, size, curr_index);
	}
	else
		return node;
}

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
			return make_VarNode(tokens, size, curr_index);
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
			// Node** statements = (Node**)calloc(no_of_blocks, sizeof(Node*));
			// Node*** blocks = (Node***)calloc(no_of_blocks, sizeof(Node**));
			// int* block_lengths = (int*)calloc(no_of_blocks, sizeof(int));

			Node** statements = (Node**)allocate_ptr_array(no_of_blocks, sizeof(Node*));
			Node*** blocks = (Node***)allocate_ptr_array(no_of_blocks, sizeof(Node**));
			int* block_lengths = (int*)allocate_ptr_array(no_of_blocks, sizeof(int));
			// memset(block_lengths, 0, no_of_blocks * sizeof(int));
			set_heap_alloced_memory(block_lengths, 0, no_of_blocks * sizeof(int));
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
				
				// temp_block = (Node**)calloc(inner_block_count, sizeof(Node*));
				temp_block = (Node**)allocate_ptr_array(inner_block_count, sizeof(Node*));
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
				// temp_block = (Node**)calloc(inner_block_count, sizeof(Node*));
				temp_block = (Node**)allocate_ptr_array(inner_block_count, sizeof(Node*));
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
				count, val_token, else_block, else_block_length
			);

			break;
		}
		case TT_WHILE:
			return make_LoopNode(tokens, size, curr_index);
			break;
		case TT_FOR:
			return make_LoopNode(tokens, size, curr_index);
			break;
		case TT_BREAK:
			node = BreakNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
			break;
		case TT_FUNCTION:{
			return make_FunctionDefinitionNode(tokens, size, curr_index);
			break;
		}
		case TT_RETURN:{
			return make_ReturnNode(tokens, size, curr_index);
			break;
		}
		case TT_STRING:{
			node = StringNode(tokens[*curr_index]);
			(*curr_index)++;
			return node;
		}
		case TT_ARRAY_OPEN:{
			return make_ArrayNode(tokens, size, curr_index);
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

Node* exponent(Token** tokens, int size, int* curr_index);

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
		case NUM_TO_NUM:{
			ops[TT_TO] = 1;
			break;
		}
		case EXPONENT:{
			ops[TT_EXPONENT] = 1;
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
			left = exponent(tokens, size, curr_index);
			break;
		}
		case EXPONENT:{
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
		case NUM_TO_NUM:{
			left = Parser(tokens, size, curr_index, 0);
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
				right = exponent(tokens, size, curr_index);
				break;
			}
			case EXPONENT:{
				right = exponent(tokens, size, curr_index);
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
			case NUM_TO_NUM:{
				ops[TT_TO] = 0;
				right = Parser(tokens, size, curr_index, 0);
				break;
			}
		}

		if(right->nodeType == ERROR_NODE)
			return right;

		left = BinOpNode((void*)left, left->nodeType, val, (void*)right, right->nodeType);
	}

	return left;
}

Node* exponent(Token** tokens, int size, int* curr_index)
{
	return operation(tokens, size, curr_index, EXPONENT);
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
		Node* left = make_VarNode(tokens, size, &temp_index);
		// temp_index++;
		if(left->nodeType == ERROR_NODE)
			return left;
		
		if(temp_index < size && IN_ASSIGNMENT_OPERATORS(tokens[temp_index]->type))
		{
			Token* val = tokens[temp_index];
			// (*curr_index)++;
			*curr_index = temp_index;
			// Token* val = tokens[*curr_index];
			(*curr_index)++;
			Node* right = Parser(tokens, size, curr_index, isVarNode);
			if(right->nodeType == FUNCTION_DEFINITION_NODE){
				if(left->nodeType != VAR_NODE){
					char* name_of_func = NULL;
					switch(left->nodeType){
						case FUNCTION_CALL_NODE:{
							name_of_func = (char*)(left->val->val);
							break;
						}
						case INDEX_NODE:{
							Node* temp = left;
							while(temp != NULL && temp->nodeType == INDEX_NODE){
								temp = (Node*)(temp->left);
							}

							char foo_name[] = {"foo"};
							name_of_func = temp == NULL ? foo_name : 
											(char*)(temp->val->val);
							break;
						}
					}
					return ErrorNode(
						make_error(
							InvalidFunctionNameError(
								name_of_func, left->nodeType,
								left->val->line_no, left->val->col_no	
							),
							left->val->line_no, left->val->col_no
						)
					);
				}
				right->else_block_Type = 1;
				right->else_block = left->val;
			}
			return right->nodeType == ERROR_NODE ? right : 
				right->nodeType == FUNCTION_DEFINITION_NODE ? right : 
				VarAssignNode(left, val, right, left->nodeType, right->nodeType, isVarNode);
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
		case STRING_NODE:{
			printf("(%d, \"%s\")", node->nodeType, (char*)(node->val->val));
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