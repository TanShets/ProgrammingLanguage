#pragma once
#include "mem_alloc.h"
#define HEAP_ALLOC_HASHING_FN(num, size, i) ((num + 5 * i + 3 * i * i) % size)
#define HEAP_ALLOCED_DEFAULT_SIZE 32

void** HEAP_ALLOCED_HASHMAP_KEY_ARRAY = NULL;
void** HEAP_ALLOCED_HASHMAP_VALUE_ARRAY = NULL;
char* HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY = NULL;
//Basically we're allowing 32 pointers to be stored in the Hashmap by default which is open to expansion
HEAP_ALLOCED_HASHMAP_INDEX_TYPE HEAP_ALLOCED_HASHMAP_CAPACITY = HEAP_ALLOCED_DEFAULT_SIZE;
HEAP_ALLOCED_HASHMAP_INDEX_TYPE HEAP_ALLOCED_HASHMAP_FILLED_COUNT = 0;

void initialize_heap_alloc_hashmap(){
    HEAP_ALLOCED_HASHMAP_KEY_ARRAY = (void**)mem_alloc(HEAP_ALLOCED_HASHMAP_CAPACITY * sizeof(void*));
    HEAP_ALLOCED_HASHMAP_VALUE_ARRAY = (void**)mem_alloc(HEAP_ALLOCED_HASHMAP_CAPACITY * sizeof(void*));
    HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY = (char*)mem_alloc(HEAP_ALLOCED_HASHMAP_CAPACITY * sizeof(char));
    for(int i = 0; i < HEAP_ALLOCED_HASHMAP_CAPACITY; i++){
        HEAP_ALLOCED_HASHMAP_KEY_ARRAY[i] = NULL;
        HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[i] = NULL;
        HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[i] = 'm';
    }
}

void expand_heap_alloced_hashmap(){
    void** old_key_arr = HEAP_ALLOCED_HASHMAP_KEY_ARRAY;
    void** old_val_arr = HEAP_ALLOCED_HASHMAP_VALUE_ARRAY;
    char* old_val_type_arr = HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY;
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE old_size = HEAP_ALLOCED_HASHMAP_CAPACITY, new_size = 2 * HEAP_ALLOCED_HASHMAP_CAPACITY;

    void** new_key_arr = mem_alloc(new_size * sizeof(void*));
    void** new_val_arr = mem_alloc(new_size * sizeof(void*));
    char* new_val_type_arr = mem_alloc(new_size * sizeof(char));
    if(new_key_arr == NULL || new_val_arr == NULL){
        printf("Heap Overflow: Could not find additional space\n");
        exit(0);
    }
    HEAP_ALLOCED_HASHMAP_KEY_ARRAY = new_key_arr;
    HEAP_ALLOCED_HASHMAP_VALUE_ARRAY = new_val_arr;
    HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY = new_val_type_arr;
    HEAP_ALLOCED_HASHMAP_CAPACITY = new_size;
    int i;
    for(i = 0; i < HEAP_ALLOCED_HASHMAP_CAPACITY; i++){
        HEAP_ALLOCED_HASHMAP_KEY_ARRAY[i] = NULL;
        HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[i] = NULL;
        HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[i] = 'm';
    }
    HEAP_ALLOCED_HASHMAP_FILLED_COUNT = 0;
    for(int i = 0; i < old_size; i++)
        if(old_key_arr[i] != NULL && old_key_arr[i] != (void*)old_key_arr)
            insert_into_heap_alloced_hashmap(old_key_arr[i], old_val_arr[i], old_val_type_arr[i] == 'h');
    
    mem_free_heap_allocated_pointer(old_key_arr, old_size * sizeof(void*));
    mem_free_heap_allocated_pointer(old_val_arr, old_size * sizeof(void*));
    mem_free_heap_allocated_pointer(old_val_type_arr, old_size * sizeof(char));
}

void insert_into_heap_alloced_hashmap(void* key, void* value, int isHead){
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE* index_ptr = (HEAP_ALLOCED_HASHMAP_INDEX_TYPE*)key;
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE num = (HEAP_ALLOCED_HASHMAP_INDEX_TYPE)index_ptr;
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE index, i = -1;
    int isLooking = 1;
    while(isLooking){
        i++;
        index = HEAP_ALLOC_HASHING_FN(num, HEAP_ALLOCED_HASHMAP_CAPACITY, i);
        isLooking = HEAP_ALLOCED_HASHMAP_KEY_ARRAY[index] != NULL && 
                    HEAP_ALLOCED_HASHMAP_KEY_ARRAY[index] != (void*)HEAP_ALLOCED_HASHMAP_KEY_ARRAY;
    }

    HEAP_ALLOCED_HASHMAP_KEY_ARRAY[index] = key;
    HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[index] = value;
    HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[index] = isHead ? 'h' : 'm';
    HEAP_ALLOCED_HASHMAP_FILLED_COUNT++;
    if(HEAP_ALLOCED_HASHMAP_FILLED_COUNT >= HEAP_ALLOCED_HASHMAP_CAPACITY / 2)
        expand_heap_alloced_hashmap();
}

HEAP_ALLOCED_HASHMAP_INDEX_TYPE find_index_from_key_from_heap_alloced_hashmap(void* key){
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE* index_ptr = (HEAP_ALLOCED_HASHMAP_INDEX_TYPE*)key;
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE num = (HEAP_ALLOCED_HASHMAP_INDEX_TYPE)index_ptr;
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE index, temp_index, i = -1;
    int isLooking = 1;

    while(isLooking){
        i++;
        temp_index = HEAP_ALLOC_HASHING_FN(num, HEAP_ALLOCED_HASHMAP_CAPACITY, i);
        isLooking = HEAP_ALLOCED_HASHMAP_KEY_ARRAY[temp_index] != NULL && 
                    HEAP_ALLOCED_HASHMAP_KEY_ARRAY[temp_index] != key;
    }

    index = HEAP_ALLOCED_HASHMAP_KEY_ARRAY[temp_index] == key ? temp_index : -1;
    return index;
}

HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE find_from_heap_alloced_hashmap(void* key){
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE index = find_index_from_key_from_heap_alloced_hashmap(key);
    HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE answer;
    answer.ptr = index == -1 ? NULL : HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[index];
    answer.type = index == -1 ? 'm' : HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[index];
    return answer;
}

void delete_key_from_heap_alloced_hashmap(void* key){
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE index = find_index_from_key_from_heap_alloced_hashmap(key);
    if(index == -1){
        printf("Failed to delete from hm\n");
        return;
    }
    
    HEAP_ALLOCED_HASHMAP_KEY_ARRAY[index] = (void*)HEAP_ALLOCED_HASHMAP_KEY_ARRAY;
    HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[index] = NULL;
    HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[index] = 'm';
}

void modify_heap_alloced_hashmap_with_key(void* key, void* new_value, int isHead){
    HEAP_ALLOCED_HASHMAP_INDEX_TYPE index = find_index_from_key_from_heap_alloced_hashmap(key);
    if(index == -1)
        insert_into_heap_alloced_hashmap(key, new_value, isHead);
    else{
        HEAP_ALLOCED_HASHMAP_VALUE_ARRAY[index] = new_value;
        HEAP_ALLOCED_HASHMAP_VALUE_TYPE_ARRAY[index] = isHead ? 'h' : 'm';
    }
}