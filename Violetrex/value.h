#pragma once
#include "Parser.h"
#include "Context.h"

#define POINT_CASTER(x, dtype) ((dtype == TT_INT) ? (int*)x : (double*)x)
#define TYPE_CASTER(x, dtype1, dtype2) ( \
	dtype1 == TT_INT && dtype2 == TT_INT ? (int)(*((int*)x)) : \
	dtype1 == TT_FLOAT && dtype2 == TT_INT ? (int)(*((double*)x)) : \
	dtype1 == TT_INT && dtype2 == TT_FLOAT ? (double)(*((int*)x)) : \
	dtype1 == TT_FLOAT && dtype2 == TT_FLOAT ? (double)(*((double*)x)) : \
	dtype1 == TT_TRUE && dtype2 == TT_INT ? (int)1 : \
	dtype1 == TT_TRUE && dtype2 == TT_FLOAT ? (double)1 : \
	dtype1 == TT_FALSE && dtype2 == TT_INT ? (int)0 : \
	dtype1 == TT_FALSE && dtype2 == TT_FLOAT ? (double)0 : \
	dtype1 == TT_TRUE && dtype2 == TT_TRUE ? 1 : \
	dtype1 == TT_FALSE && dtype2 == TT_FALSE ? 0 : 0 \
)

typedef struct VALUE{
	void* num;
	int valType, line_no, col_no;
} Value;

typedef struct INTERPRETER{
    Value** values;
    int no_of_values;
    int isBroken;
} Interpreter;

void printValue(Value* value);

Value* copy_Value(Value* value){
	// Value* new_value = (Value*)malloc(sizeof(Value));
	Value* new_value = (Value*)allocate_ptr_for_size(sizeof(Value));
	new_value->line_no = value->line_no, new_value->col_no = value->col_no;
	new_value->valType = value->valType;
	switch(value->valType){
		case TT_INT:
			// new_value->num = malloc(sizeof(int));
			new_value->num = allocate_ptr_for_size(sizeof(int));
            *((int*)new_value->num) = *((int*)value->num);
			// copy_heap_alloced_memory(new_value->num, value->num, sizeof(int));
            break;
        case TT_FLOAT:
			// new_value->num = malloc(sizeof(double));
			new_value->num = allocate_ptr_for_size(sizeof(double));
            *((double*)new_value->num) = *((double*)value->num);
			// copy_heap_alloced_memory(new_value->num, value->num, sizeof(double));
            break;
        case TT_NULL:
            new_value->num = value->num;
            break;
        case TT_STRING:{
            char* word1 = (char*)value->num;
			int length = strlen(word1);
			// char* word2 = (char*)calloc(strlen(word1) + 2, sizeof(char));
			char* word2 = (char*)allocate_ptr_array(strlen(word1) + 2, sizeof(char));
			strncpy(word2, word1, length);
			word2[length] = '\0';
			new_value->num = word2;
            break;
		}
        case TT_TRUE:
            new_value->num = value->num;
            break;
        case TT_FALSE:
            new_value->num = value->num;
            break;
        default:
            return NULL;
	}
	return new_value;
}

Value* viewNode(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix);
Value* construct_Value(Token* token);

