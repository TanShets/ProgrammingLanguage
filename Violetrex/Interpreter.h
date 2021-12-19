#pragma once
#include "value.h"
#include "default_functions.h"

#define DEFAULT_NO_OF_VALUES 10

Interpreter* construct_Interpreter(Value** values, int no_of_values, int isBroken){
    Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    interpreter->values = values;
    interpreter->no_of_values = no_of_values;
    interpreter->isBroken = isBroken;
    return interpreter;
}

void expand_Values(Value*** values, int* no_of_values, int new_no_of_values){
    int old_no_of_values = *no_of_values;
    *no_of_values = 2 * new_no_of_values;
    Value** new_values = (Value**)calloc(*no_of_values, sizeof(Value*));
    memcpy(new_values, *values, old_no_of_values * sizeof(Value*));
    Value** temp = *values;
    *values = new_values;
    free(temp);
}

Value* getFunctionCallValue(Node* node, Context* context, int* isNode);

Value** getConditionalNodeValue(Node* node, int* no_of_values, Context* context, int* isNode){
    if(node->nodeType != CONDITIONAL_NODE)
        return NULL;
    
    int n = node->leftType;
    Node** statements = (Node**)node->left;
    Node*** blocks = (Node***)node->right;
    Node** block;
    int* block_lengths = node->block_lengths;
    Value* temp_val;
    Value** values;
    int i = 0, flag = 0, j;
    Interpreter* temp_interpreter;
    while(i < n){
        temp_val = viewNode(statements[i], context, isNode);
        switch(temp_val->valType){
            case TT_TRUE:
                flag = 1;
                break;
            case TT_INT:{
                if(*((int*)temp_val->num) != 0)
                    flag = 1;
                break;
            }
            case TT_FLOAT:{
                if(*((double*)temp_val->num) != 0)
                    flag = 1;
                break;
            }
        }

        if(flag == 0)
            i++;
        else
            break;
    }

    if(i < n){
        temp_interpreter = Interpret(blocks[i], block_lengths[i], context, isNode);
        *no_of_values = temp_interpreter->no_of_values;
        values = temp_interpreter->values;
        // values = (Value**)calloc(block_lengths[i], sizeof(Value*));
        // // block = blocks[i];
        // *no_of_values = block_lengths[i];
        // for(j = 0; j < block_lengths[i]; j++){
        //     values[j] = viewNode(blocks[i][j], context, isNode);
        //     if(values[j]->valType == TT_ERROR){
        //         values = &values[j];
        //         *no_of_values = 1;
        //         return values;
        //     }
        // }
        return values;
    }
    else{
        if(node->else_block != NULL){
            temp_interpreter = Interpret(node->else_block, node->isVarNode, context, isNode);
            *no_of_values = temp_interpreter->no_of_values;
            values = temp_interpreter->values;
            // values = (Value**)calloc(node->isVarNode, sizeof(Value*));
            // // block = blocks[i];
            // block = node->else_block;
            // *no_of_values = node->isVarNode;
            // for(j = 0; j < node->isVarNode; j++){
            //     values[j] = viewNode(block[j], context, isNode);
            //     if(values[j]->valType == TT_ERROR){
            //         values = &values[j];
            //         *no_of_values = 1;
            //         return values;
            //     }
            // }
            return values;
        }
        else{
            *no_of_values = 1;
            values = (Value**)malloc(sizeof(Value*));
            Token* token = (Token*)malloc(sizeof(Token));
            memcpy(token, node->val, sizeof(Token));
            token->type = TT_NULL;
            token->val = (int*)malloc(sizeof(int));
            *((int*)token->val) = 0;
            *values = construct_Value(token);
            return values;
        }
    }
}

Value* viewNode(Node* node, Context* context, int* isNode){
	Value* answer = NULL;
	switch(node->nodeType){
        case VAR_ASSIGN_NODE:{
            answer = getVarAssignValue(node, context, isNode);
            break;
        }
		case NUM_NODE:{
			answer = getNumValue(node);
			break;
		}
		case BOOLEAN_NODE:{
			answer = getBooleanValue(node);
			break;
		}
		case NULL_NODE:{
			answer = getNullValue(node);
			break;
		}
		case BIN_OP_NODE:{
			answer = getBinOpValue(node, context, isNode);
			break;
		}
		case UN_OP_NODE:{
			answer = getUnOpValue(node, context, isNode);
			break;
		}
		case VAR_NODE:{
			answer = getVarValue(node, context);
			break;
		}
		case BREAK_NODE:{
			answer = getBreakValue(node);
			break;
		}
		case FUNCTION_DEFINITION_NODE:{
			answer = getFunctionDefinitionValue(node, context);
			break;
		}
        case STRING_NODE:{
            answer = getStringValue(node);
            break;
        }
        case FUNCTION_CALL_NODE:{
            answer = getFunctionCallValue(node, context, isNode);
            break;
        }
		default:{
			answer = NULL;
		}
	}
	return answer;
}

