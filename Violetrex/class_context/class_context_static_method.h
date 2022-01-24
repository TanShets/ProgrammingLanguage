#pragma once
#include "class_context_hashing_template.h"

void expand_static_method_in_ClassContext(ClassContext* classContext){
    expand_in_ClassContext(
        &classContext->static_method_keys, (void***)(&classContext->static_method), 
        &classContext->total_no_of_static_methods, classContext->className,
        1, &classContext->no_of_static_methods
    );
}

int insert_static_method_into_ClassContext(ClassContext* classContext, char* key){
    return insert_into_ClassContext(
        key, 1, classContext->static_method_keys, 
        &classContext->no_of_static_methods, &classContext->total_no_of_static_methods
    );
    // int i = 0;
    // int num = strlen(key), size = classContext->total_no_of_static_vars;
    // index = HASH_CLASS_STATIC_VARS(num, size, i);
    // while(
    //     classContext->static_var_keys[index] != NULL && 
    //     strcmp(classContext->static_var_keys[index], null_key_for_class_context) != 0
    // ){
    //     i++;
    //     index = HASH_CLASS_STATIC_VARS(num, size, i);
    // }

    // classContext->no_of_static_vars++;
    // if(classContext->no_of_static_vars >= classContext->total_no_of_static_vars / 2){}

    // return index;
}

int find_index_in_static_method_from_ClassContext(ClassContext* classContext, char* key){
    return find_index_in_ClassContext(key, classContext->total_no_of_static_methods, 1, classContext->static_method_keys);
    // int i = 0;
    // int index = -1, temp_index = -1;
    // if(key == NULL) return index;
    // int num = strlen(key), size = classContext->total_no_of_static_vars;
    // index = HASH_CLASS_STATIC_VARS(num, size, i);
    // while(
    //     classContext->static_var_keys[index] != NULL && 
    //     strcmp(classContext->static_var_keys[index], key) != 0
    // ){
    //     i++;
    //     index = HASH_CLASS_STATIC_VARS(num, size, i);
    // }

    // return classContext->static_var_keys[index] == NULL ? -1 : index;
}

Node* find_static_method_in_ClassContext(ClassContext* classContext, char* key){
    return (Node*)find_in_ClassContext(
        key, classContext->total_no_of_static_methods, 1, 
        classContext->static_method_keys, (void**)(classContext->static_method)
    );
    // int index = find_index_in_static_var_from_ClassContext(classContext, key);

    // return index == -1 ? NULL : classContext->static_var[index];
}

void modify_static_method_in_ClassContext(ClassContext* classContext, char* key, Node* value){
    modify_in_ClassContext(
        classContext->className, key, value, 1, classContext->static_method_keys,
        (void**)(classContext->static_method), &classContext->no_of_static_methods, &classContext->total_no_of_static_methods
    );
    // int index = find_index_in_static_var_from_ClassContext(classContext, key);

    // if(index != -1){
    //     classContext->static_var[index] = value;
    // }
    // else{
    //     index = insert_static_var_into_ClassContext(classContext, key);
    //     if(index == -1){
    //         printf("HashError: Could not store static variable in ClassContext: '%s'", classContext->className);
    //         exit(0);
    //     }
    //     classContext->static_var_keys[index] = key;
    //     classContext->static_var[index] = value;
    // }
}