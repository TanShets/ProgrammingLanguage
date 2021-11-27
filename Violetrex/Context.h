#include "Parser.h"
#include "errors.h"
#pragma once
#define CONTEXT_SIZE 100000000
#define HASH_CONTEXT(num, i, size) ((i*i + 3*i + num % size) % size)

typedef struct CONTEXT{
    struct CONTEXT *parent;
    int context_size;
    void** values;
    char** keys;
    int* type;
} Context;

Context* construct_Context(){
    Context* context = (Context*)malloc(sizeof(Context));
    context->context_size = CONTEXT_SIZE;
    context->parent = NULL;
    context->values = (void**)calloc(context->context_size, sizeof(void*));
    context->keys = (char**)calloc(context->context_size, sizeof(char*));
    context->type = (int*)calloc(context->context_size, sizeof(int));
    memset(context->type, -1, context->context_size * sizeof(int));
    return context;
}

void add_to_context(Context* context, char* key, void* value, int type){
    int length = strlen(key);
    int i = 0;
    while(context->type[HASH_CONTEXT(length, i, context->context_size)] != -1)
        i++;
    int index = HASH_CONTEXT(length, i, context->context_size);
    context->keys[index] = key;
    context->values[index] = value;
    context->type[index] = type;
}

int search_index_from_context(Context* context, char* key){
    int length = strlen(key);
    int i = 0;
    while(
        context->type[HASH_CONTEXT(length, i, context->context_size)] != -1 && 
        strcmp(
            context->keys[HASH_CONTEXT(length, i, context->context_size)], key
        ) != 0
    )
        i++;
    return HASH_CONTEXT(length, i, context->context_size);
}

void** search_from_context(Context* context, char* key){
    int length = strlen(key);
    void** answer = (void**)calloc(2, sizeof(void*));
    int* type = (int*)malloc(sizeof(int));
    
    int index = search_index_from_context(context, key);
    if(context->type[index] == -1 && context->parent == NULL)
    {
        *type = TT_NULL;
        answer[0] = NULL;
        answer[1] = type;
    }
    else if(context->type[index] == -1 && context->parent != NULL){
        answer = search_from_context(context->parent, key);
    }
    else{
        answer[0] = context->values[index];
        *type = context->type[index];
        answer[1] = type;
    }
    return answer;
}

void modify_context(Context* context, char* key, void* value, int type){
    int index = search_index_from_context(context, key);
    Context* temp = context;
    while(temp->type[index] == -1 && temp->parent != NULL){
        temp = temp->parent;
        index = search_index_from_context(temp, key);
    }

    if(temp->type[index] == -1)
        add_to_context(context, key, value, type);
    else{
        temp->type[index] = type;
        temp->values[index] = value;
    }
}