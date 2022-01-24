#include "Parser.h"
#include "errors.h"
#pragma once
#define CONTEXT_SIZE 10000
#define HASH_CONTEXT(num, i, size) ((i*i + 3*i + num % size) % size)

typedef struct CONTEXT{
    struct CONTEXT *parent;
    int context_size;
    int content_size;
    void** values;
    char** keys;
    int* type;
} Context;

Context* construct_Context(){
    // Context* context = (Context*)malloc(sizeof(Context));
    Context* context = (Context*)allocate_ptr_for_size(sizeof(Context));
    context->context_size = CONTEXT_SIZE;
    context->content_size = 0;
    context->parent = NULL;
    // context->values = (void**)calloc(context->context_size, sizeof(void*));
    // context->keys = (char**)calloc(context->context_size, sizeof(char*));
    // context->type = (int*)calloc(context->context_size, sizeof(int));
    context->values = (void**)allocate_ptr_array(context->context_size, sizeof(void*));
    context->keys = (char**)allocate_ptr_array(context->context_size, sizeof(char*));
    context->type = (int*)allocate_ptr_array(context->context_size, sizeof(int));
    // memset(context->type, -1, context->context_size * sizeof(int));
    set_heap_alloced_memory(context->type, -1, context->context_size * sizeof(int));
    return context;
}

void add_to_context(Context* context, char* key, void* value, int type);

void expand_context(Context* context){
    void** old_values = context->values;
    char** old_keys = context->keys;
    int* old_type = context->type;
    int old_context_size = context->context_size;
    context->context_size *= 2;
    // context->values = (void**)calloc(context->context_size, sizeof(void*));
    // context->keys = (char**)calloc(context->context_size, sizeof(char*));
    // context->type = (int*)calloc(context->context_size, sizeof(int));
    context->values = (void**)allocate_ptr_array(context->context_size, sizeof(void*));
    context->keys = (char**)allocate_ptr_array(context->context_size, sizeof(char*));
    context->type = (int*)allocate_ptr_array(context->context_size, sizeof(int));
    context->content_size = 0;
    // memset(context->type, -1, context->context_size * sizeof(int));
    set_heap_alloced_memory(context->type, -1, context->context_size * sizeof(int));

    for(int i = 0; i < old_context_size; i++){
        if(old_type[i] != -1)
            add_to_context(context, old_keys[i], old_values[i], old_type[i]);
    }
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
    context->content_size++;
    if(context->content_size >= context->context_size / 2)
        expand_context(context);
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
    // void** answer = (void**)calloc(2, sizeof(void*));
    void** answer = (void**)allocate_ptr_array(2, sizeof(void*));
    // int* type = (int*)malloc(sizeof(int));
    int* type = (int*)allocate_ptr_for_size(sizeof(int));
    
    int index = search_index_from_context(context, key);
    if(context->type[index] == -1 && context->parent == NULL)
    {
        *type = TT_ERROR;
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

void** search_from_local_context(Context* context, char* key){
    int length = strlen(key);
    // void** answer = (void**)calloc(2, sizeof(void*));
    void** answer = (void**)allocate_ptr_array(2, sizeof(void*));
    // int* type = (int*)malloc(sizeof(int));
    int* type = (int*)allocate_ptr_for_size(sizeof(int));
    
    int index = search_index_from_context(context, key);
    if(context->type[index] == -1)
    {
        *type = TT_ERROR;
        answer[0] = NULL;
        answer[1] = type;
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