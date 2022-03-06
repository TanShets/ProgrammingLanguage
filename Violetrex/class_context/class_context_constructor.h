#pragma once
#include "class_context_create.h"

void expand_constructor_of_ClassContext(ClassContext* classContext, int new_key){
    int old_size = classContext->total_no_of_constructors;
    int new_size = old_size * 2;
    while(new_size <= new_key) new_size *= 2;
    classContext->constructor = reallocate_heap_alloced_ptr(classContext->constructor, new_size * sizeof(Node*));
}

int insert_constructor_into_ClassContext(ClassContext* classContext, int key, Node* value){
    if(key < 0){
        printf("HashError: Expected Positive no of constructors\n");
        return 0;
    }

    if(key >= classContext->total_no_of_constructors) expand_constructor_of_ClassContext(classContext, key);

    if(classContext->constructor[key] != NULL){
        printf("HashError: Cannot override constructor in the same class definition\n");
        return 0;
    }
    
    classContext->constructor[key] = value;
    return 1;
}

Node* get_constructor_from_ClassContext(ClassContext* classContext, int key){
    return key >= classContext->total_no_of_constructors ? NULL : classContext->constructor[key];
}