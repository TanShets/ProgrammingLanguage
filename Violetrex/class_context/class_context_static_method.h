#pragma once
#include "class_context_hashing_template.h"
#include "class_context_linked_list.h"

void expand_static_method_in_ClassContext(ClassContext* classContext){
    expand_in_ClassContext(
        &classContext->static_method_keys, (void***)(&classContext->static_method), 
        &classContext->total_no_of_static_methods, classContext->className,
        1, &classContext->no_of_static_methods
    );
}

int insert_static_method_into_ClassContext(ClassContext* classContext, char* key){
    return insert_into_ClassContext(
        key, 1, &classContext->static_method_keys,
        &classContext->no_of_static_methods, &classContext->total_no_of_static_methods,
        (void***)(&classContext->static_method), classContext->className
    );
}

int find_index_in_static_method_from_ClassContext(ClassContext* classContext, char* key){
    return find_index_in_ClassContext(key, classContext->total_no_of_static_methods, 1, classContext->static_method_keys);
}

Node* find_static_method_in_ClassContext(ClassContext* classContext, char* key, Context* context, char** prefix_ptr){
    return (Node*)find_in_ClassContext(
        key, classContext->total_no_of_static_methods, 1, 
        classContext->static_method_keys, (void**)(classContext->static_method),
        context, classContext->no_of_superclasses, classContext->superclass_sequence_index_array,
        classContext->superclasses, prefix_ptr
    );
    // int index = find_index_in_static_var_from_ClassContext(classContext, key);

    // return index == -1 ? NULL : classContext->static_var[index];
}

void modify_static_method_in_ClassContext(ClassContext* classContext, char* key, Node* value){
    modify_in_ClassContext(
        classContext->className, key, value, 1, &classContext->static_method_keys,
        (void***)(&classContext->static_method), &classContext->no_of_static_methods, &classContext->total_no_of_static_methods
    );
}