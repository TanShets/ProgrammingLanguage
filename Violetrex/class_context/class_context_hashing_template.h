#pragma once
#include "class_context_create.h"

#define HASH_CLASS_STATIC_VARS(num, size, i) ((num + 4 * i + 3 * i * i) % size)
#define HASH_CLASS_STATIC_METHODS(num, size, i) ((num + 5 * i + 2 * i * i) % size)
#define HASH_CLASS_INSTANCE_METHODS(num, size, i) ((num + 3 * i + 7 * i * i) % size)

int HASH_FN_FOR_CLASS_CONTEXT(int num, int size, int i, int flag){
    switch(flag){
        case 0:
            return HASH_CLASS_STATIC_VARS(num, size, i);
        case 1:
            return HASH_CLASS_STATIC_METHODS(num, size, i);
        case 2:
            return HASH_CLASS_INSTANCE_METHODS(num, size, i);
        default:
            return -1;
    }
}

int insert_into_ClassContext(
    char* key, int flag, char** key_array,
    int* no_of_vals, int* total_no_of_vals
){
    int i = 0;
    int num = strlen(key);
    int* size = total_no_of_vals;
    int index = HASH_FN_FOR_CLASS_CONTEXT(num, *size, i, flag);
    while(
        key_array[index] != NULL && 
        strcmp(key_array[index], null_key_for_class_context) != 0
    ){
        i++;
        index = HASH_FN_FOR_CLASS_CONTEXT(num, *size, i, flag);
    }

    (*no_of_vals)++;
    if(*no_of_vals >= *total_no_of_vals / 2){}

    return index;
}

int find_index_in_ClassContext(
    char* key, int size, int flag, char** key_array
){
    int i = 0;
    int index = -1;
    if(key == NULL) return index;
    int num = strlen(key);
    index = HASH_FN_FOR_CLASS_CONTEXT(num, size, i, flag);
    while(
        key_array[index] != NULL && 
        strcmp(key_array[index], key) != 0
    ){
        i++;
        index = HASH_FN_FOR_CLASS_CONTEXT(num, size, i, flag);
    }

    return key_array[index] == NULL ? -1 : index;
}

void* find_in_ClassContext(
    char* key, int size, int flag, char** key_array, void** value_array
){
    int index = find_index_in_ClassContext(key, size, flag, key_array);

    return index == -1 ? NULL : value_array[index];
}

void modify_in_ClassContext(
    char* className, char* key, void* value, int flag, 
    char** key_array, void** value_array, int* no_of_vals, int* total_no_of_vals
){
    int index = find_index_in_ClassContext(key, *total_no_of_vals, flag, key_array);
    if(index != -1){
        value_array[index] = value;
    }
    else{
        index = insert_into_ClassContext(key, flag, key_array, no_of_vals, total_no_of_vals);
        if(index == -1){
            printf("HashError: Could not store static variable in ClassContext: '%s'", className);
            exit(0);
        }
        key_array[index] = key;
        value_array[index] = value;
    }
}

void expand_in_ClassContext(
    char*** keys, void*** values, int* size, char* className, int flag, int* no_of_vals
){
    char** old_keys = *keys;
    void** old_vals = *values;
    int old_size = *size;
    *size *= 2;
    *keys = allocate_ptr_array(*size, sizeof(char*));
    *values = allocate_ptr_array(*size, sizeof(void*));
    *no_of_vals = 0;
    for(int i = 0; i < old_size; i++){
        if(old_keys[i] != NULL && strcmp(old_keys[i], null_key_for_class_context) != 0)
            modify_in_ClassContext(className, old_keys[i], old_vals[i], flag, *keys, *values, no_of_vals, size);
    }
}