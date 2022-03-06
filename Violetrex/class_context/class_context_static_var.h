#pragma once
#include "class_context_hashing_template.h"

void expand_static_var_in_ClassContext(ClassContext* classContext){
    expand_in_ClassContext(
        &classContext->static_var_keys, (void***)(&classContext->static_var), 
        &classContext->total_no_of_static_vars, classContext->className,
        0, &classContext->no_of_static_vars
    );
}

int insert_static_var_into_ClassContext(ClassContext* classContext, char* key){
    return insert_into_ClassContext(
        key, 0, &classContext->static_var_keys, 
        &classContext->no_of_static_vars, &classContext->total_no_of_static_vars,
        (void***)(&classContext->static_var), classContext->className
    );
}

int find_index_in_static_var_from_ClassContext(ClassContext* classContext, char* key){
    return find_index_in_ClassContext(key, classContext->total_no_of_static_vars, 0, classContext->static_var_keys);
}

Value* find_static_var_in_ClassContext(ClassContext* classContext, char* key, Context* context, char** prefix_ptr){
    return (Value*)find_in_ClassContext(
        key, classContext->total_no_of_static_vars, 0, 
        classContext->static_var_keys, (void**)(classContext->static_var),
        context, classContext->no_of_superclasses, classContext->superclass_sequence_index_array,
        classContext->superclasses, prefix_ptr
    );
    // int index = find_index_in_static_var_from_ClassContext(classContext, key);

    // return index == -1 ? NULL : classContext->static_var[index];
}

void modify_static_var_in_ClassContext(ClassContext* classContext, char* key, Value* value){
    modify_in_ClassContext(
        classContext->className, key, value, 0, &classContext->static_var_keys,
        (void***)(&classContext->static_var), &classContext->no_of_static_vars, &classContext->total_no_of_static_vars
    );
}