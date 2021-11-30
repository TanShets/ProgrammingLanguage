#pragma once
#include "Parser.h"
#include "Context.h"

#define POINT_CASTER(x, dtype) ((dtype == TT_INT) ? (int*)x : (double*)x)
#define TYPE_CASTER(x, dtype1, dtype2) ( \
	dtype1 == TT_INT && dtype2 == TT_INT ? (int)(*((int*)x)) : \ 
	dtype1 == TT_FLOAT && dtype2 == TT_INT ? (int)(*((double*)x)) : \ 
	dtype1 == TT_INT && dtype2 == TT_FLOAT ? (double)(*((int*)x)) : \ 
	dtype1 == TT_FLOAT && dtype2 == TT_FLOAT ? (double)(*((double*)x)) : 0 \
)

typedef struct VALUE{
	void* num;
	int valType, line_no, col_no;
} Value;

void printValue(Value* value){
	switch(value->valType){
		case TT_INT:{
			printf("%d", *((int*)(value->num)));
			break;
		}
		case TT_FLOAT:{
			printf("%lf", *((double*)(value->num)));
			break;
		}
		case TT_ERROR:{
			printError((Error*)value->num);
			break;
		}
		case TT_TRUE:{
			printf("true");
			break;
		}
		case TT_FALSE:{
			printf("false");
			break;
		}
		case TT_NULL:{
			printf("null");
			break;
		}
		default:
			printf("%p", value->num);
	}
	printf("\n");
}

Value* viewNode(Node* node, Context* context);
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
			else
				*((double*)answer->num) += TYPE_CASTER(changer->num, typp, typo);
			// *(POINT_CASTER(answer->num, typo)) += TYPE_CASTER(changer->num, typo, typp);
			// *((POINT_CASTER(typo))answer->num) += (TYPE_CASTER(typo))(*((POINT_CASTER(typp))changer->num));
			break;
		case TT_SUB:
			if(typo == TT_INT)
				*((int*)answer->num) -= TYPE_CASTER(changer->num, typp, typo);
			else
				*((double*)answer->num) -= TYPE_CASTER(changer->num, typp, typo);
			break;
		case TT_MUL:
			if(typo == TT_INT)
				*((int*)answer->num) *= TYPE_CASTER(changer->num, typp, typo);
			else
				*((double*)answer->num) *= TYPE_CASTER(changer->num, typp, typo);
			break;
		case TT_DIV:
			if(typo == TT_INT)
				*((int*)answer->num) /= TYPE_CASTER(changer->num, typp, typo);
			else
				*((double*)answer->num) /= TYPE_CASTER(changer->num, typp, typo);
			break;
	}

	if(IN_BOOLEAN_OPERATORS(op_type)){
		Value* temp_answer = (Value*)malloc(sizeof(Value));
		memcpy(temp_answer, answer, sizeof(Value));
		int new_value = 0;
		switch(op_type){
			case TT_EQUALS:
				new_value = typo == TT_INT ? *((int*)answer->num) == TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_FLOAT ? *((double*)answer->num) == TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_NULL ? typo == typp : 0;
				break;
			case TT_NOT_EQUALS:
				new_value = typo == TT_INT ? *((int*)answer->num) != TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_FLOAT ? *((double*)answer->num) != TYPE_CASTER(changer->num, typp, typo) : 
							typo == TT_NULL ? typo != typp : 0;
				break;
			case TT_LESS_THAN:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) < TYPE_CASTER(changer->num, typp, typo);
				else
					new_value = *((double*)answer->num) < TYPE_CASTER(changer->num, typp, typo);
				break;
			case TT_LESS_THAN_EQ:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) <= TYPE_CASTER(changer->num, typp, typo);
				else
					new_value = *((double*)answer->num) <= TYPE_CASTER(changer->num, typp, typo);
				break;
			case TT_GREATER_THAN:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) > TYPE_CASTER(changer->num, typp, typo);
				else
					new_value = *((double*)answer->num) > TYPE_CASTER(changer->num, typp, typo);
				break;
			case TT_GREATER_THAN_EQ:
				if(typo == TT_INT)
					new_value = *((int*)answer->num) >= TYPE_CASTER(changer->num, typp, typo);
				else
					new_value = *((double*)answer->num) >= TYPE_CASTER(changer->num, typp, typo);
				break;
		}

		Value* new_answer = NULL;
		Token* new_token = (Token*)malloc(sizeof(Token));
		new_token->type = new_value == 1 ? TT_TRUE : TT_FALSE;
		new_token->val = (char*)calloc(STD_VAR_NAME_SIZE_LIMIT, sizeof(char));
		new_token->line_no = answer->line_no, new_token->col_no = answer->col_no;
		char *True = "true", *False = "false";
		if(new_value == 1)
			strncpy((char*)(new_token->val), True, strlen(True));
		else
			strncpy((char*)(new_token->val), False, strlen(False));
		new_answer = construct_Value(new_token);
		memcpy(answer, new_answer, sizeof(Value));
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
	else if(op_type != TT_EQUALS && op_type != TT_NOT_EQUALS){
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
		answer = left, changer = right;
		double* new_temp;
		if(left->valType == TT_INT){
			answer = (Value*)malloc(sizeof(Value));
			memcpy(answer, left, sizeof(Value));
			answer->valType = TT_FLOAT;
			new_temp = (double*)malloc(sizeof(double));
			*new_temp = (double)(*((int*)answer->num));
			answer->num = new_temp;
		}
		alterValues(answer, changer, op_type);
		return answer;
	}
}

