#include "Parser.h"

typedef struct VALUE{
	void* num;
	int valType;
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
		default:
			printf("%p", value->num);
	}
}

Value* viewNode(Node* node);
Value* construct_Value(Token* token);

Value* addValues(Value* left, Value* right){
	if(left->valType == right->valType){
		switch(left->valType){
			case TT_INT:
				*((int*)left->num) += *((int*)right->num);
				break;
			
			case TT_FLOAT:
				*((double*)left->num) += *((double*)right->num);
				break;
		}
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

		*((double*)(answer->num)) += (double)(*((int*)(changer->num)));
		return answer;
	}
}

Value* subValues(Value* left, Value* right){
	if(left->valType == right->valType){
		switch(left->valType){
			case TT_INT:
				*((int*)left->num) -= *((int*)right->num);
				break;
			
			case TT_FLOAT:
				*((double*)left->num) -= *((double*)right->num);
				break;
		}
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

		*((double*)(answer->num)) -= (double)(*((int*)(changer->num)));
		return answer;
	}
}

Value* multiplyValues(Value* left, Value* right){
	if(left->valType == right->valType){
		switch(left->valType){
			case TT_INT:
				*((int*)left->num) *= *((int*)right->num);
				break;
			
			case TT_FLOAT:
				*((double*)left->num) *= *((double*)right->num);
				break;
		}
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

		*((double*)(answer->num)) *= (double)(*((int*)(changer->num)));
		return answer;
	}
}

Value* divideValues(Value* left, Value* right){
	double* num = (double*)malloc(sizeof(double));
	Value* answer = (Value*)malloc(sizeof(Value));
	*num = left->valType == TT_INT ? (double)(*((int*)(left->num))) 
			: *((double*)(left->num));
	// printf("Num %lf\n", *num);
	*num /= right->valType == TT_INT ? (double)(*(int*)(right->num))
			: *((double*)(right->num));
	// printf("Num %lf\n", *num);
	answer->num = num;
	answer->valType = TT_FLOAT;
	return answer;
}

Value* construct_Value(Token* token){
	Value* value = (Value*)malloc(sizeof(Value));
	value->num = token->val;
	value->valType = token->type;
}

Value* getNumValue(Node* node){
	return node->nodeType != NUM_NODE ? NULL : construct_Value(node->val);
}

Value* getBinOpValue(Node* node){
	if(node->nodeType != BIN_OP_NODE)
		return NULL;
	else{
		Value* leftValue = viewNode((Node*)node->left);
		Value* rightValue = viewNode((Node*)node->right);
		switch(node->valType){
			case TT_ADD:
				return addValues(leftValue, rightValue);
				break;
			case TT_SUB:
				return subValues(leftValue, rightValue);
				break;
			case TT_MUL:
				return multiplyValues(leftValue, rightValue);
				break;
			case TT_DIV:
				return divideValues(leftValue, rightValue);
				break;
			default:
				return NULL;
		}
	}
}

Value* getUnOpValue(Node* node){
	if(node->nodeType != UN_OP_NODE)
		return NULL;
	else{
		if(node->valType == TT_SUB){
			Value* val = viewNode((Node*)node->right);
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

Value* viewNode(Node* node){
	Value* answer;
	switch(node->nodeType){
		case NUM_NODE:{
			answer = getNumValue(node);
			break;
		}

		case BIN_OP_NODE:{
			answer = getBinOpValue(node);
			break;
		}

		case UN_OP_NODE:{
			answer = getUnOpValue(node);
			break;
		}

		default:{
			answer = NULL;
		}
	}
	return answer;
}