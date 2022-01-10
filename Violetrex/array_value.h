#include "value.h"
#pragma once
#define HASH_ARRAY(num, i, size) ((2*i*i + 7*i + num) % size)

typedef struct ARRAYVALUE{
    Value **keys, **values;
    int arr_size, default_index, curr_size;
    int* key_type;
} ArrayValue;

int insert_into_ArrayValue(ArrayValue* arrayValue, Value* key);

void modify_ArrayValue(ArrayValue* arrayValue, Value* key, Value* value);

void expand_ArrayValue(ArrayValue* arrayValue){
    int old_size = arrayValue->arr_size;
    arrayValue->arr_size *= 2;
    Value** old_keys = arrayValue->keys;
    // arrayValue->keys = (Value**)calloc(arrayValue->arr_size, sizeof(Value*));
    arrayValue->keys = (Value**)allocate_ptr_array(arrayValue->arr_size, sizeof(Value*));
    Value** old_values = arrayValue->values;
    // arrayValue->values = (Value**)calloc(arrayValue->arr_size, sizeof(Value*));
    arrayValue->values = (Value**)allocate_ptr_array(arrayValue->arr_size, sizeof(Value*));
    int* old_key_type = arrayValue->key_type;
    // arrayValue->key_type = (int*)calloc(arrayValue->arr_size, sizeof(int));
    arrayValue->key_type = (int*)allocate_ptr_array(arrayValue->arr_size, sizeof(int));
    // memset(arrayValue->key_type, TT_EOF, arrayValue->arr_size * sizeof(int));
    set_heap_alloced_memory(arrayValue->key_type, TT_EOF, arrayValue->arr_size * sizeof(int));
    Value *temp_key, *temp_val;
    arrayValue->curr_size = 0;
    for(int i = 0; i < old_size; i++)
        if(old_key_type[i] != TT_EOF)
            modify_ArrayValue(arrayValue, old_keys[i], old_values[i]);
}

ArrayValue* construct_ArrayValue(int size){
    // ArrayValue* arrayValue = (ArrayValue*)malloc(sizeof(ArrayValue));
    ArrayValue* arrayValue = (ArrayValue*)allocate_ptr_for_size(sizeof(ArrayValue));
    arrayValue->arr_size = size * 2;
    // arrayValue->keys = (Value**)calloc(arrayValue->arr_size, sizeof(Value*));
    // arrayValue->values = (Value**)calloc(arrayValue->arr_size, sizeof(Value*));
    // arrayValue->key_type = (int*)calloc(arrayValue->arr_size, sizeof(int));
    arrayValue->keys = (Value**)allocate_ptr_array(arrayValue->arr_size, sizeof(Value*));
    arrayValue->values = (Value**)allocate_ptr_array(arrayValue->arr_size, sizeof(Value*));
    arrayValue->key_type = (int*)allocate_ptr_array(arrayValue->arr_size, sizeof(int));
    // memset(arrayValue->key_type, TT_EOF, arrayValue->arr_size * sizeof(int));
    set_heap_alloced_memory(arrayValue->key_type, TT_EOF, arrayValue->arr_size * sizeof(int));
    arrayValue->default_index = 0;
    arrayValue->curr_size = 0;
}

ArrayValue* get_ArrayValue_from_values(Value** values, int size){
    ArrayValue* arrayValue = construct_ArrayValue(size);
    for(int i = 0; i < size; i++){
        modify_ArrayValue(arrayValue, NULL, values[i]);
    }
    return arrayValue;
}

int find_index_in_ArrayValue(ArrayValue* arrayValue, Value* key){
    int num;
    char* word;
    switch(key->valType){
        case TT_INT:
            num = *((int*)key->num);
            break;
        case TT_FLOAT:
            num = (int)(*((double*)key->num) + 1e-9);
            break;
        case TT_NULL:
            num = 0;
            break;
        case TT_STRING:
            word = (char*)key->num;
            num = strlen(word);
            break;
        case TT_TRUE:
            num = 1;
            break;
        case TT_FALSE:
            num = 0;
            break;
        default:{
            return -1;
        }
    }

    int i = 0;
    int index = HASH_ARRAY(num, i, arrayValue->arr_size);
    i++;
    int is_eq = 0;
    while(
        arrayValue->key_type[index] != TT_EOF && !is_eq
    ){
        if(arrayValue->key_type[index] == key->valType){
            switch(key->valType){
                case TT_INT:{
                    int num1 = *((int*)key->num);
                    int num2 = *((int*)(arrayValue->keys[index]->num));
                    is_eq = num1 == num2;
                    break;
                }
                case TT_FLOAT:{
                    double num1 = *((double*)key->num);
                    double num2 = *((double*)(arrayValue->keys[index]->num));
                    is_eq = (num1 - num2) > 1e-8;
                    break;
                }
                case TT_NULL:
                    is_eq = 1;
                    break;
                case TT_STRING:{
                    char* word1 = (char*)key->num;
                    char* word2 = (char*)(arrayValue->keys[index]->num);
                    is_eq = strcmp(word1, word2) == 0;
                    break;
                }
                case TT_TRUE:
                    is_eq = 1;
                    break;
                case TT_FALSE:
                    is_eq = 1;
                    break;
            }

            if(is_eq == 1)
                break;
        }

        index = HASH_ARRAY(num, i, arrayValue->arr_size);
        i++;
    }

    if(arrayValue->key_type[index] == TT_EOF){
        return -1;
    }

    return index;
}