void alterValues(Value* answer, Value* changer, int op_type){
	int typo = answer->valType;
	int typp = changer->valType;
	int* temp1 = (int*)(answer->num);
	double* temp2 = (double*)(answer->num);
	//printf("%d %d %d %d\n", typo, typp, TT_INT, TT_FLOAT);
	//printf("%d %lf %lf %lf\n", *temp1, *temp2, (double)(*((int*)(changer->num))), TYPE_CASTER(changer->num, typp, typo));

	switch(op_type){
		case TT_ADD:
			if(typo == TT_INT)
				*((int*)answer->num) += TYPE_CASTER(changer->num, typp, typo);
			else if(typo == TT_FLOAT)
				*((double*)answer->num) += TYPE_CASTER(changer->num, typp, typo);
			else if(typo == TT_STRING && typp == TT_STRING){
				char* temp_string = (char*)answer->num;
				char* temp_string2 = (char*)changer->num;
				int new_length = strlen(temp_string) + strlen(temp_string2) + 2;
				// answer->num = (char*)calloc(new_length, sizeof(char));
				answer->num = (char*)allocate_ptr_array(new_length, sizeof(char));
				strncpy((char*)(answer->num), temp_string, strlen(temp_string));
				((char*)(answer->num))[strlen(temp_string)] = '\0';
				strcat((char*)(answer->num), (char*)(changer->num));
			}
			// *(POINT_CASTER(answer->num, typo)) += TYPE_CASTER(changer->num, typo, typp);
			// *((POINT_CASTER(typo))answer->num) += (TYPE_CASTER(typo))(*((POINT_CASTER(typp))changer->num));
			break;
		case TT_SUB:
			if(typo == TT_INT)
				*((int*)answer->num) -= TYPE_CASTER(changer->num, typp, typo);
			else if(typo == TT_FLOAT)
				*((double*)answer->num) -= TYPE_CASTER(changer->num, typp, typo);
			break;
		case TT_MUL:
			if(typo == TT_INT)
				*((int*)answer->num) *= TYPE_CASTER(changer->num, typp, typo);
			else if(typo == TT_FLOAT)
				*((double*)answer->num) *= TYPE_CASTER(changer->num, typp, typo);
			break;
		case TT_DIV:{
			if(typo == TT_INT){
				// double* new_val = (double*)malloc(sizeof(double));
				double* new_val = (double*)allocate_ptr_for_size(sizeof(double));
				*new_val = (double)(*((int*)answer->num));
				answer->num = new_val;
				answer->valType = TT_FLOAT;
			}
			else if(typo != TT_FLOAT)
				return;
			*((double*)answer->num) /= TYPE_CASTER(changer->num, typp, typo);
			break;
		}
		case TT_EXPONENT:{
			if(typo == TT_INT && typp == TT_INT){
				int* temp_answer = (int*)answer->num;
				int temp_num = *temp_answer;
				int index = *((int*)changer->num);
				*temp_answer = 1;
				if(index >= 0)
					for(int i = 0; i < index; i++)
						*temp_answer *= temp_num;
				else{
					answer->valType = TT_FLOAT;
					// answer->num = (double*)malloc(sizeof(double));
					answer->num = (double*)allocate_ptr_for_size(sizeof(double));
					*((double*)answer->num) = 1;
					for(int i = 0; i > index; i--)
						*((double*)answer->num) /= temp_num;
				}
			}
			else if(
				(typo == TT_INT || typo == TT_FLOAT) && 
				(typp == TT_INT || typp == TT_FLOAT)
			){
				double num1 = TYPE_CASTER(answer->num, typo, TT_FLOAT);
				double num2 = TYPE_CASTER(changer->num, typp, TT_FLOAT);
				num1 = pow(num1, num2);
				answer->valType = TT_FLOAT;
				// answer->num = malloc(sizeof(double));
				answer->num = allocate_ptr_for_size(sizeof(double));
				*((double*)answer->num) = num1;
			}
			break;
		}
	}

	if(IN_BOOLEAN_OPERATORS(op_type)){
		// Value* temp_answer = (Value*)malloc(sizeof(Value));
		Value* temp_answer = (Value*)allocate_ptr_for_size(sizeof(Value));
		// memcpy(temp_answer, answer, sizeof(Value));
		copy_heap_alloced_memory(temp_answer, answer, sizeof(Value));
		int new_value = 0;
		switch(op_type){
			case TT_EQUALS:
				new_value = typo == TT_INT ? *((int*)answer->num) == TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_FLOAT ? *((double*)answer->num) == TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_NULL || typp == TT_NULL ? typo == typp : 
							typo == TT_STRING ? strcmp(
								(char*)(answer->num),(char*)(changer->num)
							) == 0 : 
							0;
				break;
			case TT_NOT_EQUALS:
				new_value = typo == TT_INT ? *((int*)answer->num) != TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_FLOAT ? *((double*)answer->num) != TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_NULL || typp == TT_NULL ? typo != typp : 
							typo == TT_STRING ? strcmp(
								(char*)(answer->num),(char*)(changer->num)
							) != 0 : 0;
				break;
			case TT_LESS_THAN:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) < TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_FLOAT)
					new_value = *((double*)answer->num) < TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_STRING && typp == TT_STRING)
					new_value = strcmp(
									(char*)(answer->num),(char*)(changer->num)
								) < 0;
				break;
			case TT_LESS_THAN_EQ:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) <= TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_FLOAT)
					new_value = *((double*)answer->num) <= TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_STRING && typp == TT_STRING)
					new_value = strcmp(
									(char*)(answer->num),(char*)(changer->num)
								) <= 0;
				break;
			case TT_GREATER_THAN:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) > TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_FLOAT)
					new_value = *((double*)answer->num) > TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_STRING && typp == TT_STRING)
					new_value = strcmp(
									(char*)(answer->num),(char*)(changer->num)
								) > 0;
				break;
			case TT_GREATER_THAN_EQ:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) >= TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_FLOAT)
					new_value = *((double*)answer->num) >= TYPE_CASTER(changer->num, typp, typo);
				else if(typo == TT_STRING && typp == TT_STRING)
					new_value = strcmp(
									(char*)(answer->num),(char*)(changer->num)
								) >= 0;
				break;
		}

		Value* new_answer = NULL;
		// Token* new_token = (Token*)malloc(sizeof(Token));
		Token* new_token = (Token*)allocate_ptr_for_size(sizeof(Token));
		new_token->type = new_value == 1 ? TT_TRUE : TT_FALSE;
		// new_token->val = (char*)calloc(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
		new_token->val = (char*)allocate_ptr_array(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
		new_token->line_no = answer->line_no, new_token->col_no = answer->col_no;
		char *True = "true", *False = "false";
		if(new_value == 1)
			strncpy((char*)(new_token->val), True, strlen(True));
		else
			strncpy((char*)(new_token->val), False, strlen(False));
		new_answer = construct_Value(new_token);
		// memcpy(answer, new_answer, sizeof(Value));
		copy_heap_alloced_memory(answer, new_answer, sizeof(Value));
	}
	else if(IN_CONDITIONAL_OPERATORS(op_type)){
		// Token* new_token = (Token*)malloc(sizeof(Token));
		Token* new_token = (Token*)allocate_ptr_for_size(sizeof(Token));
		int new_val = 0;
		int val1 = 0, val2 = 0;
		val1 = TYPE_CASTER(answer->num, typo, TT_INT);
		val2 = TYPE_CASTER(changer->num, typp, TT_INT);
		new_val = op_type == TT_AND ? val1 && val2 : val1 || val2;
		
		new_token->type = new_val ? TT_TRUE : TT_FALSE;
		// new_token->val = (char*)calloc(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
		new_token->val = (char*)allocate_ptr_array(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
		new_token->line_no = answer->line_no, new_token->col_no = answer->col_no;
		char *True = "true", *False = "false";
		if(new_val)
			strncpy((char*)(new_token->val), True, strlen(True));
		else
			strncpy((char*)(new_token->val), False, strlen(False));
		Value* new_answer = construct_Value(new_token);
		// memcpy(answer, new_answer, sizeof(Value));
		copy_heap_alloced_memory(answer, new_answer, sizeof(Value));
	}
}