Value* construct_Value(Token* token){
	Value* value = (Value*)malloc(sizeof(Value));
	value->num = token->val;
	value->valType = token->type;
	value->line_no = token->line_no, value->col_no = token->col_no;
}

Value* getNumValue(Node* node){
	return node->nodeType != NUM_NODE ? NULL : construct_Value(node->val);
}

Value* getNullValue(Node* node){
	return node->nodeType != NULL_NODE ? NULL : construct_Value(node->val);
}

Value* getVarValue(Node* node, Context* context){
	Token* nodeVal = (Token*)(node->val);
	char* key = (char*)(nodeVal->val);
	void** answer = search_from_context(context, key);
	int line_no = nodeVal->line_no, col_no = nodeVal->col_no;
	Token* token;
	if(answer[1] != NULL && *((int*)answer[1]) != TT_ERROR){
		token = (Token*)malloc(sizeof(Token));
		token->type = *((int*)answer[1]);
		token->val = answer[0];
		token->line_no = node->val->line_no, token->col_no = node->val->col_no;
		return construct_Value(token);
	}
	else
		return construct_Value(
			make_error(
				ValueNotFoundError(key, line_no, col_no), line_no, col_no
			)
		);
}

Value* getBinOpValue(Node* node, Context* context){
	if(node->nodeType != BIN_OP_NODE)
		return NULL;
	else{
		Value* leftValue = viewNode((Node*)node->left, context);
		Value* rightValue = viewNode((Node*)node->right, context);
		return operateValues(leftValue, rightValue, node->valType, 0);
	}
}

Value* getUnOpValue(Node* node, Context* context){
	if(node->nodeType != UN_OP_NODE)
		return NULL;
	else{
		if(node->valType == TT_SUB){
			Value* val = viewNode((Node*)node->right, context);
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
		return NULL;
	}
}

Value* getVarAssignValue(Node* node, Context* context){
	char* key = (char*)(((Token*)node->left)->val);
	Value* val = viewNode((Node*)(node->right), context);
	Value* val2;
	Token* token;
	void** answer = NULL;
	int line_no, col_no;
	if(node->val->type != TT_EQ){
		line_no = node->val->line_no;
		col_no = node->val->col_no;
		answer = search_from_context(context, key);
		if(answer[1] != NULL && *((int*)answer[1]) != TT_ERROR){
			token = (Token*)malloc(sizeof(Token));
			token->type = *((int*)answer[1]);
			token->val = answer[0];
			token->line_no = node->val->line_no, token->col_no = node->val->col_no;
			val2 = construct_Value(token);
			switch(node->val->type){
				case TT_INCREMENT:
					val = operateValues(val2, val, TT_ADD, 1);
					break;
				case TT_DECREMENT:
					val = operateValues(val2, val, TT_SUB, 1);
					break;
				case TT_PRODUCT_INCREMENT:
					val = operateValues(val2, val, TT_MUL, 1);
					break;
				case TT_PRODUCT_DECREMENT:
					val = operateValues(val2, val, TT_DIV, 1);
					break;
			}

			if(val->valType == TT_ERROR)
				return val;
		}
		else
			return construct_Value(
				make_error(
					ValueNotFoundError(key, line_no, col_no), line_no, col_no
				)
			);
	}
	
	modify_context(context, key, val->num, val->valType);
	
	if(node->isVarNode != 0)
		return val;
	token = (Token*)malloc(sizeof(Token));
	memcpy(token, node->val, sizeof(Token));
	token->type = TT_NULL;
	token->val = (int*)malloc(sizeof(int));
	*((int*)token->val) = 0;
	return construct_Value(token);
}

Value* viewNode(Node* node, Context* context){
	Value* answer;
	switch(node->nodeType){
		case VAR_ASSIGN_NODE:{
			answer = getVarAssignValue(node, context);
			break;
		}
		case NUM_NODE:{
			answer = getNumValue(node);
			break;
		}
		case NULL_NODE:{
			answer = getNullValue(node);
			break;
		}
		case BIN_OP_NODE:{
			answer = getBinOpValue(node, context);
			break;
		}
		case UN_OP_NODE:{
			answer = getUnOpValue(node, context);
			break;
		}
		case VAR_NODE:{
			answer = getVarValue(node, context);
			break;
		}
		default:{
			answer = NULL;
		}
	}
	return answer;
}