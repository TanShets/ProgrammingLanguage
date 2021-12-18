#pragma once
#include "Lexer.h"
#include "Parser.h"
#include "value.h"

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context, int* isNode);

int to_int(double num){

}

Value* print_function(Node* node, Context* context, int* isNode){
    int no_of_parameters = node->leftType;
    Node** param_vals = (Node**)node->left;
    int i;
    Value* temp;
    for(i = 0; i < no_of_parameters; i++){
        temp = viewNode(param_vals[i], context, isNode);
        if(temp->valType == TT_ERROR){
            return temp;
        }
        printValue(temp);
        printf(" ");
    }
    printf("\n");
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = TT_NULL;
    char* null_word = {"null"};
    token->val = null_word;
    token->line_no = -1, token->col_no = -1;
    return construct_Value(token);
}

Value* input_function(Node* node, Context* context, int* isNode){
    int no_of_parameters = node->leftType;
    Node** param_vals = (Node**)node->left;
    Value* temp;
    Token* temp_token = (Token*)(node->val);
    int line_no = temp_token->line_no, col_no = temp_token->col_no;
    if(no_of_parameters > 1){
        temp = construct_Value(
            make_error(
                SyntaxError(
                    "no parameter or only single parameter like a string, Eg. \"input here: \"", 
                    line_no, col_no
                ),
                line_no, col_no
            )
        );
        return temp;
    }
    else if(no_of_parameters == 1){
        temp = viewNode(*param_vals, context, isNode);
        printValue(temp);
    }

    char c;
    char* word = (char*)calloc(STD_STRING_SIZE_LIMIT, sizeof(char));
    int max_size = STD_STRING_SIZE_LIMIT, temp_size = 0;

    while((c = getchar()) != EOF){
        if(c == '\n'){
            word[temp_size] = '\0';
            break;
        }

        word[temp_size] = c;
        temp_size++;
        if(temp_size == max_size)
            expand_string(&word, temp_size, &max_size);
    }

    Token* token = (Token*)malloc(sizeof(Token));
    token->type = TT_STRING, token->val = word;
    token->line_no = -1, token->col_no = -1;
    return construct_Value(token);
}

Value* convert_to_string(Node* node, Context* context, int* isNode){
    int no_of_parameters = node->leftType;
    Node** param_vals = (Node**)node->left;
    int line_no = ((Token*)(node->val))->line_no;
    int col_no = ((Token*)(node->val))->col_no;
    if(no_of_parameters != 1){
        Value* temp = construct_Value(
            make_error(
                SyntaxError(
                    "only a single parameter like a value, Eg. 5, 5.5, etc.", 
                    line_no, col_no
                ),
                line_no, col_no
            )
        );
        return temp;
    }
    Value* temp_val = viewNode(*param_vals, context, isNode);
    Value* answer = NULL;
    if(temp_val == NULL){
        temp_val = construct_Value(
            make_error(
                SyntaxError("Expected a value", line_no, col_no),
                line_no, col_no
            )
        );
        return temp_val;
    }

    char* str_val = NULL;
    char* temp_str = NULL;
    char* temp_str2 = NULL;
    switch(temp_val->valType){
        case TT_INT:{
            int num = *((int*)(temp_val->num));
            str_val = INT_TO_STR(num);
            break;
        }
        case TT_FLOAT:{
            double num = *((double*)(temp_val->num));
            // printf("Num: %lf\n", num);
            int int_part = (int)num;
            int zero_string = 0;
            num -= (double)int_part;
            while(num - (double)((int)(num + 1e-9)) > pow(10, -12)){
                // printf("ss: %.5lf %.5lf %.5lf\n", num, (double)((int)(num + 1e-10)), num - (double)((int)(num + 1e-10)));
                num *= 10;
                zero_string++;
                if(zero_string > 5)
                    break;
                //printf("Wait %lf\n", num);
            }
            int decimal_part = (int)(num + 1e-9);
            str_val = INT_TO_STR(int_part);
            temp_str = INT_TO_STR(decimal_part);
            int_part = strlen(str_val) + zero_string;

            temp_str2 = str_val;
            str_val = (char*)calloc(int_part * 2, sizeof(char));
            int length = 0;
            strncpy(str_val, temp_str2, strlen(temp_str2));
            length = strlen(temp_str2);
            str_val[length] = '\0';
            strncat(str_val, ".", 1);
            length++;
            str_val[length] = '\0';
            for(int i = 0; i < zero_string - strlen(temp_str); i++){
                strncat(str_val, "0", 1);
                length++;
                str_val[length] = '\0';
            }
            strncat(str_val, temp_str, strlen(temp_str));
            length += strlen(temp_str);
            str_val[length] = '\0';
            break;
        }
        case TT_STRING:
            str_val = (char*)calloc(strlen((char*)(temp_val->num)) + 2, sizeof(char));
            strncpy(str_val, (char*)(temp_val->num), strlen((char*)(temp_val->num)));
            str_val[strlen(str_val)] = '\0';
            break;
        case TT_NULL:{
            str_val = (char*)calloc(strlen((char*)(temp_val->num)) + 2, sizeof(char));
            strncpy(str_val, (char*)(temp_val->num), strlen((char*)(temp_val->num)));
            str_val[strlen(str_val)] = '\0';
            break;
        }
        case TT_TRUE:{
            str_val = (char*)calloc(strlen((char*)(temp_val->num)) + 2, sizeof(char));
            strncpy(str_val, (char*)(temp_val->num), strlen((char*)(temp_val->num)));
            str_val[strlen(str_val)] = '\0';
            break;
        }
        case TT_FALSE:{
            str_val = (char*)calloc(strlen((char*)(temp_val->num)) + 2, sizeof(char));
            strncpy(str_val, (char*)(temp_val->num), strlen((char*)(temp_val->num)));
            str_val[strlen(str_val)] = '\0';
            break;
        }
        case TT_ERROR:
            return temp_val;
        default:{
            answer = construct_Value(
                make_error(
                    SyntaxError("a convertible and defined value", line_no, col_no),
                    line_no, col_no
                )
            );
            return answer;
        }
    }

    Token* token = (Token*)malloc(sizeof(Token));
    token->type = TT_STRING, token->val = str_val;
    token->line_no = line_no, token->col_no = col_no;
    return construct_Value(token);
}

Value* default_function_control(Node* node, Context* context, int* isNode){
    char* key = (char*)(((Token*)node->val)->val);
    int function_num = GET_FUNCTION_CODE(key);
    Value* temp = NULL;
    switch(function_num){
        case INPUT_FN:
            return input_function(node, context, isNode);
        case PRINT_FN:
            return print_function(node, context, isNode);
        case STRING_FN:
            return convert_to_string(node, context, isNode);
        default:
            return temp;
    }
}