Value* operateValues(Value* left, Value* right, int op_type, int isChangingVar){
	if(op_type == TT_DIV){
		if(TYPE_CASTER(right->num, right->valType, left->valType) == 0)
			return construct_Value(
				make_error(
					DivisionByZeroError(right->line_no, right->col_no),
					right->line_no, right->col_no
				)
			);
	}
	else if(!IN_NULL_OPERATORS(op_type)){
		if(left->valType == TT_NULL || right->valType == TT_NULL)
			return construct_Value(
				make_error(
					NullOperationError(
						T_OPERATOR_REVERSE(isChangingVar == 1 ? op_type + 15 : op_type),
						left->valType == TT_NULL ? left->line_no : right->line_no,
						left->valType == TT_NULL ? left->col_no : right->col_no
					),
					left->valType == TT_NULL ? left->line_no : right->line_no,
					left->valType == TT_NULL ? left->col_no : right->col_no
				)
			);
	}
	
	if(left->valType == right->valType){
		alterValues(left, right, op_type);
		return left;
	}
	else{
		Value* answer = NULL;
		Value* changer = NULL;
		changer = right;
		// answer = (Value*)malloc(sizeof(Value));
		answer = (Value*)allocate_ptr_for_size(sizeof(Value));
		// memcpy(answer, left, sizeof(Value));
		copy_heap_alloced_memory(answer, left, sizeof(Value));
		double* new_temp;
		if(left->valType == TT_INT && right->valType == TT_FLOAT){
			answer->valType = TT_FLOAT;
			// new_temp = (double*)malloc(sizeof(double));
			new_temp = (double*)allocate_ptr_for_size(sizeof(double));
			*new_temp = (double)(*((int*)answer->num));
			answer->num = new_temp;
		}
		alterValues(answer, changer, op_type);
		return answer;
	}
}

