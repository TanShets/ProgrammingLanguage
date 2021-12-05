#pragma once
#include "value.h"

#define DEFAULT_NO_OF_VALUES 10

typedef struct INTERPRETER{
    Value** values;
    int no_of_values;
    int isBroken;
} Interpreter;

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

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context, int* isLoopNode);

Value** getConditionalNodeValue(Node* node, int* no_of_values, Context* context, int* isLoopNode){
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
        temp_val = viewNode(statements[i], context);
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
        temp_interpreter = Interpret(blocks[i], block_lengths[i], context, isLoopNode);
        *no_of_values = temp_interpreter->no_of_values;
        values = temp_interpreter->values;
        // values = (Value**)calloc(block_lengths[i], sizeof(Value*));
        // // block = blocks[i];
        // *no_of_values = block_lengths[i];
        // for(j = 0; j < block_lengths[i]; j++){
        //     values[j] = viewNode(blocks[i][j], context);
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
            temp_interpreter = Interpret(node->else_block, node->isVarNode, context, isLoopNode);
            *no_of_values = temp_interpreter->no_of_values;
            values = temp_interpreter->values;
            // values = (Value**)calloc(node->isVarNode, sizeof(Value*));
            // // block = blocks[i];
            // block = node->else_block;
            // *no_of_values = node->isVarNode;
            // for(j = 0; j < node->isVarNode; j++){
            //     values[j] = viewNode(block[j], context);
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

Value** getLoopNodeValue(Node* node, int* no_of_values, Context* context, int* isLoopNode){
    Value** values = (Value**)calloc(DEFAULT_NO_OF_VALUES, sizeof(Value*));
    int max_no_of_values = DEFAULT_NO_OF_VALUES;
    int i, hasStarted = 0;
    *no_of_values = 0;
    Value* temp_value;
    Value* statement_outcome = NULL;
    Interpreter* temp_interpreter;
    int flag = 0;
    if(node->val->type == TT_WHILE){
        //statement_outcome = viewNode(node->left, context);
        while(convertToBoolean(viewNode(node->left, context))){
            (*isLoopNode)++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isLoopNode);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            if(*isLoopNode < 0){
                *isLoopNode *= -1;
                flag = 1;
            }
            (*isLoopNode)--;
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
        Value* change_val = viewNode(change_node, context);
        condition->right = (Node*)toVals->left;
        // a to b, left: a, right: b
        temp_value = viewNode(condition, context);
        // Assigns iterator which in this case x to a
        //From now the iterator will be incremented by value change until value b is attained or surpassed
        int criterion = isNegativeValue(change_val) ? TT_GREATER_THAN : TT_LESS_THAN;
        Value* right_end = viewNode(toVals->right, context);
        while(operateValues(temp_value, right_end, criterion, 0)->valType == TT_TRUE){
            if(hasStarted == 0){
                hasStarted = 1;
                condition->val->type = TT_INCREMENT;
                condition->right = change_node;
                condition->rightType = change_node->nodeType;
            }
            (*isLoopNode)++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isLoopNode);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            temp_value = viewNode(condition, context);
            if(*isLoopNode < 0){
                *isLoopNode *= -1;
                flag = 1;
            }
            (*isLoopNode)--;
            if(flag == 1)
                break;
        }
    }
    return values;
}

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context, int* isLoopNode){
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
                temp_values = getConditionalNodeValue(nodes[i], &no_of_values, context, isLoopNode);
                break;
            }
            case LOOP_NODE:{
                temp_values = getLoopNodeValue(nodes[i], &no_of_values, context, isLoopNode);
                break;
            }
            case BREAK_NODE:{
                breaker++;
                Token* token = NULL;
                if(*isLoopNode == 0){
                    no_of_values = 1;
                    token = make_error(
                        SyntaxError(
                            "statement instead of break in a non loop",
                            nodes[i]->val->line_no, nodes[i]->val->col_no
                        ), nodes[i]->val->line_no, nodes[i]->val->col_no
                    );
                }
                else{
                    *isLoopNode *= -1;
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
            default:
                no_of_values = 1;
                temp_values = (Value**)malloc(sizeof(Value*));
                *temp_values = viewNode(nodes[i], context);
        }
        if(total_no_of_values + no_of_values >= size)
            expand_Values(&values, &size, total_no_of_values + no_of_values);
        memmove(values + total_no_of_values, temp_values, no_of_values * sizeof(Value*));
        total_no_of_values += no_of_values;

        if(breaker > 0)
            break;
    }
    return construct_Interpreter(values, total_no_of_values, breaker);
}

void printValues(Interpreter* interpreter){
    Value** values = interpreter->values;
    int no_of_values = interpreter->no_of_values;
    printf("No of vals: %d\n", no_of_values);
    for(int i = 0; i < no_of_values; i++)
        printValue(values[i]);
}