#pragma once
#include "Parser.h"
#include "Context.h"

#define POINT_CASTER(x, dtype) ((dtype == TT_INT) ? (int*)x : (double*)x)
#define TYPE_CASTER(x, dtype1, dtype2) (dtype1 == TT_INT && dtype2 == TT_INT ? (int)(*((int*)x)) : dtype1 == TT_FLOAT && dtype2 == TT_INT ? (double)(*((int*)x)) : dtype1 == TT_INT && dtype2 == TT_FLOAT ? (int)(*((double*)x)) : dtype1 == TT_FLOAT && dtype2 == TT_FLOAT ? (double)(*((double*)x)) : 0)

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
	int* temp1 = (int*)answer->num;
	double* temp2 = (double*)answer->num;

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
}

Value* operateValues(Value* left, Value* right, int op_type){
	if(op_type == TT_DIV){
		if(TYPE_CASTER(right->num, right->valType, left->valType) == 0)
			return construct_Value(
				make_error(
					DivisionByZeroError(right->line_no, right->col_no),
					right->line_no, right->col_no
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
		if(left->valType == TT_INT){
			answer = right, changer = left;
		}
		else{
			answer = left, changer = right;
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

Value* getVarValue(Node* node, Context* context){
	Token* nodeVal = (Token*)(node->val);
	char* key = (char*)(nodeVal->val);
	void** answer = search_from_context(context, key);
	int line_no = nodeVal->line_no, col_no = nodeVal->col_no;
	Token* token;
	if(answer[1] != NULL && *((int*)answer[1]) != TT_NULL){
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
		return operateValues(leftValue, rightValue, node->valType);
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
	modify_context(context, key, val->num, val->valType);
	if(node->isVarNode != 0)
		return val;
	Token* token = (Token*)malloc(sizeof(Token));
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