#pragma once
#include "class_context_hashing_template.h"

void expand_instance_method_in_ClassContext(ClassContext* classContext){
    expand_in_ClassContext(
        &classContext->instance_method_keys, (void***)(&classContext->instance_method), 
        &classContext->total_no_of_instance_methods, classContext->className,
        2, &classContext->no_of_instance_methods
    );
}

int insert_instance_method_into_ClassContext(ClassContext* classContext, char* key){
    return insert_into_ClassContext(
        key, 2, &classContext->instance_method_keys, 
        &classContext->no_of_instance_methods, &classContext->total_no_of_instance_methods,
        (void***)(&classContext->instance_method), classContext->className
    );
}

int find_index_in_instance_method_from_ClassContext(ClassContext* classContext, char* key){
    return find_index_in_ClassContext(key, classContext->total_no_of_instance_methods, 2, classContext->instance_method_keys);
}

Node* find_instance_method_in_ClassContext(ClassContext* classContext, char* key, Context* context, char** prefix_ptr){
    Node* answer = (Node*)find_in_ClassContext(
                    key, classContext->total_no_of_instance_methods, 2, 
                    classContext->instance_method_keys, (void**)(classContext->instance_method),
                    context, classContext->no_of_superclasses, classContext->superclass_sequence_index_array,
                    classContext->superclasses, prefix_ptr
                );
    
    if(answer != NULL && prefix_ptr != NULL){
        *prefix_ptr = merge_strings(*prefix_ptr == NULL ? "" : *prefix_ptr, classContext->className);
        *prefix_ptr = merge_strings(*prefix_ptr, ".");
    }
    return answer;
}

void modify_instance_method_in_ClassContext(ClassContext* classContext, char* key, Node* value){
    // printf("Adding method %s to be found later in class %s\n", key, classContext->className);
    modify_in_ClassContext(
        classContext->className, key, value, 2, &(classContext->instance_method_keys),
        (void***)(&(classContext->instance_method)), &classContext->no_of_instance_methods, &classContext->total_no_of_instance_methods
    );
}