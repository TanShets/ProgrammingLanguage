#pragma once
#include "class_context_create.h"
#include "class_context_static_var.h"
#include "class_context_static_method.h"
#include "class_context_instance_method.h"
#include "class_context_constructor.h"
#include "class_context_linked_list.h"

#define DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT 10

#define HASH_OBJECT_INSTANCE_VARS(num, size, i) ((num + 7 * i + i * i) % size)

typedef struct OBJECT_CONTEXT{
    ClassContext* static_context;
    char **instance_var_keys, **superclass_keys;
    Value** instance_var;
    int no_of_instance_vars, total_no_of_instance_vars;
    int no_of_superclasses, total_no_of_superclasses;
    int* superclass_object_index_sequence;
    char* objectName;
    struct OBJECT_CONTEXT** superclass_objects;
} ObjectContext;

void insert_superclass_objects_into_ObjectContext(ObjectContext* objectContext, Context* context, char* prefix);

ObjectContext* construct_ObjectContext(ClassContext* static_context, Context* context, char* prefix){
    ObjectContext* objectContext = (ObjectContext*)allocate_ptr_for_size(sizeof(ObjectContext));
    objectContext->objectName = null_key_for_class_context;
    objectContext->static_context = static_context;
    objectContext->instance_var_keys = (char**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT, sizeof(char*));
    objectContext->instance_var = (Value**)allocate_ptr_array(DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT, sizeof(Value*));
    objectContext->no_of_instance_vars = 0;
    objectContext->total_no_of_instance_vars = DEFAULT_NO_OF_INSTANCE_VARS_IN_OBJECTCONTEXT;

    objectContext->total_no_of_superclasses = static_context->total_no_of_superclasses;
    objectContext->superclass_keys = static_context->superclasses;
    objectContext->superclass_object_index_sequence = static_context->superclass_sequence_index_array;
    objectContext->superclass_objects = (ObjectContext**)allocate_ptr_array(static_context->total_no_of_superclasses, sizeof(ObjectContext*));
    objectContext->no_of_superclasses = 0;
    insert_superclass_objects_into_ObjectContext(objectContext, context, prefix);
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
    if(objectContext->no_of_instance_vars >= objectContext->total_no_of_instance_vars / 2){
        expand_instance_var_in_ObjectContext(objectContext);
        return insert_instance_var_into_ObjectContext(objectContext, key);
    }
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

int find_index_instance_var_of_superclass_from_ObjectContext(
    ObjectContext* objectContext, char* key, char* prefix, Context* context
){
    char* temp_key = merge_strings(prefix, key);
    int index = find_index_from_instance_var_in_ObjectContext(objectContext, temp_key);
    int length;
    void** result = NULL;
    if(index == -1){
        ClassContext* classContext = NULL;
        if(prefix == NULL){
            classContext = objectContext->static_context;
        }
        else{
            length = 0;
            char** words = split(prefix, '.', &length);
            if(length == 1)
                return -1;
            char* className = words[length - 2];
            result = search_from_context(context, className);
            if(*((int*)(result[1])) != TT_CLASS)
                return -1;
            classContext = (ClassContext*)(result[0]);
        }

        if(classContext->no_of_superclasses > 0){
            int* indices = classContext->superclass_sequence_index_array;
            char** superclasses = classContext->superclasses;
            int i = 0, no_of_superclasses = classContext->no_of_superclasses;
            char* temp_prefix = NULL;
            while(i < no_of_superclasses && index == -1){
                temp_prefix = merge_strings(prefix == NULL ? "" : prefix, superclasses[indices[i]]);
                strcat(temp_prefix, ".");
                index = find_index_instance_var_of_superclass_from_ObjectContext(objectContext, key, temp_prefix, context);
                i++;
            }
        }
    }
    return index;
}

Value* find_instance_var_from_ObjectContext(ObjectContext* objectContext, char* key, char* prefix, Context* context){
    int length = 0, curr_index;
    char** words = split(key, '.', &length);
    char *temp_key = NULL, *temp_prefix = NULL;
    int index, i;
    if(length == 1){
        index = find_index_from_instance_var_in_ObjectContext(objectContext, key);
        if(prefix != NULL && index == -1){
            length = 0;
            words = split(prefix, '.', &length);
            length--;
            i = length - 1;
            temp_key = key;
            while(index == -1 && i >= 0){
                temp_key = merge_strings(words[i], temp_key);
                i--;
                index = find_index_from_instance_var_in_ObjectContext(objectContext, temp_key);
            }

            if(index == -1)
                index = find_index_instance_var_of_superclass_from_ObjectContext(objectContext, key, prefix, context);
        }
    }
    else{
        temp_key = prefix != NULL ? merge_strings(prefix, key) : key;
        index = find_index_from_instance_var_in_ObjectContext(objectContext, temp_key);
        if(index == -1){
            temp_key = words[length - 1];
            temp_prefix = prefix;
            for(i = 0; i < length - 1; i++){
                temp_prefix = merge_strings(temp_prefix, words[i]);
                strcat(temp_prefix, ".");
            }
            index = find_index_instance_var_of_superclass_from_ObjectContext(objectContext, temp_key, temp_prefix, context);
        }
    }
    return index == -1 ? NULL : objectContext->instance_var[index];
}

Value* find_instance_var_from_only_subclass_ObjectContext(ObjectContext* objectContext, char* key){
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

// void modify_instance_var_from_superclass_of_ObjectContext(ObjectContext* objectContext, char* className, char* key, Value* value){
//     int index = find_index_in_superclass_from_ClassContext(objectContext->static_context, className);
//     if(index == -1){
//         printf("ClassError: %s is not a superclass of %s\n", className, objectContext->static_context->className);
//         exit(0);
//     }
//     modify_instance_var_from_ObjectContext(objectContext->superclass_objects[index], key, value);
// }

void* find_from_ObjectContext(ObjectContext* objectContext, char* key, int type, Context* context, char** prefix_ptr){
    void* answer = NULL;
    switch(type){
        case VAR_NODE:{
            answer = find_instance_var_from_ObjectContext(objectContext, key, *prefix_ptr, context);
            if(answer == NULL){
                answer = find_static_var_in_ClassContext(objectContext->static_context, key, context, prefix_ptr);
            }
            break;
        }
        case FUNCTION_CALL_NODE:{
            answer = find_instance_method_in_ClassContext(objectContext->static_context, key, context, prefix_ptr);
            if(answer == NULL)
                answer = find_static_method_in_ClassContext(objectContext->static_context, key, context, prefix_ptr);
            break;
        }
        default:{
            return NULL;
        }
    }
    return answer;
}

void insert_superclass_objects_into_ObjectContext(ObjectContext* objectContext, Context* context, char* prefix){
    ClassContext* classContext = objectContext->static_context;
    ClassContext* temp_class = NULL;
    int *superclass_names_indices = classContext->superclass_sequence_index_array, *isNode = NULL;
    char** superclass_names = objectContext->superclass_keys;
    char* temp_name = NULL;
    int no_of_superclasses = classContext->no_of_superclasses;
    int index = -1;
    void** results = NULL;
    Node *constructor = NULL, *temp_node = NULL;
    Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
    token->type = TT_VAR, token->line_no = -1, token->col_no = -1;
    temp_node = construct_Node(token, FUNCTION_CALL_NODE);
    temp_node->left = NULL, temp_node->leftType = 0;
    isNode = allocate_ptr_array(4, sizeof(int));
    set_heap_alloced_memory(isNode, 0, 4 * sizeof(int));
    Value* tempo_vals;
    char* temp_prefix = NULL;
    for(int i = 0; i < no_of_superclasses; i++){
        index = superclass_names_indices[i];
        temp_name = superclass_names[index];
        temp_prefix = merge_strings(prefix == NULL ? "" : prefix, temp_name);
        temp_prefix = merge_strings(temp_prefix, ".");
        results = search_from_context(context, temp_name);
        if(*((int*)(results[1])) != TT_CLASS){
            printf("ClassNotFoundError: '%s' class is not defined\n", temp_name);
            exit(0);
        }
        temp_class = (ClassContext*)(results[0]);
        objectContext->superclass_objects[index] = construct_ObjectContext(temp_class, context, prefix);
        constructor = get_constructor_from_ClassContext(temp_class, 0);
        if(constructor->nodeType != NULL_NODE){
            isNode[3]++;
            temp_node->val->val = temp_name;
            tempo_vals = getMethodValue(constructor, context, isNode, 0, objectContext, temp_node, prefix, temp_prefix);
            isNode[3]--;
            if(tempo_vals->valType == TT_ERROR){
                printValue(tempo_vals);
                exit(0);
            }
        }
        objectContext->no_of_superclasses++;
    }
}