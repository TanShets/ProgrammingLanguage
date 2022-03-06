#pragma once
#include "../value.h"

#define DEFAULT_NO_OF_VARS 5
#define DEFAULT_NO_OF_STATIC_METHODS 5
#define DEFAULT_NO_OF_INSTANCE_METHODS 10
#define DEFAULT_NO_OF_CONSTRUCTORS 5
#define DEFAULT_NO_OF_SUPERCLASSES 10

#define null_key_for_class_context "null"

typedef struct CLASS_CONTEXT{
    char* className;
    char **static_var_keys, **static_method_keys, **instance_method_keys, **superclasses;
    Node **constructor, **static_method, **instance_method;
    Value** static_var;
    int total_no_of_constructors, total_no_of_static_vars, total_no_of_static_methods;
    int total_no_of_instance_methods, total_no_of_superclasses;
    int no_of_constructors, no_of_static_vars, no_of_static_methods, no_of_instance_methods;
    int no_of_superclasses;
    int* superclass_sequence_index_array;
} ClassContext;

ClassContext* construct_ClassContext(char* className){
    ClassContext* classContext = (ClassContext*)allocate_ptr_for_size(sizeof(ClassContext));
    int i;
    classContext->className = className;
    classContext->static_var_keys = (char**)allocate_ptr_array(DEFAULT_NO_OF_VARS, sizeof(char*));
    classContext->static_var = (Value**)allocate_ptr_array(DEFAULT_NO_OF_VARS, sizeof(Value*));
    classContext->total_no_of_static_vars = DEFAULT_NO_OF_VARS;
    classContext->constructor = (Node**)allocate_ptr_array(DEFAULT_NO_OF_CONSTRUCTORS, sizeof(Node*));
    classContext->total_no_of_constructors = DEFAULT_NO_OF_CONSTRUCTORS;
    classContext->static_method_keys = (char**)allocate_ptr_array(DEFAULT_NO_OF_STATIC_METHODS, sizeof(char*));
    classContext->static_method = (Node**)allocate_ptr_array(DEFAULT_NO_OF_STATIC_METHODS, sizeof(Node*));
    classContext->total_no_of_static_methods = DEFAULT_NO_OF_STATIC_METHODS;
    classContext->instance_method_keys = (char**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_METHODS, sizeof(char*));
    classContext->instance_method = (Node**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_METHODS, sizeof(Node*));
    classContext->total_no_of_instance_methods = DEFAULT_NO_OF_INSTANCE_METHODS;
    classContext->superclasses = (char**)allocate_ptr_array(DEFAULT_NO_OF_SUPERCLASSES, sizeof(char*));
    classContext->total_no_of_superclasses = DEFAULT_NO_OF_SUPERCLASSES;
    classContext->superclass_sequence_index_array = (int*)allocate_ptr_array(DEFAULT_NO_OF_SUPERCLASSES, sizeof(int));
    set_heap_alloced_memory(classContext->superclass_sequence_index_array, TT_EOF, DEFAULT_NO_OF_SUPERCLASSES * sizeof(int));

    classContext->no_of_constructors = 0;
    classContext->no_of_static_vars = 0;
    classContext->no_of_static_methods = 0;
    classContext->no_of_instance_methods = 0;
    classContext->no_of_superclasses = 0;
    return classContext;
}