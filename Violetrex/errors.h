#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "symbols.h"

#define ILLEGAL_CHAR_ERROR 0
#define EOF_ERROR 1
#define SYNTAX_ERROR 2
#define DIVISION_BY_ZERO_ERROR 3
#define VALUE_NOT_FOUND_ERROR 4
#define NULL_OPERATION_ERROR 5
#define FUNCTION_NOT_FOUND_ERROR 6
#define KEY_NOT_FOUND_ERROR 7
#define INVALID_ARRAY_OPERATION_ERROR 8
#define INDEX_ERROR 9
#define MISMATCH_ERROR 10
#define INVALID_FUNCTION_NAME_ERROR 11
#define INVALID_STATIC_FUNCTION_NAME_ERROR 12
#define CLASS_SYNTAX_ERROR 13
#define METHOD_NOT_FOUND_ERROR 14
#define STATIC_VAR_NOT_FOUND_ERROR 15
#define INSTANCE_VAR_NOT_FOUND_ERROR 16

typedef struct ERROR{
    int errType;
    char* error_statement;
    int line_no, col_no;
} Error;

Error* construct_Error(int type, char statement[], int line_no, int col_no){
    // Error* error = (Error*)malloc(sizeof(Error));
    Error* error = (Error*)allocate_ptr_for_size(sizeof(Error));
    error->errType = type;
    // error->error_statement = (char*)calloc(2 * strlen(statement), sizeof(char));
    error->error_statement = (char*)allocate_ptr_array(2 * strlen(statement), sizeof(char));
    strncpy(error->error_statement, statement, strlen(statement));
    error->error_statement[strlen(statement)] = '\0';
    error->line_no = line_no;
    error->col_no = col_no;
    return error;
}

void printError(Error* error){
    switch(error->errType){
        case ILLEGAL_CHAR_ERROR:{
            printf("\nIllegalCharacterError: ");
            break;
        }
        case EOF_ERROR:{
            printf("\nEOFError: ");
            break;
        }
        case SYNTAX_ERROR:{
            printf("\nSyntaxError: ");
            break;
        }
        case DIVISION_BY_ZERO_ERROR:{
            printf("\nDivisionByZeroError: ");
            break;
        }
        case VALUE_NOT_FOUND_ERROR:{
            printf("\nValueNotFoundError: ");
            break;
        }
        case NULL_OPERATION_ERROR:{
            printf("\nNullOperationError: ");
            break;
        }
        case FUNCTION_NOT_FOUND_ERROR:{
            printf("\nFunctionNotFoundError: ");
            break;
        }
        case KEY_NOT_FOUND_ERROR:{
            printf("\nKeyNotFoundError: ");
            break;
        }
        case INVALID_ARRAY_OPERATION_ERROR:{
            printf("\nInvalidArrayOperationError: ");
            break;
        }
        case INDEX_ERROR:{
            printf("\nIndexError: ");
            break;
        }
        case MISMATCH_ERROR:{
            printf("\nMisMatchError: ");
            break;
        }
        case INVALID_FUNCTION_NAME_ERROR:{
            printf("\nInvalidFunctionNameError: ");
            break;
        }
        case INVALID_STATIC_FUNCTION_NAME_ERROR:{
            printf("\nInvalidStaticFunctionNameError: ");
            break;
        }
        case CLASS_SYNTAX_ERROR:{
            printf("\nClassSyntaxError: ");
            break;
        }
        case METHOD_NOT_FOUND_ERROR:{
            printf("\nMethodNotFoundError: ");
            break;
        }
        default:{
            printf("\nError: ");
        }
    }
    printf("Line %d Col %d: ", error->line_no, error->col_no);
    printf("%s\n", error->error_statement);
}

Error* IllegalCharacterError(int line_no, int col_no, int count, ...){
    va_list list;
    va_start(list, count);
    char *x, *expected;
    x = va_arg(list, char*);
    if(count > 1)
        expected = va_arg(list, char*);
    char statement[100] = {"Unexpected character \'"};
    strncat(statement, x, strlen(x));
    char s2[] = {"\' Expected "}, s3 = '\'';
    if(count > 1){
        strncat(statement, s2, strlen(s2));
        char temp;
        int l = strlen(statement);
        for(int i = 0; i < strlen(expected); i++){
            statement[l] = s3;
            statement[l+1] = expected[i];
            statement[l+2] = s3;
            statement[l+3] = i < strlen(expected) - 1 ? ',' : '\0';
            l += 4;
        }
    }
    else
        strncat(statement, &s3, 1);
    return construct_Error(ILLEGAL_CHAR_ERROR, statement, line_no, col_no);
}

Error* EOFError(int line_no, int col_no){
    char statement[100] = {"End of file"};
    return construct_Error(EOF_ERROR, statement, line_no, col_no);
}

Error* SyntaxError(char* expected, int line_no, int col_no){
    char statement[100] = {"Expected "};
    char c = '\'';
    int l = strlen(statement);
    for(int i = 0; i < strlen(expected); i++){
        statement[l] = expected[i];
        l++;
    }
    statement[l] = '\0';
    return construct_Error(SYNTAX_ERROR, statement, line_no, col_no);
}

Error* DivisionByZeroError(int line_no, int col_no){
    char statement[] = {"Division of number by 0 given"};
    return construct_Error(DIVISION_BY_ZERO_ERROR, statement, line_no, col_no);
}