Value* construct_Value(Token* token){
	// Value* value = (Value*)malloc(sizeof(Value));
	Value* value = (Value*)allocate_ptr_for_size(sizeof(Value));
	value->num = token->val;
	value->valType = token->type;
	char* temp_val;
	switch(value->valType){
		case TT_INT:
			// value->num = (int*)malloc(sizeof(int));
			value->num = (int*)allocate_ptr_for_size(sizeof(int));
			*((int*)(value->num)) = *((int*)(token->val));
			break;
		case TT_FLOAT:
			// value->num = (double*)malloc(sizeof(double));
			value->num = (double*)allocate_ptr_for_size(sizeof(double));
			*((double*)(value->num)) = *((double*)(token->val));
			break;
		case TT_ERROR:
			// value->num = (Error*)malloc(sizeof(Error));
			value->num = (Error*)allocate_ptr_for_size(sizeof(Error));
			// memcpy(value->num, token->val, sizeof(Error));
			copy_heap_alloced_memory(value->num, token->val, sizeof(Error));
			break;
		case TT_TRUE:
			temp_val = (char*)(token->val);
			// value->num = (char*)calloc(strlen(temp_val), sizeof(char));
			value->num = (char*)allocate_ptr_array(strlen(temp_val), sizeof(char));
			strncpy((char*)(value->num), temp_val, strlen(temp_val));
			break;
		case TT_FALSE:
			temp_val = (char*)(token->val);
			// value->num = (char*)calloc(strlen(temp_val), sizeof(char));
			value->num = (char*)allocate_ptr_array(strlen(temp_val), sizeof(char));
			strncpy((char*)(value->num), temp_val, strlen(temp_val));
			break;
		case TT_NULL:
			temp_val = (char*)(token->val);
			// value->num = (char*)calloc(strlen(temp_val), sizeof(char));
			value->num = (char*)allocate_ptr_array(strlen(temp_val), sizeof(char));
			strncpy((char*)(value->num), temp_val, strlen(temp_val));
			break;
		default:
			value->num = token->val;
	}
	value->line_no = token->line_no, value->col_no = token->col_no;
}

Value* getStringValue(Node* node){
	return node->nodeType != STRING_NODE ? NULL : construct_Value(node->val);
}

Value* getNumValue(Node* node){
	return node->nodeType != NUM_NODE ? NULL : construct_Value(node->val);
}

Value* getBooleanValue(Node* node){
	return node->nodeType != BOOLEAN_NODE ? NULL : construct_Value(node->val);
}

Value* getNullValue(Node* node){
	return node->nodeType != NULL_NODE ? NULL : construct_Value(node->val);
}

Value* getBinOpValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
	if(node->nodeType != BIN_OP_NODE)
		return NULL;
	else{
		Value* leftValue = viewNode((Node*)node->left, context, isNode, flag_type, context_needed, prefix);
		Value* rightValue = viewNode((Node*)node->right, context, isNode, flag_type, context_needed, prefix);
		return operateValues(leftValue, rightValue, node->valType, 0);
	}
}