Value* find_in_ArrayValue(ArrayValue* arrayValue, Value* key){
    int index = find_index_in_ArrayValue(arrayValue, key);
    if(index == -1){
        return construct_Value(
            make_error(
                KeyNotFoundError(
                    key->num, key->valType, key->line_no, 
                    key->col_no
                ),
                key->line_no, key->col_no
            )
        );
    }
    return arrayValue->values[index];
}

int insert_into_ArrayValue(ArrayValue* arrayValue, Value* key){
    int num;
    char* word;
    if(key == NULL)
        return -1;
    switch(key->valType){
        case TT_INT:
            num = *((int*)key->num);
            break;
        case TT_FLOAT:
            num = (int)(*((double*)key->num) + 1e-9);
            break;
        case TT_NULL:
            num = 0;
            break;
        case TT_STRING:
            word = (char*)key->num;
            num = strlen(word);
            break;
        case TT_TRUE:
            num = 1;
            break;
        case TT_FALSE:
            num = 0;
            break;
        default:
            printf("Undetected %d\n", key->valType);
            return -1;
    }
    int i = 0;
    int index = HASH_ARRAY(num, i, arrayValue->arr_size);
    i++;
    while(
        arrayValue->key_type[index] != TT_EOF && 
        arrayValue->key_type[index] != TT_ERROR
    ){
        index = HASH_ARRAY(num, i, arrayValue->arr_size);
        i++;
    }
    
    return index;
}

void modify_ArrayValue(ArrayValue* arrayValue, Value* key, Value* value){
    int num, index, old_index = -2;
    Value* temp_key;
    if(key == NULL){
        num = arrayValue->default_index;
        // Token* token = (Token*)malloc(sizeof(Token));
        Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
        // token->val = malloc(sizeof(int));
        token->val = allocate_ptr_for_size(sizeof(int));
        *((int*)token->val) = num;
        token->type = TT_INT;
        token->line_no = value->line_no, token->col_no = value->col_no;
        temp_key = construct_Value(token);
        while(find_index_in_ArrayValue(arrayValue, temp_key) != -1){
            num++;
            *((int*)(temp_key->num)) = num;
            if(num >= arrayValue->arr_size / 2)
                expand_ArrayValue(arrayValue);
        }
        arrayValue->default_index = num + 1;
        index = insert_into_ArrayValue(arrayValue, temp_key);
    }
    else{
        index = find_index_in_ArrayValue(arrayValue, key);
        old_index = index;
        if(index == -1)
            index = insert_into_ArrayValue(arrayValue, key);
        
        if(index == -1){
            return;
        }
        temp_key = copy_Value(key);
        if(temp_key == NULL)
            return;
    }

    arrayValue->key_type[index] = temp_key->valType;
    arrayValue->keys[index] = temp_key;
    arrayValue->values[index] = copy_Value(value);

    if(key == NULL || old_index == -1)
        arrayValue->curr_size++;
    
    if(arrayValue->curr_size >= arrayValue->arr_size / 2){
        expand_ArrayValue(arrayValue);
    }
}

void delete_from_ArrayValue(ArrayValue* arrayValue, Value* key){
    int index = find_index_in_ArrayValue(arrayValue, key);
    if(index == -1)
        return;
    
    arrayValue->key_type[index] = TT_ERROR;
    free(arrayValue->keys[index]);
    free(arrayValue->values[index]);
    arrayValue->curr_size--;
}

void print_ArrayValue(ArrayValue* arrayValue){
    printf("[");
    int size = arrayValue->curr_size;
    for(int i = 0; i < arrayValue->arr_size; i++){
        if(
            arrayValue->key_type[i] != TT_EOF && 
            arrayValue->key_type[i] != TT_ERROR
        ){
            if(arrayValue->key_type[i] == TT_STRING)
                printf("'");
            printValue(arrayValue->keys[i]);
            if(arrayValue->key_type[i] == TT_STRING)
                printf("'");
            printf(": ");
            printValue(arrayValue->values[i]);
            size--;
            if(size > 0)
                printf(", ");
            else
                break;
        }
    }
    // for(int i = 0; i < arrayValue->arr_size; i++)
    //     printf("%d ", arrayValue->key_type[i]);
    printf("]");
}