Error* ValueNotFoundError(char* varname, int line_no, int col_no){
    char statement[100] = {"Variable '"};
    strncpy(statement + strlen(statement), varname, strlen(varname));
    strcat(statement, "' not initialized");
    return construct_Error(VALUE_NOT_FOUND_ERROR, statement, line_no, col_no);
}

Error* FunctionNotFoundError(char* varname, char* no_of_params, int line_no, int col_no){
    char statement[100] = {"Function '"};
    strncpy(statement + strlen(statement), varname, strlen(varname));
    strcat(statement, "' with ");
    strcat(statement, no_of_params);
    strcat(statement, " parameters not defined");
    return construct_Error(FUNCTION_NOT_FOUND_ERROR, statement, line_no, col_no);
}

Error* NullOperationError(char* op, int line_no, int col_no){
    char statement[100] = {"Null value cannot be used by operator '"};
    strcat(statement, op);
    strcat(statement, "'");
    return construct_Error(NULL_OPERATION_ERROR, statement, line_no, col_no);
}

Error* InvalidArrayOperationError(char* arr_name, char* op, int line_no, int col_no){
    char statement[200] = {"Invalid Operation '"};
    strcat(statement, op);
    strcat(statement, "'");
    strcat(statement, " on array '");
    strcat(statement, arr_name);
    strcat(statement, "'");
    return construct_Error(INVALID_ARRAY_OPERATION_ERROR, statement, line_no, col_no);
}

Error* KeyNotFoundError(void* val, int type, int line_no, int col_no){
    char statement[100] = {"Cannot find key: "};
    strcat(statement, value_to_string_eq(val, type));
    return construct_Error(KEY_NOT_FOUND_ERROR, statement, line_no, col_no);
}

Error* IndexError(char* var_name, int line_no, int col_no){
    char statement[100] = {"Illegal Indexing on variable '"};
    int length = strlen(statement) + strlen(var_name);
    strncat(statement, var_name, strlen(var_name));
    statement[length] = '\0';
    strcat(statement, "' which is not an array");
    return construct_Error(INDEX_ERROR, statement, line_no, col_no);
}

Error* MisMatchError(char* var_name, int line_no, int col_no){
    char statement[100] = {"Mismatched assignment to indices on variable '"};
    int length = strlen(statement) + strlen(var_name);
    strncat(statement, var_name, strlen(var_name));
    statement[length] = '\0';
    strcat(statement, "'");
    return construct_Error(MISMATCH_ERROR, statement, line_no, col_no);
}

Error* InvalidFunctionNameError(char* var_name, int nodeType, int line_no, int col_no){
    char statement[100] = {"Invalid name for function '"};
    int length = strlen(statement) + strlen(var_name);
    strncat(statement, var_name, strlen(var_name));
    statement[length] = '\0';
    switch(nodeType){
        case 17:{
            strcat(statement, "[...]'");
            break;
        }
        case 14:{
            strcat(statement, "(...)'");
            break;
        }
        default:
            strcat(statement, "'");
    }
    return construct_Error(INVALID_FUNCTION_NAME_ERROR, statement, line_no, col_no);
}

Error* InvalidStaticFunctionNameError(char* var_name, int nodeType, int line_no, int col_no){
    char statement[100] = {"Invalid name for static function '"};
    int length = strlen(statement) + strlen(var_name);
    strncat(statement, var_name, strlen(var_name));
    statement[length] = '\0';
    switch(nodeType){
        case 17:{
            strcat(statement, "[...]'");
            break;
        }
        case 14:{
            strcat(statement, "(...)'");
            break;
        }
        default:
            strcat(statement, "'");
    }
    return construct_Error(INVALID_STATIC_FUNCTION_NAME_ERROR, statement, line_no, col_no);
}

Error* ClassSyntaxError(char* statement, int line_no, int col_no){
    return construct_Error(CLASS_SYNTAX_ERROR, statement, line_no, col_no);
}

Error* MethodNotFoundError(char* varname, char* no_of_params, int line_no, int col_no){
    char statement[100] = {"Method '"};
    strncpy(statement + strlen(statement), varname, strlen(varname));
    strcat(statement, "' with ");
    strcat(statement, no_of_params);
    strcat(statement, " parameters not defined");
    return construct_Error(METHOD_NOT_FOUND_ERROR, statement, line_no, col_no);
}

Error* StaticVarNotFoundError(char* varname, char* className, int line_no, int col_no){
    char statement[200] = {"Variable '"};
    strncpy(statement + strlen(statement), varname, strlen(varname));
    strcat(statement, "' in class '");
    strcat(statement, className);
    strcat(statement, "' not initialized");
    return construct_Error(STATIC_VAR_NOT_FOUND_ERROR, statement, line_no, col_no);
}

Error* InstanceVarNotFoundError(char* varname, char* className, int line_no, int col_no){
    char statement[200] = {"Variable '"};
    strncpy(statement + strlen(statement), varname, strlen(varname));
    strcat(statement, "' in object of class '");
    strcat(statement, className);
    strcat(statement, "' not initialized");
    return construct_Error(INSTANCE_VAR_NOT_FOUND_ERROR, statement, line_no, col_no);
}