Value* getUnOpValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
	if(node->nodeType != UN_OP_NODE)
		return NULL;
	else{
		if(node->valType == TT_SUB){
			Value* val = viewNode((Node*)node->right, context, isNode, flag_type, context_needed, prefix);
			switch(val->valType){
				case TT_INT:{
					*((int*)(val->num)) *= -1;
					break;
				}
				case TT_FLOAT:{
					*((double*)(val->num)) *= -1;
					break;
				}
			}
			return val;
		}
		else if(node->valType == TT_NOT){
			Value* val = viewNode((Node*)node->right, context, isNode, flag_type, context_needed, prefix);
			int value = 0;
			char *True = "true", *False = "false";
			switch(val->valType){
				case TT_INT:{
					int* temp = (int*)val->num;
					value = *temp == 0 ? 1 : 0;
					break;
				}
				case TT_FLOAT:{
					double* temp = (double*)val->num;
					value = *temp == 0 ? 1 : 0;
					break;
				}
				case TT_TRUE:
					value = 0;
					break;
				case TT_FALSE:
					value = 1;
					break;
				case TT_NULL:
					value = 1;
					break;
			}
			// val->num = calloc(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
			val->num = allocate_ptr_array(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
			if(value == 0)
				strncpy((char*)(val->num), False, strlen(False));
			else
				strncpy((char*)(val->num), True, strlen(True));
			val->valType = value == 1 ? TT_TRUE : TT_FALSE;
			return val;
		}
		return NULL;
	}
}

Value* getVarAssignValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix);

Value* getBreakValue(Node* node){
	return node->nodeType == BREAK_NODE ? construct_Value(node->val) : NULL;
}

Value* getFunctionDefinitionValue(Node* node, Context* context){
    if(node->nodeType != FUNCTION_DEFINITION_NODE)
        return NULL;
    else if(node->else_block_Type == 0){
        return construct_Value(
            make_error(
                SyntaxError(
                    "function name or variable assigned to function",
                    node->val->line_no, node->val->col_no
                ),
                node->val->line_no, node->val->col_no
            )
        );
    }
    char* name = (char*)(((Token*)node->else_block)->val);
	char* num_params = INT_TO_STR(node->leftType);
	// char* temp_name = (char*)calloc(strlen(name) + strlen(num_params) + 5, sizeof(char));
	char* temp_name = (char*)allocate_ptr_array(strlen(name) + strlen(num_params) + 5, sizeof(char));
	strncpy(temp_name, name, strlen(name));
	temp_name[strlen(name)] = '\0';
	strcat(temp_name, num_params);
	name = temp_name;
	modify_context(context, name, node, FUNCTION_DEFINITION_NODE);
	// Token* token = (Token*)malloc(sizeof(Token));
	Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
	// memcpy(token, node->val, sizeof(Token));
	copy_heap_alloced_memory(token, node->val, sizeof(Token));
	token->type = TT_NULL;
	// token->val = (char*)calloc(5, sizeof(char));
	token->val = (char*)allocate_ptr_array(5, sizeof(char));
	strcpy((char*)(token->val), "null");
	return construct_Value(token);
}

// Value* viewNode(Node* node, Context* context, int* isNode){
// 	Value* answer;
// 	switch(node->nodeType){
// 		case NUM_NODE:{
// 			answer = getNumValue(node);
// 			break;
// 		}
// 		case BOOLEAN_NODE:{
// 			answer = getBooleanValue(node);
// 			break;
// 		}
// 		case NULL_NODE:{
// 			answer = getNullValue(node);
// 			break;
// 		}
// 		case BIN_OP_NODE:{
// 			answer = getBinOpValue(node, context);
// 			break;
// 		}
// 		case UN_OP_NODE:{
// 			answer = getUnOpValue(node, context);
// 			break;
// 		}
// 		case VAR_NODE:{
// 			answer = getVarValue(node, context);
// 			break;
// 		}
// 		case BREAK_NODE:{
// 			answer = getBreakValue(node);
// 			break;
// 		}
// 		case FUNCTION_DEFINITION_NODE:{
// 			answer = getFunctionDefinitionValue(node, context);
// 			break;
// 		}
// 		default:{
// 			answer = NULL;
// 		}
// 	}
// 	return answer;
// }

Value* getFunctionCallValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix);
Value* getMethodValue(Node* node, Context* context, int* isNode, int method_flag, void* needed_context, Node* fn_call, char* prefix, char* new_prefix);