#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ILLEGAL_CHAR_ERROR 0
#define EOF_ERROR 1
#define SYNTAX_ERROR 2
#define DIVISION_BY_ZERO_ERROR 2
#define VALUE_NOT_FOUND_ERROR 3
#define NULL_OPERATION_ERROR 4

typedef struct ERROR{
    int errType;
    char* error_statement;
    int line_no, col_no;
} Error;

Error* construct_Error(int type, char statement[], int line_no, int col_no){
    Error* error = (Error*)malloc(sizeof(Error));
    error->errType = type;
    error->error_statement = (char*)calloc(2 * strlen(statement), sizeof(char));
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

Error* NullOperationError(char* op, int line_no, int col_no){
    char statement[100] = {"Null value cannot be used by operator '"};
    strcat(statement, op);
    strcat(statement, "'");
    return construct_Error(NULL_OPERATION_ERROR, statement, line_no, col_no);
}