int convertToBoolean(Value* value){
    switch(value->valType){
        case TT_TRUE:
            return 1;
        case TT_FALSE:
            return 0;
        case TT_NULL:
            return 0;
        case TT_INT:{
            return *((int*)(value->num)) == 0 ? 0 : 1;
        }
        case TT_FLOAT:{
            return *((double*)(value->num)) == 0 ? 0 : 1;
        }
        default:
            return 0;
    }
}

int isNegativeValue(Value* value){
    switch(value->valType){
        case TT_TRUE:
            return 0;
        case TT_FALSE:
            return 0;
        case TT_NULL:
            return 0;
        case TT_INT:{
            return *((int*)(value->num)) >= 0 ? 0 : 1;
        }
        case TT_FLOAT:{
            return *((double*)(value->num)) >= 0 ? 0 : 1;
        }
        default:
            return 0;
    }
}

Value** getLoopNodeValue(Node* node, int* no_of_values, Context* context, int* isNode){
    Value** values = (Value**)calloc(DEFAULT_NO_OF_VALUES, sizeof(Value*));
    int max_no_of_values = DEFAULT_NO_OF_VALUES;
    int i, hasStarted = 0;
    *no_of_values = 0;
    Value* temp_value;
    Value* statement_outcome = NULL;
    Interpreter* temp_interpreter;
    int flag = 0;
    if(node->val->type == TT_WHILE){
        //statement_outcome = viewNode(node->left, context, isNode);
        while(convertToBoolean(viewNode(node->left, context, isNode))){
            isNode[0]++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isNode);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            if(isNode[0] < 0){
                isNode[0] *= -1;
                flag = 1;
            }
            else if(isNode[1] < 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            else if(isNode[2] > 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            isNode[0]--;
            if(flag == 1)
                break;
        }
    }
    else{
        Node* condition = (Node*)(node->left);
        // left = loop condition, right = loop block
        Node* toNode = (Node*)(condition->right);
        // condition: x = a to b change = c, left=x, right= (a to b change = c)
        Node* change_node = (Node*)(toNode->right);
        // a to b change = c, right: change = c
        Node* toVals = (Node*)(toNode->left);
        // a to b change = c, left: a to b
        Value* change_val = viewNode(change_node, context, isNode);
        condition->right = (Node*)toVals->left;
        // a to b, left: a, right: b
        temp_value = viewNode(condition, context, isNode);
        // Assigns iterator which in this case x to a
        //From now the iterator will be incremented by value change until value b is attained or surpassed
        int criterion = isNegativeValue(change_val) ? TT_GREATER_THAN : TT_LESS_THAN;
        Value* right_end = viewNode(toVals->right, context, isNode);
        while(operateValues(temp_value, right_end, criterion, 0)->valType == TT_TRUE){
            if(hasStarted == 0){
                hasStarted = 1;
                condition->val->type = TT_INCREMENT;
                condition->right = change_node;
                condition->rightType = change_node->nodeType;
            }
            isNode[0]++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isNode);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            temp_value = viewNode(condition, context, isNode);
            if(isNode[0] < 0){
                isNode[0] *= -1;
                flag = 1;
            }
            else if(isNode[1] < 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            else if(isNode[2] > 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            isNode[0]--;
            if(flag == 1)
                break;
        }
    }
    return values;
}

Value* getFunctionCallValue(Node* node, Context* context, int* isNode){
    Value* tempo_vals = default_function_control(node, context, isNode);
    if(tempo_vals != NULL)
        return tempo_vals;
    Value** values = (Value**)malloc(sizeof(Value*));
    int no_of_parameters = node->leftType;
    Node** param_vals = (Node**)node->left;
    char* key = (char*)(((Token*)node->val)->val);
    char* num = INT_TO_STR(no_of_parameters);
    char* temp_key = (char*)calloc(strlen(key) + strlen(num) + 5, sizeof(char));
    strncpy(temp_key, key, strlen(key));
    strcat(temp_key, num);
    char* old_key = key;
    key = temp_key;
    void** results = search_from_context(context, key);
    if(*((int*)results[1]) != FUNCTION_DEFINITION_NODE)
    {
        *values = construct_Value(
            make_error(
                FunctionNotFoundError(
                    old_key, num, ((Token*)node->val)->line_no,
                    ((Token*)node->val)->col_no
                ),
                ((Token*)node->val)->line_no,
                ((Token*)node->val)->col_no
            )
        );
        isNode[2]++;
        return *values;
    }

    Node* fn_defn = (Node*)results[0];
    int block_size = fn_defn->rightType;
    Node** block = (Node**)fn_defn->right;
    Node** parameters = (Node**)fn_defn->left;
    int i;
    Value* temp_value;
    Interpreter* temp_interpreter;
    Context* function_context = construct_Context();
    function_context->parent = context;
    for(i = 0; i < no_of_parameters; i++){
        key = (char*)(((Token*)parameters[i]->val)->val);
        temp_value = viewNode(param_vals[i], context, isNode);
        modify_context(function_context, key, temp_value->num, temp_value->valType);
    }

    isNode[1]++;
    temp_interpreter = Interpret(block, block_size, function_context, isNode);
    if(temp_interpreter->no_of_values == 1 && (isNode[1] < 0 || isNode[2] > 0)){
        if(isNode[1] < 0)
            isNode[1] *= -1;
        return *(temp_interpreter->values);
    }
    else{
        Token* token = (Token*)malloc(sizeof(Token));
        token->type = TT_NULL;
        char* word = {"null"};
        token->val = word, token->line_no = ((Token*)node->val)->line_no;
        token->col_no = ((Token*)node->val)->col_no;
        return construct_Value(token);
    }
}

Value* getVarAssignValue(Node* node, Context* context, int* isNode){
	char* key = (char*)(((Token*)node->left)->val);
	// printf("%s: ", key);
	// printNode((Node*)(node->right), 0);
    Node* nodes = (Node*)node->right;
    Interpreter* temp_interpreter = Interpret(&nodes, 1, context, isNode);
    int line_no = ((Token*)node->val)->line_no;
    int col_no = ((Token*)node->val)->col_no;
    if(temp_interpreter->no_of_values != 1){
        return construct_Value(
            make_error(SyntaxError("a specific value", line_no, col_no),
            line_no, col_no)
        );
    }
    else if(isNode[2] > 0)
        return *(temp_interpreter->values);
	Value* val = *(temp_interpreter->values);
	Value* val2;
	Token* token;
	void** answer = NULL;
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
	
	// printValue(val);
	modify_context(context, key, val->num, val->valType);
	
	if(node->isVarNode != 0){
		return val;
    }
	token = (Token*)malloc(sizeof(Token));
	memcpy(token, node->val, sizeof(Token));
	token->type = TT_NULL;
	token->val = (char*)calloc(5, sizeof(char));
	strcpy((char*)(token->val), "null");
	return construct_Value(token);
}

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context, int* isNode){
    Value **values, **temp_values;
    int no_of_values = 1, total_no_of_values = 0;
    int size = DEFAULT_NO_OF_VALUES;
    int i = 0;
    values = (Value**)calloc(size, sizeof(Value*));
    int breaker = 0;
    for(i = 0; i < no_of_nodes; i++){
        no_of_values = 1;
        switch(nodes[i]->nodeType){
            case CONDITIONAL_NODE:{
                temp_values = getConditionalNodeValue(nodes[i], &no_of_values, context, isNode);
                break;
            }
            case LOOP_NODE:{
                temp_values = getLoopNodeValue(nodes[i], &no_of_values, context, isNode);
                break;
            }
            case BREAK_NODE:{
                breaker++;
                Token* token = NULL;
                if(isNode[0] == 0){
                    no_of_values = 1;
                    token = make_error(
                        SyntaxError(
                            "statement instead of break in a non loop",
                            nodes[i]->val->line_no, nodes[i]->val->col_no
                        ), nodes[i]->val->line_no, nodes[i]->val->col_no
                    );
                    isNode[2]++;
                }
                else{
                    isNode[0] *= -1;
                    token = (Token*)malloc(sizeof(Token));
                    token->type = TT_NULL, token->line_no = nodes[i]->val->line_no;
                    token->col_no = nodes[i]->val->col_no;
                    token->val = (char*)calloc(5, sizeof(char));
                    strcpy((char*)(token->val), "null");
                }
                no_of_values = 1;
                temp_values = (Value**)malloc(sizeof(Value*));
                *temp_values = construct_Value(token);
                break;
            }
            case RETURN_NODE:{
                breaker++;
                Token* token = NULL;
                temp_values = (Value**)malloc(sizeof(Value*));
                if(isNode[1] == 0){
                    no_of_values = 1;
                    token = make_error(
                        SyntaxError(
                            "statement instead of return outside of any function",
                            nodes[i]->val->line_no, nodes[i]->val->col_no
                        ), nodes[i]->val->line_no, nodes[i]->val->col_no
                    );
                    no_of_values = 1;
                    *temp_values = construct_Value(token);
                    isNode[2]++;
                }
                else{
                    no_of_values = 1;
                    isNode[1] *= -1;
                    *temp_values = viewNode((Node*)(nodes[i]->right), context, isNode);
                }
                break;
            }
            default:{
                no_of_values = 1;
                temp_values = (Value**)malloc(sizeof(Value*));
                *temp_values = viewNode(nodes[i], context, isNode);
            }
        }

        if(no_of_values == 1 && (*temp_values)->valType == TT_ERROR){
            isNode[2]++;
        }

        if(total_no_of_values + no_of_values >= size)
            expand_Values(&values, &size, total_no_of_values + no_of_values);
        memmove(values + total_no_of_values, temp_values, no_of_values * sizeof(Value*));
        total_no_of_values += no_of_values;

        if(isNode[1] < 0 || isNode[2] > 0){
            total_no_of_values = 1;
            values = temp_values;
        }

        if(breaker > 0 || isNode[2] > 0)
            break;
    }
    return construct_Interpreter(values, total_no_of_values, breaker);
}

void printValues(Interpreter* interpreter){
    Value** values = interpreter->values;
    int no_of_values = interpreter->no_of_values;
    for(int i = 0; i < no_of_values; i++){
        printValue(values[i]);
        printf("\n");
    }
}