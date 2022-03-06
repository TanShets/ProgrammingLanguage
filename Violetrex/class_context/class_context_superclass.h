#pragma once
#include "class_context_hashing_template.h"

void insert_superclass_into_ClassContext(ClassContext* classContext, char* key);

void expand_superclass_in_ClassContext(ClassContext* classContext){
    int old_size = classContext->total_no_of_superclasses;
    char** old_superclasses = classContext->superclasses;
    int* old_super_array = classContext->superclass_sequence_index_array;
    int old_no_of_superclasses = classContext->no_of_superclasses;

    classContext->total_no_of_superclasses *= 2;
    classContext->superclasses = (char**)allocate_ptr_array(classContext->total_no_of_superclasses, sizeof(char*));
    classContext->superclass_sequence_index_array = (int*)allocate_ptr_array(classContext->total_no_of_superclasses, sizeof(int));
    classContext->no_of_superclasses = 0;

    for(int i = 0; i < old_no_of_superclasses; i++)
        insert_superclass_into_ClassContext(classContext, old_superclasses[old_super_array[i]]);
    
    free_pointer(old_superclasses);
    free_pointer(old_super_array);
}

void insert_superclass_into_ClassContext(ClassContext* classContext, char* key){
    char** superclasses = classContext->superclasses;
    if(key == NULL || superclasses == NULL) return;
    int size = classContext->total_no_of_superclasses;
    int i = 0;
    int num = strlen(key);
    int index = HASH_CLASS_SUPERCLASS(num, size, i);
    while(superclasses[index] != NULL){
        i++;
        index = HASH_CLASS_SUPERCLASS(num, size, i);
    }

    if(classContext->no_of_superclasses >= classContext->total_no_of_superclasses / 2){
        expand_superclass_in_ClassContext(classContext);
        insert_superclass_into_ClassContext(classContext, key);
        return;
    }
    classContext->superclass_sequence_index_array[classContext->no_of_superclasses] = index;
    classContext->no_of_superclasses++;
    classContext->superclasses[index] = key;
}

int find_index_in_superclass_from_ClassContext(ClassContext* classContext, char* key){
    char** superclasses = classContext->superclasses;
    if(key == NULL || superclasses == NULL) return -1;
    int num = strlen(key), i = 0, size = classContext->total_no_of_superclasses;
    int index = HASH_CLASS_SUPERCLASS(num, size, i);
    while(superclasses[index] != NULL && strcmp(superclasses[index], key) != 0){
        i++;
        index = HASH_CLASS_SUPERCLASS(num, size, i);
    }
    return superclasses[index] == NULL ? -1 : index;
}

char* find_superclass_in_ClassContext(ClassContext* classContext, char* key){
    int index = find_index_in_superclass_from_ClassContext(classContext, key);
    return index == -1 ? NULL : classContext->superclasses[index];
}