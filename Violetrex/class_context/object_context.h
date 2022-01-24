#pragma once
#include "class_context_create.h"
#include "class_context_static_var.h"
#include "class_context_static_method.h"
#include "class_context_instance_method.h"

#define DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT 10

#define HASH_OBJECT_INSTANCE_VARS(num, size, i) ((num + 7 * i + i * i) % size)

typedef struct OBJECT_CONTEXT{
    ClassContext* static_context;
    char** instance_var_keys;
    Value** instance_var;
    int no_of_instance_vars, total_no_of_instance_vars;
    char* objectName;
} ObjectContext;

ObjectContext* construct_ObjectContext(ClassContext* static_context, char* objectName){
    ObjectContext* objectContext = (ObjectContext*)allocate_ptr_for_size(sizeof(ObjectContext));
    objectContext->objectName = objectName;
    objectContext->static_context = static_context;
    objectContext->instance_var_keys = (char**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT, sizeof(char*));
    objectContext->instance_var = (Value**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT, sizeof(Value*));
    objectContext->no_of_instance_vars = 0;
    objectContext->total_no_of_instance_vars = DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT;
    return objectContext;
}

void modify_instance_var_from_ObjectContext(ObjectContext* objectContext, char* key, Value* value);

void expand_instance_var_in_ObjectContext(ObjectContext* objectContext){
    char** old_keys = objectContext->instance_var_keys;
    Value** old_vals = objectContext->instance_var;
    int old_size = objectContext->total_no_of_instance_vars;
    objectContext->total_no_of_instance_vars *= 2;
    objectContext->instance_var_keys = (char**)allocate_ptr_array(objectContext->total_no_of_instance_vars, sizeof(char*));
    objectContext->instance_var = (Value**)allocate_ptr_array(objectContext->total_no_of_instance_vars, sizeof(Value*));
    objectContext->no_of_instance_vars = 0;

    for(int i = 0; i < old_size; i++){
        if(old_keys[i] != NULL && strcmp(old_keys[i], null_key_for_class_context) != 0)
            modify_instance_var_from_ObjectContext(objectContext, old_keys[i], old_vals[i]);
    }
}

int insert_instance_var_into_ObjectContext(ObjectContext* objectContext, char* key){
    if(key == NULL) return -1;
    int num = strlen(key), size = objectContext->total_no_of_instance_vars;
    int i = 0;
    int index = HASH_OBJECT_INSTANCE_VARS(num, size, i);
    while(
        objectContext->instance_var_keys[index] != NULL && 
        strcmp(objectContext->instance_var_keys[index], null_key_for_class_context) != 0
    ){
        i++;
        index = HASH_OBJECT_INSTANCE_VARS(num, size, i);
    }

    objectContext->no_of_instance_vars++;
    if(objectContext->no_of_instance_vars >= objectContext->total_no_of_instance_vars / 2)
        expand_instance_var_in_ObjectContext(objectContext);
    return index;
}

int find_index_from_instance_var_in_ObjectContext(ObjectContext* objectContext, char* key){
    int index = -1;
    if(key == NULL) return index;
    int num = strlen(key), size = objectContext->total_no_of_instance_vars;
    int i = 0;
    index = HASH_OBJECT_INSTANCE_VARS(num, size, i);
    while(
        objectContext->instance_var_keys[index] != NULL && 
        strcmp(objectContext->instance_var_keys[index], key) != 0
    ){
        i++;
        index = HASH_OBJECT_INSTANCE_VARS(num, size, i);
    }

    return objectContext->instance_var_keys[index] == NULL ? -1 : index;
}

Value* find_instance_var_from_ObjectContext(ObjectContext* objectContext, char* key){
    int index = find_index_from_instance_var_in_ObjectContext(objectContext, key);
    return index == -1 ? NULL : objectContext->instance_var[index];
}

void modify_instance_var_from_ObjectContext(ObjectContext* objectContext, char* key, Value* value){
    int index = find_index_from_instance_var_in_ObjectContext(objectContext, key);
    if(index != -1){
        objectContext->instance_var[index] = value;
    }
    else{
        index = insert_instance_var_into_ObjectContext(objectContext, key);
        if(index == -1){
            printf(
                "HashError: Failed to hash instance variable in object '%s' of class %s\n",
                objectContext->objectName, objectContext->static_context->className
            );
            exit(0);
        }
        objectContext->instance_var_keys[index] = key;
        objectContext->instance_var[index] = value;
    }
}

void* find_from_ObjectContext(ObjectContext* objectContext, char* key, int type){
    void* answer = NULL;
    switch(type){
        case VAR_NODE:{
            answer = find_instance_var_from_ObjectContext(objectContext, key);
            if(answer == NULL){
                answer = find_static_var_in_ClassContext(objectContext->static_context, key);
            }
            break;
        }
        case FUNCTION_CALL_NODE:{
            answer = find_instance_method_in_ClassContext(objectContext->static_context, key);
            if(answer == NULL)
                answer = find_static_method_in_ClassContext(objectContext->static_context, key);
            break;
        }
        default:{
            return NULL;
        }
    }
    return answer;
}