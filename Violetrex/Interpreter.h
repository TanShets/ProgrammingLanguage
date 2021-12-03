#pragma once
#include "value.h"

#define DEFAULT_NO_OF_VALUES 10

typedef struct INTERPRETER{
    Value** values;
    int no_of_values;
} Interpreter;

Interpreter* construct_Interpreter(Value** values, int no_of_values){
    Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    interpreter->values = values;
    interpreter->no_of_values = no_of_values;
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

Value** getConditionalNodeValue(Node* node, int* no_of_values, Context* context){
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
        values = (Value**)calloc(block_lengths[i], sizeof(Value*));
        // block = blocks[i];
        *no_of_values = block_lengths[i];
        for(j = 0; j < block_lengths[i]; j++){
            values[j] = viewNode(blocks[i][j], context);
            if(values[j]->valType == TT_ERROR){
                values = &values[j];
                *no_of_values = 1;
                return values;
            }
        }
        return values;
    }
    else{
        if(node->else_block != NULL){
            values = (Value**)calloc(node->isVarNode, sizeof(Value*));
            // block = blocks[i];
            block = node->else_block;
            *no_of_values = node->isVarNode;
            for(j = 0; j < node->isVarNode; j++){
                values[j] = viewNode(block[j], context);
                if(values[j]->valType == TT_ERROR){
                    values = &values[j];
                    *no_of_values = 1;
                    return values;
                }
            }
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

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context){
    Value **values, **temp_values;
    int no_of_values = 1, total_no_of_values = 0;
    int size = DEFAULT_NO_OF_VALUES;
    int i = 0;
    values = (Value**)calloc(size, sizeof(Value*));
    for(i = 0; i < no_of_nodes; i++){
        no_of_values = 1;
        switch(nodes[i]->nodeType){
            case CONDITIONAL_NODE:{
                temp_values = getConditionalNodeValue(nodes[i], &no_of_values, context);
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
    }
    return construct_Interpreter(values, total_no_of_values);
}

void printValues(Interpreter* interpreter){
    Value** values = interpreter->values;
    int no_of_values = interpreter->no_of_values;
    for(int i = 0; i < no_of_values; i++)
        printValue(values[i]);
}