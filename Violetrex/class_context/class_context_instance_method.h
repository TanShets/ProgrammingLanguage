#pragma once
#include "class_context_hashing_template.h"

void expand_instance_method_in_ClassContext(ClassContext* classContext){
    // char** old_keys = classContext->instance_method_keys;
    // Node** old_vals = classContext->instance_method;
    // int old_size = classContext->total_no_of_instance_methods;
    // int new_size = old_size * 2;
    // classContext->instance_method_keys = allocate_ptr_array(new_size, sizeof(char*));
    // classContext->instance_method = allocate_ptr_array(new_size, sizeof(Node*));
    // for(int i = 0; i < old_size; i++){
    //     if(old_keys[i] != NULL && strcmp(old_keys[i], null_key_for_class_context) != 0)
    //         modify_instance_method_in_ClassContext(classContext, old_keys[i], old_vals[i]);
    // }
    expand_in_ClassContext(
        &classContext->instance_method_keys, (void***)(&classContext->instance_method), 
        &classContext->total_no_of_instance_methods, classContext->className,
        2, &classContext->no_of_instance_methods
    );
}

int insert_instance_method_into_ClassContext(ClassContext* classContext, char* key){
    return insert_into_ClassContext(
        key, 2, classContext->instance_method_keys, 
        &classContext->no_of_instance_methods, &classContext->total_no_of_instance_methods
    );
}

int find_index_in_instance_method_from_ClassContext(ClassContext* classContext, char* key){
    return find_index_in_ClassContext(key, classContext->total_no_of_instance_methods, 2, classContext->instance_method_keys);
}

Node* find_instance_method_in_ClassContext(ClassContext* classContext, char* key){
    return (Node*)find_in_ClassContext(
        key, classContext->total_no_of_instance_methods, 2, 
        classContext->instance_method_keys, (void**)(classContext->instance_method)
    );
}

void modify_instance_method_in_ClassContext(ClassContext* classContext, char* key, Node* value){
    modify_in_ClassContext(
        classContext->className, key, value, 2, classContext->instance_method_keys,
        (void**)(classContext->instance_method), &classContext->no_of_instance_methods, &classContext->total_no_of_instance_methods
    );
}