#pragma once
#include "class_context_create.h"

#define HASH_CLASS_STATIC_VARS(num, size, i) ((num + 4 * i + 3 * i * i) % size)
#define HASH_CLASS_STATIC_METHODS(num, size, i) ((num + 5 * i + 2 * i * i) % size)
#define HASH_CLASS_INSTANCE_METHODS(num, size, i) ((num + 3 * i + 7 * i * i) % size)
#define HASH_CLASS_SUPERCLASS(num, size, i) ((num + 4 * i + 7 * i * i) % size)

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

void expand_in_ClassContext(char*** keys, void*** values, int* size, char* className, int flag, int* no_of_vals);

int insert_into_ClassContext(
    char* key, int flag, char*** key_array_ptr, int* no_of_vals, 
    int* total_no_of_vals, void*** value_array_ptr, char* className
){
    int i = 0;
    int num = strlen(key);
    int* size = total_no_of_vals;
    char** key_array = *key_array_ptr;
    int index = HASH_FN_FOR_CLASS_CONTEXT(num, *size, i, flag);
    while(
        key_array[index] != NULL && 
        strcmp(key_array[index], null_key_for_class_context) != 0
    ){
        i++;
        index = HASH_FN_FOR_CLASS_CONTEXT(num, *size, i, flag);
    }

    (*no_of_vals)++;
    if(*no_of_vals >= *total_no_of_vals / 2){
        expand_in_ClassContext(key_array_ptr, value_array_ptr, total_no_of_vals, className, flag, no_of_vals);
        index = insert_into_ClassContext(key, flag, key_array_ptr, no_of_vals, total_no_of_vals, value_array_ptr, className);
    }

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
    char* key, int size, int flag, char** key_array, void** value_array, Context* context,
    int no_of_superclasses, int* superclass_indices, char** superclass_names, char** prefix_ptr
){
    int index, i, temp_index;
    void** result = NULL;
    ClassContext* temp_class;
    int length = 0;
    char** word_array = split(key, '.', &length);
    char **new_key_array = NULL, **new_superclasses = NULL;
    int* new_superclass_indices = NULL;
    int new_no_of_superclasses = -1;
    void** new_value_array = NULL;
    StringStack* top = NULL;
    char *new_key = NULL, *temp_prefix = NULL;
    int new_size = -1;
    if(length > 1){
        char* temp_key = word_array[length - 2];
        new_key = word_array[length - 1];
        result = search_from_context(context, temp_key);
        if(*((int*)result[1]) != TT_CLASS){
            printf("ClassNotFoundError: superclass '%s' does not exist\n", temp_key);
            exit(0);
        }
        temp_class = (ClassContext*)result[0];
        switch(flag){
            case 0:{
                new_key_array = temp_class->static_var_keys;
                new_value_array = (void**)(temp_class->static_var);
                new_size = temp_class->total_no_of_static_vars;
                break;
            }
            case 1:{
                new_key_array = temp_class->static_method_keys;
                new_value_array = (void**)temp_class->static_method;
                new_size = temp_class->total_no_of_static_methods;
                break;
            }
            case 2:{
                new_key_array = temp_class->static_var_keys;
                new_value_array = (void**)temp_class->static_var;
                new_size = temp_class->total_no_of_static_vars;
                break;
            }
            default:
                return NULL;
        }

        char* new_prefix = NULL;
        void* answer = find_in_ClassContext(
                new_key, new_size, flag, new_key_array, new_value_array, context,
                temp_class->no_of_superclasses, temp_class->superclass_sequence_index_array,
                temp_class->superclasses, &new_prefix
            );
        if(prefix_ptr != NULL && new_prefix != NULL){
            for(i = 0; i < length - 1; i++){
                *prefix_ptr = merge_strings(*prefix_ptr == NULL ? "" : *prefix_ptr, word_array[i]);
                strcat(*prefix_ptr, ".");
            }
            *prefix_ptr = merge_strings(*prefix_ptr, new_prefix);
        }
        return answer;
    }

    index = find_index_in_ClassContext(key, size, flag, key_array);
    if(index == -1){
        if(prefix_ptr != NULL && *prefix_ptr != NULL){
            length = 0;
            word_array = split(*prefix_ptr, '.', &length);
            if(length <= 1){
                printf("UnknownError: Fault in compilation\n");
                exit(0);
            }

            for(i = 0; i < length - 1; i++){
                result = search_from_context(context, word_array[i]);
                if(*((int*)result[1]) == TT_CLASS){
                    printf("ClassNotFoundError: superclass '%s' does not exist\n", word_array[i]);
                    exit(0);
                }
                temp_class = (ClassContext*)result[0];

                switch(flag){
                    case 0:{
                        new_key_array = temp_class->static_var_keys;
                        new_value_array = (void**)temp_class->static_var;
                        new_size = temp_class->total_no_of_static_vars;
                        break;
                    }
                    case 1:{
                        new_key_array = temp_class->static_method_keys;
                        new_value_array = (void**)temp_class->static_method;
                        new_size = temp_class->total_no_of_static_methods;
                        break;
                    }
                    case 2:{
                        new_key_array = temp_class->static_var_keys;
                        new_value_array = (void**)temp_class->static_var;
                        new_size = temp_class->total_no_of_static_vars;
                        break;
                    }
                    default:
                        return NULL;
                }

                index = find_index_in_ClassContext(key, new_size, flag, new_key_array);
                if(index != -1)
                    return new_value_array[index];
            }
            new_superclasses = temp_class->superclasses;
            new_superclass_indices = temp_class->superclass_sequence_index_array;
            new_no_of_superclasses = temp_class->no_of_superclasses;
        }
        else{
            new_superclasses = superclass_names;
            new_superclass_indices = superclass_indices;
            new_no_of_superclasses = no_of_superclasses;
        }

        for(i = 0; i < new_no_of_superclasses; i++){
            temp_index = new_superclass_indices[new_no_of_superclasses - i - 1];
            push_StringStack(&top, new_superclasses[temp_index], "");
        }

        while(index == -1 && top != NULL){
            temp_prefix = top->prefix;
            new_key = pop_StringStack(&top);
            // printf("StringStack :: ENTERED %s\n", new_key);
            result = search_from_context(context, new_key);
            if(*((int*)result[1]) != TT_CLASS){
                printf("ClassNotFoundError: superclass '%s' does not exist\n", new_key);
                exit(0);
            }

            temp_class = (ClassContext*)result[0];
            temp_prefix = merge_strings(temp_prefix, temp_class->className);
            strcat(temp_prefix, ".");
            switch(flag){
                case 0:{
                    new_key_array = temp_class->static_var_keys;
                    new_value_array = (void**)temp_class->static_var;
                    new_size = temp_class->total_no_of_static_vars;
                    break;
                }
                case 1:{
                    new_key_array = temp_class->static_method_keys;
                    new_value_array = (void**)temp_class->static_method;
                    new_size = temp_class->total_no_of_static_methods;
                    break;
                }
                case 2:{
                    new_key_array = temp_class->instance_method_keys;
                    new_value_array = (void**)temp_class->instance_method;
                    new_size = temp_class->total_no_of_instance_methods;
                    // printf("Looking for %s\n", key);
                    break;
                }
                default:
                    return NULL;
            }
            index = find_index_in_ClassContext(key, new_size, flag, new_key_array);
            if(index == -1 && temp_class->no_of_superclasses > 0){
                for(i = temp_class->no_of_superclasses - 1; i >= 0; i--){
                    temp_index = temp_class->superclass_sequence_index_array[i];
                    push_StringStack(&top, temp_class->superclasses[temp_index], temp_prefix);
                }
            }
            else if(index != -1 && prefix_ptr != NULL){
                *prefix_ptr = merge_strings(*prefix_ptr == NULL ? "" : *prefix_ptr, temp_prefix);
            }
        }
        return index == -1 ? NULL : new_value_array[index];
    }

    return value_array[index];
}

void modify_in_ClassContext(
    char* className, char* key, void* value, int flag, 
    char*** key_array_ptr, void*** value_array_ptr, int* no_of_vals, int* total_no_of_vals
){
    char** key_array = *key_array_ptr;
    void** value_array = *value_array_ptr;
    int index = find_index_in_ClassContext(key, *total_no_of_vals, flag, key_array);
    if(index != -1){
        value_array[index] = value;
    }
    else{
        index = insert_into_ClassContext(
                    key, flag, key_array_ptr, no_of_vals, total_no_of_vals, value_array_ptr, className
                );
        key_array = *key_array_ptr;
        value_array = *value_array_ptr;
        if(index == -1){
            printf("HashError: Could not store static variable in ClassContext: '%s'", className);
            exit(0);
        }
        key_array[index] = key;
        value_array[index] = value;
        // printf("ADDING KEY :: %s :: IN INDEX :: %d :: TO BE FOUND LATER\n", key, index);
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
            modify_in_ClassContext(className, old_keys[i], old_vals[i], flag, keys, values, no_of_vals, size);
    }
}