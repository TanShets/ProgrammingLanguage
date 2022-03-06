#pragma once
#include "value.h"
#include "default_functions.h"
#include "array_value.h"
#include "class_context/class_context.h"

#define DEFAULT_NO_OF_VALUES 10

void printValue(Value* value){
    // check_byte_content_of_pointer(value->num, 4 * sizeof(char));
	if(value == NULL){
		printf("Null value");
		return;
	}
	switch(value->valType){
		case TT_INT:{
			printf("%d", *((int*)(value->num)));
			break;
		}
		case TT_FLOAT:{
			printf("%lf", *((double*)(value->num)));
			break;
		}
		case TT_ERROR:{
			printError((Error*)value->num);
			break;
		}
		case TT_TRUE:{
			printf("true");
			break;
		}
		case TT_FALSE:{
			printf("false");
			break;
		}
		case TT_NULL:{
			printf("null");
			break;
		}
		case TT_STRING:{
            char* line = (char*)(value->num);
			printf("%s", line);
			break;
		}
        case TT_ARRAY:{
            print_ArrayValue((ArrayValue*)value->num);
            break;
        }
		default:
			printf("%p", value->num);
	}
}

Interpreter* construct_Interpreter(Value** values, int no_of_values, int isBroken){
    // Interpreter* interpreter = (Interpreter*)malloc(sizeof(Interpreter));
    Interpreter* interpreter = (Interpreter*)allocate_ptr_for_size(sizeof(Interpreter));
    interpreter->values = values;
    interpreter->no_of_values = no_of_values;
    interpreter->isBroken = isBroken;
    return interpreter;
}

void expand_Values(Value*** values, int* no_of_values, int new_no_of_values){
    int old_no_of_values = *no_of_values;
    *no_of_values = 2 * new_no_of_values;
    // *values = (Value**)realloc(*values, *no_of_values * sizeof(Value*));
    *values = (Value**)reallocate_heap_alloced_ptr(*values, *no_of_values * sizeof(Value*));
    // Value** new_values = (Value**)calloc(*no_of_values, sizeof(Value*));
    // memcpy(new_values, *values, old_no_of_values * sizeof(Value*));
    // Value** temp = *values;
    // *values = new_values;
    // free(temp);
}

Value* getFunctionCallValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix);

Value* getIndexValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix);
Value* getClassDefinitionValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed);
Value* getObjectValue(Node* node, Context* context, int* isNode, int method_flag, void* needed_context, char* prefix);

Value** getConditionalNodeValue(Node* node, int* no_of_values, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    if(node->nodeType != CONDITIONAL_NODE)
        return NULL;
    
    int n = node->leftType;
    Node** statements = (Node**)node->left;
    Node*** blocks = (Node***)node->right;
    Node** block;
    int* block_lengths = node->block_lengths;
    Value* temp_val;
    Value** values;
    int i = 0, flag = 0, j;
    Interpreter* temp_interpreter;
    while(i < n){
        temp_val = viewNode(statements[i], context, isNode, flag_type, context_needed, prefix);
        switch(temp_val->valType){
            case TT_TRUE:
                flag = 1;
                break;
            case TT_INT:{
                if(*((int*)temp_val->num) != 0)
                    flag = 1;
                break;
            }
            case TT_FLOAT:{
                if(*((double*)temp_val->num) != 0)
                    flag = 1;
                break;
            }
        }

        if(flag == 0)
            i++;
        else
            break;
    }

    if(i < n){
        temp_interpreter = Interpret(blocks[i], block_lengths[i], context, isNode, flag_type, context_needed, prefix);
        *no_of_values = temp_interpreter->no_of_values;
        values = temp_interpreter->values;
        // values = (Value**)calloc(block_lengths[i], sizeof(Value*));
        // // block = blocks[i];
        // *no_of_values = block_lengths[i];
        // for(j = 0; j < block_lengths[i]; j++){
        //     values[j] = viewNode(blocks[i][j], context, isNode);
        //     if(values[j]->valType == TT_ERROR){
        //         values = &values[j];
        //         *no_of_values = 1;
        //         return values;
        //     }
        // }
        return values;
    }
    else{
        if(node->else_block != NULL){
            temp_interpreter = Interpret(node->else_block, node->isVarNode, context, isNode, flag_type, context_needed, prefix);
            *no_of_values = temp_interpreter->no_of_values;
            values = temp_interpreter->values;
            // values = (Value**)calloc(node->isVarNode, sizeof(Value*));
            // // block = blocks[i];
            // block = node->else_block;
            // *no_of_values = node->isVarNode;
            // for(j = 0; j < node->isVarNode; j++){
            //     values[j] = viewNode(block[j], context, isNode);
            //     if(values[j]->valType == TT_ERROR){
            //         values = &values[j];
            //         *no_of_values = 1;
            //         return values;
            //     }
            // }
            return values;
        }
        else{
            *no_of_values = 1;
            // values = (Value**)malloc(sizeof(Value*));
            values = (Value**)allocate_ptr_for_size(sizeof(Value*));
            // Token* token = (Token*)malloc(sizeof(Token));
            Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
            // memcpy(token, node->val, sizeof(Token));
            copy_heap_alloced_memory(token, node->val, sizeof(Token));
            token->type = TT_NULL;
            // token->val = (int*)malloc(sizeof(int));
            token->val = (int*)allocate_ptr_for_size(sizeof(int));
            *((int*)token->val) = 0;
            *values = construct_Value(token);
            return values;
        }
    }
}

Value* getArrayValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    if(node->nodeType != ARRAY_NODE)
        return NULL;
    Value* val = construct_Value(node->val);
    ArrayValue* arrayValue = construct_ArrayValue(node->rightType);
    Node** keys = (Node**)node->left;
    Node** values = (Node**)node->right;
    Value *temp_key, *temp_val;
    int i;
    for(i = 0; i < node->rightType; i++){
        temp_key = keys[i] != NULL ? viewNode(keys[i], context, isNode, flag_type, context_needed, prefix) : NULL;
        if(temp_key != NULL && temp_key->valType == TT_ERROR)
            return temp_key;
        temp_val = viewNode(values[i], context, isNode, flag_type, context_needed, prefix);
        if(temp_val->valType == TT_ERROR)
            return temp_val;
        
        modify_ArrayValue(arrayValue, temp_key, temp_val);
    }
    val->num = arrayValue;
    return val;
}

Value* getVarValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
	Token* nodeVal = (Token*)(node->val);
	char* key = (char*)(nodeVal->val);
    Value* val = NULL;
    ObjectContext* objectContext = NULL;
    ClassContext* classContext = NULL;
	void** answer = search_from_local_context(context, key);
	int line_no = nodeVal->line_no, col_no = nodeVal->col_no;
	Token* token;
	if(answer[1] != NULL && *((int*)answer[1]) != TT_ERROR){
		// token = (Token*)malloc(sizeof(Token));
		token = (Token*)allocate_ptr_for_size(sizeof(Token));
		token->type = *((int*)answer[1]);
		token->val = answer[0];
		token->line_no = node->val->line_no, token->col_no = node->val->col_no;
		return construct_Value(token);
	}
    else if(flag_type != -1 && context_needed != NULL){
        char* prefix_copy = prefix != NULL ? allocate_ptr_array(strlen(prefix) + 5, sizeof(char)) : NULL;
        if(prefix != NULL) strcpy(prefix_copy, prefix);

        switch(flag_type){
            case IS_INSTANCE_METHOD:{
                objectContext = (ObjectContext*)(context_needed);
                classContext = objectContext->static_context;
                val = find_instance_var_from_ObjectContext(objectContext, key, prefix_copy, context);
                if(val == NULL)
                    val = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                break;
            }
            case IS_STATIC_METHOD:{
                classContext = (ClassContext*)context_needed;
                val = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                break;
            }
            case IS_CONSTRUCTOR:{
                objectContext = (ObjectContext*)(context_needed);
                classContext = objectContext->static_context;
                val = find_instance_var_from_ObjectContext(objectContext, key, prefix_copy, context);
                if(val == NULL)
                    val = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                break;
            }
        }

        if(val != NULL)
            return val;
    }
	
    answer = search_from_context(context, key);
    if(answer[1] != NULL && *((int*)answer[1]) != TT_ERROR){
		// token = (Token*)malloc(sizeof(Token));
		token = (Token*)allocate_ptr_for_size(sizeof(Token));
		token->type = *((int*)answer[1]);
		token->val = answer[0];
		token->line_no = node->val->line_no, token->col_no = node->val->col_no;
		return construct_Value(token);
	}

    return construct_Value(
        make_error(
            ValueNotFoundError(key, line_no, col_no), line_no, col_no
        )
    );
}

Value* viewNode(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
	Value* answer = NULL;
	switch(node->nodeType){
        case VAR_ASSIGN_NODE:{
            answer = getVarAssignValue(node, context, isNode, flag_type, context_needed, prefix);
            break;
        }
		case NUM_NODE:{
			answer = getNumValue(node);
			break;
		}
		case BOOLEAN_NODE:{
			answer = getBooleanValue(node);
			break;
		}
		case NULL_NODE:{
			answer = getNullValue(node);
			break;
		}
		case BIN_OP_NODE:{
			answer = getBinOpValue(node, context, isNode, flag_type, context_needed, prefix);
			break;
		}
		case UN_OP_NODE:{
			answer = getUnOpValue(node, context, isNode, flag_type, context_needed, prefix);
			break;
		}
		case VAR_NODE:{
			answer = getVarValue(node, context, isNode, flag_type, context_needed, prefix);
			break;
		}
		case BREAK_NODE:{
			answer = getBreakValue(node);
			break;
		}
		case FUNCTION_DEFINITION_NODE:{
			answer = getFunctionDefinitionValue(node, context);
			break;
		}
        case STRING_NODE:{
            answer = getStringValue(node);
            break;
        }
        case FUNCTION_CALL_NODE:{
            answer = getFunctionCallValue(
                        node, context, isNode, 
                        flag_type == IS_CONSTRUCTOR ? IS_CONSTRUCTOR_CALLED_IN_CONSTRUCTOR : flag_type, 
                        context_needed, prefix
                    );
            break;
        }
        case ARRAY_NODE:{
            answer = getArrayValue(node, context, isNode, flag_type, context_needed, prefix);
            break;
        }
        case INDEX_NODE:{
            answer = getIndexValue(node, context, isNode, flag_type, context_needed, prefix);
            break;
        }
        case CLASS_DEFINITION_NODE:{
            answer = getClassDefinitionValue(node, context, isNode, flag_type, context_needed);
            break;
        }
        case OBJECT_NODE:{
            answer = getObjectValue(node, context, isNode, flag_type, context_needed, prefix);
            break;
        }
		default:{
			answer = NULL;
		}
	}
	return answer;
}

int convertToBoolean(Value* value){
    switch(value->valType){
        case TT_TRUE:
            return 1;
        case TT_FALSE:
            return 0;
        case TT_NULL:
            return 0;
        case TT_INT:{
            return *((int*)(value->num)) == 0 ? 0 : 1;
        }
        case TT_FLOAT:{
            return *((double*)(value->num)) == 0 ? 0 : 1;
        }
        default:
            return 0;
    }
}

int isNegativeValue(Value* value){
    switch(value->valType){
        case TT_TRUE:
            return 0;
        case TT_FALSE:
            return 0;
        case TT_NULL:
            return 0;
        case TT_INT:{
            return *((int*)(value->num)) >= 0 ? 0 : 1;
        }
        case TT_FLOAT:{
            return *((double*)(value->num)) >= 0 ? 0 : 1;
        }
        default:
            return 0;
    }
}

Value* getMethodValue(
    Node* node, Context* context, int* isNode, int method_flag, 
    void* needed_context, Node* fn_call, char* prefix, char* new_prefix
){
    ObjectContext* objectContext = NULL;
    ClassContext* classContext = NULL;
    Token* token = NULL;
    Value* value = NULL;
    if(needed_context == NULL)
        return NULL;

    // switch(method_flag){
    //     case IS_CONSTRUCTOR:{
    //         objectContext = (ObjectContext*)needed_context;
    //         classContext = objectContext->static_context;
    //         break;
    //     }
    //     case IS_INSTANCE_METHOD:{
    //         objectContext = (ObjectContext*)needed_context;
    //         classContext = objectContext->static_context;
    //         break;
    //     }
    //     case IS_STATIC_METHOD:{
    //         classContext = (ClassContext*)needed_context;
    //         break;
    //     }
    // }

    Node* fn_defn = node;
    int block_size = fn_defn->rightType;
    Node** block = (Node**)fn_defn->right;
    Node** parameters = (Node**)fn_defn->left;
    Node** param_vals = (Node**)fn_call->left;
    int no_of_parameters = fn_call->leftType;
    int i;
    Value* temp_value;
    Interpreter* temp_interpreter;
    Context* function_context = construct_Context();
    char* key = NULL;
    function_context->parent = context;
    for(i = 0; i < no_of_parameters; i++){
        key = (char*)(((Token*)parameters[i]->val)->val);
        temp_value = viewNode(param_vals[i], context, isNode, method_flag, needed_context, prefix);
        modify_context(function_context, key, temp_value->num, temp_value->valType);
    }
    // key = (char*)(fn_call->val->val);
    // key = merge_strings(key, ".");
    // if(prefix != NULL)
    //     key = merge_strings(new_prefix, key);
    isNode[1]++;
    isNode[3]++;
    temp_interpreter = Interpret(block, block_size, function_context, isNode, method_flag, needed_context, new_prefix);
    isNode[3]--;
    if(temp_interpreter->no_of_values == 1 && (isNode[1] < 0 || isNode[2] > 0)){
        if(isNode[1] < 0)
            isNode[1] *= -1;
        return *(temp_interpreter->values);
    }
    else{
        token = (Token*)allocate_ptr_for_size(sizeof(Token));
        token->type = TT_NULL;
        char* word = {"null"};
        token->val = word, token->line_no = ((Token*)node->val)->line_no;
        token->col_no = ((Token*)node->val)->col_no;
        return construct_Value(token);
    }
    return value;
}

Value* extract_IndexNode_from_Object(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* key, char** prefix_ptr){
    char* new_key = NULL;
    switch (node->nodeType)
    {
        case INDEX_NODE:{
            Value* val = extract_IndexNode_from_Object((Node*)(node->left), context, isNode, flag_type, context_needed, key, prefix_ptr);
            if(val->valType != TT_ARRAY){
                return construct_Value(
                    make_error(
                        SyntaxError("variable of type 'array'", node->val->line_no, node->val->col_no),
                        node->val->line_no, node->val->col_no
                    )
                );
            }
            ArrayValue* arrayValue = (ArrayValue*)(val->num);
            Node** indices = (Node**)(node->right);
            int count = node->rightType;
            Value *temp_key, *temp_val = NULL;
            // Value** values = count > 1 ? (Value**)calloc(count, sizeof(Value*)) : NULL;
            Value** values = count > 1 ? (Value**)allocate_ptr_array(count, sizeof(Value*)) : NULL;
            for(int i = 0; i < count; i++){
                temp_key = viewNode(indices[i], context, isNode, flag_type, context_needed, *prefix_ptr);
                if(temp_key->valType == TT_ERROR)
                    return temp_key;
                
                temp_val = find_in_ArrayValue(arrayValue, temp_key);
                if(temp_val->valType == TT_ERROR)
                    return temp_val;
                
                if(count > 1){
                    values[i] = temp_val;
                }
            }

            if(count > 1){
                // temp_val = (Value*)malloc(sizeof(Value));
                temp_val = (Value*)allocate_ptr_for_size(sizeof(Value));
                temp_val->valType = TT_ARRAY;
                temp_val->line_no = values[0]->line_no;
                temp_val->col_no = values[1]->col_no;
                temp_val->num = get_ArrayValue_from_values(values, count);
                return temp_val;
            }
            else
                return temp_val;
        }
        case VAR_NODE:
            new_key = key == NULL ? (char*)node->val->val : merge_strings(key, (char*)node->val->val);
            // prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
            // if(prefix == NULL) strcpy(prefix_copy, prefix);
            return flag_type == IS_CONSTRUCTOR || flag_type == IS_INSTANCE_METHOD ? 
                    find_from_ObjectContext((ObjectContext*)(context_needed), new_key, VAR_NODE, context, prefix_ptr) : 
                    find_static_var_in_ClassContext((ClassContext*)(context_needed), new_key, context, prefix_ptr);
        case FUNCTION_CALL_NODE:
            new_key = NULL;
            new_key = key == NULL ? (char*)node->val->val : merge_strings(key, (char*)node->val->val);
            // prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
            return flag_type == IS_CONSTRUCTOR || flag_type == IS_INSTANCE_METHOD ? 
                    find_from_ObjectContext((ObjectContext*)(context_needed), new_key, FUNCTION_CALL_NODE, context, prefix_ptr) : 
                    find_static_method_in_ClassContext((ClassContext*)(context_needed), new_key, context, prefix_ptr);
        default:
            return NULL;
    }
}

Value* getObjectValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    if(node->nodeType != OBJECT_NODE)
        return NULL;
    Node* object = (Node*)(node->left);
    int isStatic = 0;
    char* name = NULL;
    ClassContext *classContext = NULL, *classContext2 = NULL;
    ObjectContext* objectContext = NULL;
    Value* object_val = NULL;
    int isAmbigious = 0;
    switch (object->nodeType)
    {
        case VAR_NODE:{
            name = (char*)(object->val->val);
            void** result = search_from_context(context, name);
            if(*((int*)result[1]) == TT_CLASS){
                if(isNode[3] > 0 && (flag_type == IS_CONSTRUCTOR || flag_type == IS_INSTANCE_METHOD) && 
                    context_needed != NULL){
                    objectContext = (ObjectContext*)context_needed;
                    classContext = objectContext->static_context;
                    classContext2 = (ClassContext*)result[0];
                    isAmbigious = 1;
                }
                else{
                    classContext = (ClassContext*)(result[0]);
                    isStatic = 1;
                }
            }
            else{
                object_val = viewNode(object, context, isNode, flag_type, context_needed, prefix);
                if(object_val == NULL){
                    return construct_Value(
                        make_error(
                            NullOperationError(".", object->val->line_no, object->val->col_no),
                            object->val->line_no, object->val->col_no
                        )
                    );
                }
                else if(object_val != NULL && object_val->valType != TT_OBJECT){
                    return construct_Value(
                        make_error(
                            SyntaxError("Object", object->val->line_no, object->val->col_no),
                            object->val->line_no, object->val->col_no
                        )
                    );
                }
                objectContext = (ObjectContext*)(object_val->num);
            }
            break;
        }
        case THIS_NODE:{
            if(isNode[3] > 0 && context_needed != NULL)
                objectContext = (ObjectContext*)context_needed;
            else{
                return construct_Value(
                    make_error(
                        ClassSyntaxError(
                            "'this' cannot be used outside of a class",
                            object->val->line_no, object->val->col_no
                        ), object->val->line_no, object->val->col_no
                    )
                );
            }
            break;
        }
        default:{
            object_val = viewNode(object, context, isNode, flag_type, context_needed, prefix);
            objectContext = (ObjectContext*)(object_val->num);
            if(object_val != NULL && object_val->valType != TT_OBJECT){
                return construct_Value(
                    make_error(
                        SyntaxError("'Object'", object->val->line_no, object->val->col_no),
                        object->val->line_no, object->val->col_no
                    )
                );
            }
        }
    }
    
    // objectContext = isStatic ? NULL : (ObjectContext*)(object_val->num);
    classContext = isStatic ? classContext : objectContext->static_context;
    char* className = classContext->className;
    Node* right = (Node*)(node->right);
    Value* value = NULL;
    Node* function_node = NULL;
    char* key = NULL;
    void** answer = NULL;
    Value* temp_answer = NULL;
    char* prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
    if(prefix != NULL) strcpy(prefix_copy, prefix);
    /*
    switch(right->nodeType){
        case VAR_NODE:{
            key = (char*)(right->val->val);
            if(isStatic){
                value = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
            }
            else{
                value = (Value*)find_instance_var_from_ObjectContext(objectContext, key, prefix_copy, context);
                if(value == NULL)
                    value = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
            }
            return value;
        }
        case FUNCTION_CALL_NODE:{
            name = (char*)(right->val->val);
            if(strcmp(name, "constructor") == 0 || strcmp(name, className) == 0){
                return construct_Value(
                    make_error(
                        InvalidStaticFunctionNameError(name, FUNCTION_CALL_NODE, right->val->line_no, right->val->col_no),
                        right->val->line_no, right->val->col_no
                    )
                );
            }
            char* num_params = INT_TO_STR(right->leftType);
            char* temp_name = (char*)allocate_ptr_array(strlen(name) + strlen(num_params) + 5, sizeof(char));
            strncpy(temp_name, name, strlen(name));
            temp_name[strlen(name)] = '\0';
            strcat(temp_name, num_params);
            name = temp_name;
            int flag_used = -1;

            if(isStatic){
                function_node = find_static_method_in_ClassContext(classContext, name, context, &prefix_copy);
                flag_used = IS_STATIC_METHOD;
            }
            else{
                function_node = find_instance_method_in_ClassContext(classContext, name, context, &prefix_copy);
                flag_used = IS_INSTANCE_METHOD;
                if(function_node == NULL){
                    function_node = find_static_method_in_ClassContext(classContext, name, context, &prefix_copy);
                    flag_used = IS_STATIC_METHOD;
                }
            }
            
            if(function_node == NULL){
                return construct_Value(
                    make_error(
                        MethodNotFoundError(
                            (char*)(right->val->val), num_params, right->val->line_no, right->val->col_no
                        ),
                        right->val->line_no, right->val->col_no
                    )
                );
            }
            isNode[3]++;
            value = getMethodValue(
                function_node, context, isNode, flag_used, 
                flag_used == IS_STATIC_METHOD ? (void*)classContext : (void*)objectContext,
                right, prefix
            );
            isNode[3]--;
            return value;
        }
    }
    */
    Node* temp = (Node*)(node->left);
    /*
    switch(temp->nodeType){
        case THIS_NODE:{
            if(isNode[3] > 0 && (flag_type == IS_INSTANCE_METHOD || flag_type == IS_CONSTRUCTOR) && context_needed != NULL){
                objectContext = (ObjectContext*)(context_needed);
                classContext = objectContext->static_context;
            }
            else{
                return construct_Value(
                    make_error(
                        ClassSyntaxError(
                            "Cannot use 'this' keyword outside instance method or constructor\n",
                            temp->val->line_no, temp->val->col_no
                        ), temp->val->line_no, temp->val->col_no
                    )
                );
            }
            break;
        }
        case VAR_NODE:{
            key = (char*)(temp->val->val);
            answer = search_from_context(context, key);
            isStatic = 0;
            int valType_answer = -1;
            if(*((int*)answer[1]) != TT_ERROR){
                valType_answer = *((int*)answer[1]);
                switch(valType_answer){
                    case TT_OBJECT:{
                        temp_answer = (Value*)(answer[0]);
                        objectContext = (ObjectContext*)(temp_answer->num);
                        classContext = objectContext->static_context;
                        key = NULL;
                        break;
                    }
                    case TT_CLASS:{
                        if(isNode[3] > 0 && (flag_type == IS_INSTANCE_METHOD || flag_type == IS_CONSTRUCTOR) 
                            && context_needed != NULL){
                            objectContext = (ObjectContext*)(context_needed);
                            classContext = objectContext->static_context;
                            if(find_superclass_in_ClassContext(classContext, key) != NULL){
                                strcat(key, ".");
                                classContext2 = (ClassContext*)answer[0];
                                isAmbigious = 1;
                            }
                            else{
                                isStatic = 1;
                                objectContext = NULL;
                                classContext = (ClassContext*)(answer[0]);
                                key = NULL;
                            }
                        }
                        else{
                            isStatic = 1;
                            objectContext = NULL;
                            classContext = (ClassContext*)(answer[0]);
                            key = NULL;
                        }
                        break;
                    }
                    default:{
                        return construct_Value(
                            make_error(
                                SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                temp->val->line_no, temp->val->col_no
                            )
                        );
                    }
                }
            }
            else{
                if(isNode[3] > 0 && context_needed != NULL){
                    switch(flag_type){
                        case IS_INSTANCE_METHOD:{
                            objectContext = (ObjectContext*)(context_needed);
                            classContext = objectContext->static_context;
                            temp_answer = (Value*)find_from_ObjectContext(objectContext, key, VAR_NODE, context, &prefix_copy);
                            break;
                        }
                        case IS_STATIC_METHOD:{
                            objectContext = NULL;
                            classContext = (ClassContext*)(context_needed);
                            prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
                            temp_answer = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                            break;
                        }
                        default:{
                            return construct_Value(
                                make_error(
                                    ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                    }
                    key = NULL;
                }
                else{
                    return construct_Value(
                        make_error(
                            ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
                }
            }
            break;
        }
        case INDEX_NODE:{
            temp_answer = getIndexValue(temp, context, isNode, flag_type, context_needed, prefix);
            if(temp_answer == NULL || temp_answer->valType != TT_OBJECT){
                return temp_answer != NULL && temp_answer->valType == TT_ERROR ? temp_answer : 
                    construct_Value(
                        make_error(
                            SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
            }
            objectContext = (ObjectContext*)(temp_answer->num);
            classContext = (ClassContext*)(objectContext->static_context);
            key = NULL;
            break;
        }
        default:{
            return construct_Value(
                make_error(
                    SyntaxError("varName", temp->val->line_no, temp->val->col_no),
                    temp->val->line_no, temp->val->col_no
                )
            );
        }
    }
    */
    temp = (Node*)(node->right);
    Node *fp = NULL, *temp_node = NULL;
    right = NULL;
    char *num_params, *temp_name;
    name = NULL;
    while(temp != NULL && temp->nodeType == OBJECT_NODE){
        fp = temp;
        temp = (Node*)(temp->left);
        switch(temp->nodeType){
            case VAR_NODE:{
                name = (char*)(temp->val->val);
                key = key == NULL ? name : merge_strings(key, name);
                answer = search_from_context(context, name);
                if(*((int*)answer[1]) == TT_CLASS)
                    strcat(key, ".");
                else{
                    temp_answer = isStatic ? 
                    find_static_var_in_ClassContext(classContext, key, context, &prefix_copy) : 
                    find_instance_var_from_ObjectContext(objectContext, key, prefix_copy, context);
                    if(!isStatic && temp_answer == NULL)
                        temp_answer = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                    

                    if(temp_answer == NULL){
                        if(isAmbigious){
                            temp_answer = find_static_var_in_ClassContext(classContext2, key, context, &prefix_copy);
                            isAmbigious = 0;
                        }

                        if(temp_answer != NULL){
                            classContext = classContext2;
                            isStatic = 1;
                        }
                        else{
                            return construct_Value(
                                make_error(
                                    ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                    }
                    else if(temp_answer->valType == TT_OBJECT){
                        objectContext = (ObjectContext*)(temp_answer->num);
                        classContext = objectContext->static_context;
                    }
                    else{
                        return construct_Value(
                            make_error(
                                SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                temp->val->line_no, temp->val->col_no
                            )
                        );
                    }

                    key = NULL;
                    isAmbigious = 0;
                }
                break;
            }
            case INDEX_NODE:{
                temp_answer = extract_IndexNode_from_Object(
                                temp, context, isNode, 
                                isStatic ? IS_STATIC_METHOD : IS_INSTANCE_METHOD, 
                                isStatic ? (void*)classContext : (void*)objectContext,
                                key, &prefix_copy
                            );
                if(temp_answer == NULL || temp_answer->valType != TT_OBJECT){
                    return construct_Value(
                        make_error(
                            SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
                }
                objectContext = (ObjectContext*)(temp_answer->num);
                classContext = objectContext->static_context;
                key = NULL;
                break;
            }
            case FUNCTION_CALL_NODE:{
                name = (char*)(temp->val->val);
                num_params = INT_TO_STR(temp->leftType);
                temp_name = (char*)allocate_ptr_array(strlen(name) + strlen(num_params) + 5, sizeof(char));
                strncpy(temp_name, name, strlen(name));
                temp_name[strlen(name)] = '\0';
                strcat(temp_name, num_params);
                name = temp_name;
                key = key == NULL ? merge_strings("", name) : merge_strings(key, name);
                temp_node = isStatic ? find_static_method_in_ClassContext(classContext, key, context, &prefix_copy) : 
                            (Node*)find_from_ObjectContext(objectContext, key, FUNCTION_CALL_NODE, context, &prefix_copy);
                

                if(temp_node == NULL){
                    if(isAmbigious){
                        temp_node = find_static_method_in_ClassContext(classContext2, key, context, &prefix_copy);
                        isAmbigious = 0;
                    }

                    if(temp_node != NULL){
                        classContext = classContext2;
                        isStatic = 1;
                    }
                    else{
                        return construct_Value(
                            make_error(
                                MethodNotFoundError(
                                    (char*)(temp->val->val), num_params, temp->val->line_no, temp->val->col_no
                                ), temp->val->line_no, temp->val->col_no
                            )
                        );
                    }
                }

                isStatic = isStatic ? isStatic : 
                            find_instance_method_in_ClassContext(classContext, key, context, &prefix_copy) == NULL ? 1 : 
                            isStatic;

                temp_answer = getMethodValue(
                                temp_node, context, isNode, isStatic ? IS_STATIC_METHOD : IS_INSTANCE_METHOD,
                                isStatic ? (void*)classContext : (void*)objectContext, temp, prefix, prefix_copy
                            );
                
                if(temp_answer == NULL || temp_answer->valType == TT_ERROR)
                    return temp_answer;
                else if(temp_answer->valType == TT_OBJECT){
                    objectContext = (ObjectContext*)(temp_answer->num);
                    classContext = objectContext->static_context;
                    key = NULL;
                }
                else{
                    return construct_Value(
                        make_error(
                            SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
                }
                break;
            }
        }
        temp = (Node*)(fp->right);
    }

    if(temp == NULL)
        return NULL;
    else{
        Value* valuex = NULL;
        Node* methodx = NULL;
        if(temp->nodeType == VAR_NODE){
            name = (char*)(temp->val->val);
            name = key == NULL ? name : merge_strings(key, name);
            if(isStatic){
                valuex = find_static_var_in_ClassContext(classContext, name, context, &prefix_copy);
                if(valuex == NULL){
                    return construct_Value(
                        make_error(
                            StaticVarNotFoundError(
                                name, classContext->className, temp->val->line_no, temp->val->col_no
                            ), temp->val->line_no, temp->val->col_no
                        )
                    );
                }
                return valuex;
            }
            else{
                if(flag_type != IS_CONSTRUCTOR){
                    valuex = find_instance_var_from_ObjectContext(objectContext, name, prefix_copy, context);
                    if(valuex == NULL)
                        valuex = find_static_var_in_ClassContext(objectContext->static_context, name, context, &prefix_copy);
                    
                    if(valuex == NULL){
                        return construct_Value(
                            make_error(
                                InstanceVarNotFoundError(
                                    name, objectContext->static_context->className, 
                                    temp->val->line_no, temp->val->col_no
                                ), temp->val->line_no, temp->val->col_no
                            )
                        );
                    }
                }
                return valuex;
            }
        }
        else if(temp->nodeType == FUNCTION_CALL_NODE){
            name = (char*)(temp->val->val);
            name = key == NULL ? name : merge_strings(key, name);
            char* buffer_name = name;
            name = merge_strings(name, INT_TO_STR(temp->leftType));
            int flag_sent = -1;
            if(isStatic){
                flag_sent = IS_STATIC_METHOD;
                methodx = find_static_method_in_ClassContext(classContext, name, context, &prefix_copy);
                if(methodx == NULL){
                    return construct_Value(
                        make_error(
                            MethodNotFoundError(
                                buffer_name, INT_TO_STR(temp->leftType), temp->val->line_no, temp->val->col_no
                            ), temp->val->line_no, temp->val->col_no
                        )
                    );
                }
            }
            else{
                flag_sent = IS_INSTANCE_METHOD;
                methodx = find_instance_method_in_ClassContext(objectContext->static_context, name, context, &prefix_copy);
                if(methodx == NULL){
                    methodx = find_static_method_in_ClassContext(objectContext->static_context, name, context, &prefix_copy);
                    isStatic = 1;
                    flag_sent = IS_STATIC_METHOD;
                }

                if(methodx == NULL){
                    return construct_Value(
                        make_error(
                            MethodNotFoundError(
                                buffer_name, INT_TO_STR(temp->leftType), 
                                temp->val->line_no, temp->val->col_no
                            ), temp->val->line_no, temp->val->col_no
                        )
                    );
                }
            }
            return getMethodValue(
                methodx, context, isNode, flag_sent, 
                isStatic ? (void*)classContext : (void*)objectContext, 
                temp, prefix, prefix_copy
            );
        }
        else{
            return construct_Value(
                make_error(
                    SyntaxError("varName", temp->val->line_no, temp->val->col_no),
                    temp->val->line_no, temp->val->col_no
                )
            );
        }
    }
    return value;
}

Value* getClassDefinitionValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed){
    if(node->nodeType != CLASS_DEFINITION_NODE)
        return NULL;
    
    Node **superclasses = (Node**)node->left, **body = (Node**)node->right;
    int no_of_superclasses = node->leftType, no_of_nodes = node->rightType;
    char* className = (char*)(((Node*)node->else_block)->val->val);
    void** temp_result = search_from_context(context, className);
    int type = *((int*)(temp_result[1]));
    if(type != TT_ERROR){
        char statement[200] = {"Redefinition of "};
        strcat(statement, className);
        strcat(statement, "' when it already exists\n");
        return construct_Value(
            make_error(
                ClassSyntaxError(
                    statement, node->val->line_no, node->val->col_no
                ), node->val->line_no, node->val->col_no
            )
        );
    }
    ClassContext* classContext = construct_ClassContext(className);
    Node* temp_node = NULL;
    int i;
    isNode[3]++;
    for(i = 0; i < no_of_superclasses; i++){
        if(superclasses[i]->nodeType != VAR_NODE){
            return construct_Value(
                make_error(
                    ClassSyntaxError(
                        "Illegal ClassName given\n", superclasses[i]->val->line_no, superclasses[i]->val->col_no
                    ), superclasses[i]->val->line_no, superclasses[i]->val->col_no
                )
            );
        }
        temp_result = search_from_context(context, (char*)(superclasses[i]->val->val));
        type = *((int*)(temp_result[1]));
        if(type != TT_CLASS){
            return construct_Value(
                make_error(
                    ClassSyntaxError(
                        "Expected a 'ClassName'\n", superclasses[i]->val->line_no, superclasses[i]->val->col_no
                    ), superclasses[i]->val->line_no, superclasses[i]->val->col_no
                )
            );
        }
        insert_superclass_into_ClassContext(classContext, (char*)(superclasses[i]->val->val));
    }

    for(i = 0; i < no_of_nodes; i++){
        temp_node = body[i];
        switch(temp_node->nodeType){
            case FUNCTION_DEFINITION_NODE:{
                char* func_name = (char*)(((Token*)(temp_node->else_block))->val);
                if(strcmp(func_name, "constructor") == 0 || strcmp(func_name, className) == 0){
                    int outcome = insert_constructor_into_ClassContext(classContext, temp_node->leftType, temp_node);
                    if(!outcome){
                        exit(0);
                    }
                }
                else{
                    char* num_params = INT_TO_STR(temp_node->leftType);
                    char* temp_name = (char*)allocate_ptr_array(strlen(func_name) + strlen(num_params) + 5, sizeof(char));
                    strncpy(temp_name, func_name, strlen(func_name));
                    temp_name[strlen(func_name)] = '\0';
                    strcat(temp_name, num_params);
                    modify_instance_method_in_ClassContext(classContext, temp_name, temp_node);
                }
                break;
            }
            case STATIC_FUNCTION_DEFINITION_NODE:{
                Node* func = (Node*)(temp_node->right);
                char* func_name = (char*)(((Token*)(func->else_block))->val);
                char* num_params = INT_TO_STR(func->leftType);
                char* temp_name = (char*)allocate_ptr_array(strlen(func_name) + strlen(num_params) + 5, sizeof(char));
                strncpy(temp_name, func_name, strlen(func_name));
                temp_name[strlen(func_name)] = '\0';
                strcat(temp_name, num_params);
                if(strcmp(func_name, "constructor") == 0 || strcmp(func_name, className) == 0){
                    return construct_Value(
                        make_error(
                            InvalidStaticFunctionNameError(
                                func_name, FUNCTION_CALL_NODE, temp_node->val->line_no, temp_node->val->col_no
                            ),
                            temp_node->val->line_no, temp_node->val->col_no
                        )
                    );
                }
                modify_static_method_in_ClassContext(classContext, temp_name, func);
                break;
            }
            case VAR_ASSIGN_NODE:{
                Node* left = (Node*)temp_node->left;
                if(left->nodeType != VAR_NODE){
                    return construct_Value(
                        make_error(
                            SyntaxError("varName", left->val->line_no, left->val->col_no),
                            left->val->line_no, left->val->col_no
                        )
                    );
                }
                char* name = (char*)(left->val->val);
                Node* right = (Node*)(temp_node->right);
                Value* right_val = viewNode(right, context, isNode, flag_type, context_needed, NULL);
                if(right_val != NULL && right_val->valType != TT_ERROR){
                    modify_static_var_in_ClassContext(classContext, name, right_val);
                }
                break;
            }
        }
    }
    isNode[3]--;
    Node* default_constructor = get_constructor_from_ClassContext(classContext, 0);
    Token* token = allocate_ptr_for_size(sizeof(Token));
    token->type = TT_NULL;
    token->val = null_key_for_class_context;
    token->line_no = -1, token->col_no = -1;
    if(default_constructor == NULL){
        Node* new_constructor = NullNode(token);
        i = insert_constructor_into_ClassContext(classContext, 0, new_constructor);
        if(!i)
            exit(0);
    }
    modify_context(context, className, classContext, TT_CLASS);
    return construct_Value(token);
}

Value* getIndexValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    Node* starter = (Node*)(node->left);
    Value* arr_val = viewNode(starter, context, isNode, flag_type, context_needed, prefix);
    if(arr_val->valType != TT_ARRAY){
        char foofighter[] = {"foo"};
        char* key = starter->nodeType == VAR_NODE || 
                    starter->nodeType == FUNCTION_CALL_NODE ? 
                    (char*)(starter->val->val) : foofighter;
        
        return construct_Value(make_error(
            IndexError(key, starter->val->line_no, starter->val->col_no),
            starter->val->line_no, starter->val->col_no
        ));
    }

    ArrayValue* arrayValue = (ArrayValue*)(arr_val->num);
    Node** indices = (Node**)(node->right);
    int count = node->rightType;

    Value *temp_key, *temp_val = NULL;
    // Value** values = count > 1 ? (Value**)calloc(count, sizeof(Value*)) : NULL;
    Value** values = count > 1 ? (Value**)allocate_ptr_array(count, sizeof(Value*)) : NULL;
    for(int i = 0; i < count; i++){
        temp_key = viewNode(indices[i], context, isNode, flag_type, context_needed, prefix);
        if(temp_key->valType == TT_ERROR)
            return temp_key;
        
        temp_val = find_in_ArrayValue(arrayValue, temp_key);
        if(temp_val->valType == TT_ERROR)
            return temp_val;
        
        if(count > 1){
            values[i] = temp_val;
        }
    }

    if(count > 1){
        // temp_val = (Value*)malloc(sizeof(Value));
        temp_val = (Value*)allocate_ptr_for_size(sizeof(Value));
        temp_val->valType = TT_ARRAY;
        temp_val->line_no = values[0]->line_no;
        temp_val->col_no = values[1]->col_no;
        temp_val->num = get_ArrayValue_from_values(values, count);
        return temp_val;
    }
    else
        return temp_val;
}

Value** getLoopNodeValue(Node* node, int* no_of_values, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    // Value** values = (Value**)calloc(DEFAULT_NO_OF_VALUES, sizeof(Value*));
    Value** values = (Value**)allocate_ptr_array(DEFAULT_NO_OF_VALUES, sizeof(Value*));
    int max_no_of_values = DEFAULT_NO_OF_VALUES;
    int i, hasStarted = 0;
    *no_of_values = 0;
    Value* temp_value;
    Value* statement_outcome = NULL;
    Interpreter* temp_interpreter;
    int flag = 0;
    if(node->val->type == TT_WHILE){
        //statement_outcome = viewNode(node->left, context, isNode);
        while(convertToBoolean(viewNode(node->left, context, isNode, flag_type, context_needed, prefix))){
            isNode[0]++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isNode, flag_type, context_needed, prefix);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            // memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            copy_heap_alloced_memory(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            if(isNode[0] < 0){
                isNode[0] *= -1;
                flag = 1;
            }
            else if(isNode[1] < 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            else if(isNode[2] > 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            isNode[0]--;
            if(flag == 1)
                break;
        }
    }
    else{
        Node* condition = (Node*)(node->left);
        // left = loop condition, right = loop block
        Node* toNode = (Node*)(condition->right);
        // condition: x = a to b change = c, left=x, right= (a to b change = c)
        Node* change_node = (Node*)(toNode->right);
        // a to b change = c, right: change = c
        Node* toVals = (Node*)(toNode->left);
        // a to b change = c, left: a to b
        Value* change_val = viewNode(change_node, context, isNode, flag_type, context_needed, prefix);
        condition->right = (Node*)toVals->left;
        // a to b, left: a, right: b
        temp_value = viewNode(condition, context, isNode, flag_type, context_needed, prefix);
        // Assigns iterator which in this case x to a
        //From now the iterator will be incremented by value change until value b is attained or surpassed
        int criterion = isNegativeValue(change_val) ? TT_GREATER_THAN : TT_LESS_THAN;
        Value* right_end = viewNode(toVals->right, context, isNode, flag_type, context_needed, prefix);
        while(operateValues(temp_value, right_end, criterion, 0)->valType == TT_TRUE){
            if(hasStarted == 0){
                hasStarted = 1;
                condition->val->type = TT_INCREMENT;
                condition->right = change_node;
                condition->rightType = change_node->nodeType;
            }
            isNode[0]++;
            temp_interpreter = Interpret(node->right, node->rightType, context, isNode, flag_type, context_needed, prefix);
            if(*no_of_values + temp_interpreter->no_of_values >= max_no_of_values)
                expand_Values(&values, &max_no_of_values, *no_of_values + temp_interpreter->no_of_values);
            
            // memcpy(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            copy_heap_alloced_memory(values + *no_of_values, temp_interpreter->values, temp_interpreter->no_of_values * sizeof(Value*));
            *no_of_values += temp_interpreter->no_of_values;
            temp_value = viewNode(condition, context, isNode, flag_type, context_needed, prefix);
            if(isNode[0] < 0){
                isNode[0] *= -1;
                flag = 1;
            }
            else if(isNode[1] < 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            else if(isNode[2] > 0){
                flag = 1;
                *no_of_values = temp_interpreter->no_of_values;
                return temp_interpreter->values;
            }
            isNode[0]--;
            if(flag == 1)
                break;
        }
    }
    return values;
}

Value* getConstructorValue(Node* node, Context* context, int* isNode, Node* constructor, ObjectContext* objectContext){
    Node** param_vals = (Node**)(node->left);
    int no_of_parameters = node->leftType;
    Context* function_context = construct_Context();
    function_context->parent = context;
    isNode[3]++;
    isNode[3]--;
}

Value* getFunctionCallValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    Value* tempo_vals = default_function_control(node, context, isNode, flag_type, context_needed);
    Token* token = NULL;
    ObjectContext* objectContext = NULL;
    ClassContext* classContext = NULL;
    Node* potential_method_result = NULL;
    if(tempo_vals != NULL)
        return tempo_vals;
    char* key = (char*)(node->val->val);
    char *num = NULL, *temp_key = NULL;
    int isStatic = 0;
    num = INT_TO_STR(node->leftType);
    // char* temp_key = (char*)calloc(strlen(key) + strlen(num) + 5, sizeof(char));
    temp_key = (char*)allocate_ptr_array(strlen(key) + strlen(num) + 5, sizeof(char));
    strncpy(temp_key, key, strlen(key));
    strcat(temp_key, num);
    char* prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
    if(prefix != NULL) strcpy(prefix_copy, prefix);

    if(flag_type != -1 && context_needed != NULL){
        switch(flag_type){
            case IS_INSTANCE_METHOD:{
                objectContext = (ObjectContext*)(context_needed);
                classContext = objectContext->static_context;
                potential_method_result = find_instance_method_in_ClassContext(classContext, temp_key, context, &prefix_copy);
                if(potential_method_result == NULL){
                    potential_method_result = find_static_method_in_ClassContext(classContext, temp_key, context, &prefix_copy);
                    isStatic = 1;
                }
                break;
            }
            case IS_STATIC_METHOD:{
                classContext = (ClassContext*)(context_needed);
                isStatic = 1;
                potential_method_result = find_static_method_in_ClassContext(classContext, temp_key, context, &prefix_copy);
                break;
            }
        }

        if(potential_method_result != NULL){
            isNode[3]++;
            tempo_vals = getMethodValue(
                potential_method_result, context, isNode, isStatic ? IS_STATIC_METHOD : IS_INSTANCE_METHOD,
                isStatic ? (void*)classContext : (void*)objectContext, node, prefix, prefix_copy
            );
            isNode[3]--;
            return tempo_vals;
        }
    }

    void** results = search_from_context(context, key);
    if(*((int*)results[1]) == TT_CLASS){
        classContext = (ClassContext*)(results[0]);
        Node* constructor = get_constructor_from_ClassContext(classContext, node->leftType);
        if(constructor == NULL){
            return construct_Value(
                make_error(
                    FunctionNotFoundError(
                        key, INT_TO_STR(node->leftType), node->val->line_no, node->val->col_no
                    ),
                    node->val->line_no, node->val->col_no
                )
            );
        }
        objectContext = flag_type == IS_CONSTRUCTOR_CALLED_IN_CONSTRUCTOR ? (ObjectContext*)(context_needed) : 
                        construct_ObjectContext(classContext, context, prefix);
        if(node->leftType != 0 || constructor->nodeType != NULL_NODE){
            isNode[3]++;
            if(
                strcmp(key, objectContext->static_context->className) != 0 && 
                find_index_in_superclass_from_ClassContext(objectContext->static_context, key) == -1
            ){
                key = merge_strings("class ", key);
                key = merge_strings(key, " not an immediate superclass of ");
                key = merge_strings(key, objectContext->static_context->className);
                return construct_Value(
                    make_error(
                        ClassSyntaxError(key, node->val->line_no, node->val->col_no),
                        node->val->line_no, node->val->col_no
                    )
                );
            }
            key = merge_strings(key, ".");
            key = merge_strings(prefix == NULL ? "" : prefix, key);
            tempo_vals = getMethodValue(constructor, context, isNode, IS_CONSTRUCTOR, objectContext, node, prefix, key);
            isNode[3]--;
            if(tempo_vals->valType == TT_ERROR)
                return tempo_vals;
        }
        //  do constructor stuff here
        token = allocate_ptr_for_size(sizeof(Token));
        token->type = TT_OBJECT, token->val = objectContext;
        token->line_no = node->val->line_no, token->col_no = node->val->col_no;
        return construct_Value(token);
    }
    // Value** values = (Value**)malloc(sizeof(Value*));
    Value** values = (Value**)allocate_ptr_for_size(sizeof(Value*));
    int no_of_parameters = node->leftType;
    Node** param_vals = (Node**)node->left;
    // key = (char*)(((Token*)node->val)->val);
    // num = INT_TO_STR(no_of_parameters);
    // // char* temp_key = (char*)calloc(strlen(key) + strlen(num) + 5, sizeof(char));
    // temp_key = (char*)allocate_ptr_array(strlen(key) + strlen(num) + 5, sizeof(char));
    // strncpy(temp_key, key, strlen(key));
    // strcat(temp_key, num);
    char* old_key = key;
    key = temp_key;
    results = search_from_context(context, key);
    if(*((int*)results[1]) != FUNCTION_DEFINITION_NODE)
    {
        *values = construct_Value(
            make_error(
                FunctionNotFoundError(
                    old_key, num, ((Token*)node->val)->line_no,
                    ((Token*)node->val)->col_no
                ),
                ((Token*)node->val)->line_no,
                ((Token*)node->val)->col_no
            )
        );
        isNode[2]++;
        return *values;
    }
    void* needed_context = NULL;
    int method_flag = -1;
    Node* fn_defn = (Node*)results[0];
    int block_size = fn_defn->rightType;
    Node** block = (Node**)fn_defn->right;
    Node** parameters = (Node**)fn_defn->left;
    int i;
    Value* temp_value;
    Interpreter* temp_interpreter;
    Context* function_context = construct_Context();
    function_context->parent = context;
    int temp_flag_type = flag_type == IS_CONSTRUCTOR_CALLED_IN_CONSTRUCTOR ? IS_CONSTRUCTOR : flag_type;
    for(i = 0; i < no_of_parameters; i++){
        key = (char*)(((Token*)parameters[i]->val)->val);
        temp_value = viewNode(param_vals[i], context, isNode, temp_flag_type, context_needed, NULL);
        modify_context(function_context, key, temp_value->num, temp_value->valType);
    }

    isNode[1]++;
    temp_interpreter = Interpret(block, block_size, function_context, isNode, -1, NULL, NULL);
    if(temp_interpreter->no_of_values == 1 && (isNode[1] < 0 || isNode[2] > 0)){
        if(isNode[1] < 0)
            isNode[1] *= -1;
        return *(temp_interpreter->values);
    }
    else{
        // Token* token = (Token*)malloc(sizeof(Token));
        token = (Token*)allocate_ptr_for_size(sizeof(Token));
        token->type = TT_NULL;
        char* word = {"null"};
        token->val = word, token->line_no = ((Token*)node->val)->line_no;
        token->col_no = ((Token*)node->val)->col_no;
        return construct_Value(token);
    }
    return NULL;
}

Value* getVarAssignValue(Node* node, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
	char* key = NULL;
    Node* starter = NULL;
    Node* temp;
    switch(node->leftType){
        case INDEX_NODE:{
            starter = (Node*)node->left;
            // if(node->leftType != VAR_NODE && node->leftType != INDEX_NODE)
            key = NULL;
            temp = starter;
            while(temp != NULL && temp->leftType != INDEX_NODE)
                temp = (Node*)(temp->left);
            key = (char*)(starter->val->val);
            break;
        }
        case VAR_NODE:{
            Node* key_node = (Node*)node->left;
            key = (char*)(key_node->val->val);
            break;
        }
        case OBJECT_NODE:{
            starter = (Node*)(node->left);
            temp = (Node*)starter->right;
            break;
        }
        default:{
            key = (char*)(((Token*)node->left)->val);
        }
    }
	// printNode((Node*)(node->right), 0);
    Node* nodes = (Node*)node->right;
    Interpreter* temp_interpreter = Interpret(&nodes, 1, context, isNode, flag_type, context_needed, prefix);
    int line_no = ((Token*)node->val)->line_no;
    int col_no = ((Token*)node->val)->col_no;
    if(temp_interpreter->no_of_values != 1){
        return construct_Value(
            make_error(SyntaxError("a specific value", line_no, col_no),
            line_no, col_no)
        );
    }
    else if(isNode[2] > 0)
        return *(temp_interpreter->values);
	Value* val = *(temp_interpreter->values);
	Value* val2;
	Token* token;
	void** answer = NULL;
    Value* temp_answer;
    ClassContext *classContext = NULL, *classContext2 = NULL;
    ObjectContext* objectContext = NULL;
	if(node->val->type != TT_EQ){
		line_no = node->val->line_no;
		col_no = node->val->col_no;
        switch(node->leftType){
            case INDEX_NODE: {
                temp_answer = viewNode(starter, context, isNode, flag_type, context_needed, prefix);
                // answer = (void**)calloc(2, sizeof(void*));
                answer = (void**)allocate_ptr_array(2, sizeof(void*));
                // answer[1] = (int*)malloc(sizeof(int));
                answer[1] = (int*)allocate_ptr_for_size(sizeof(int));
                *((int*)answer[1]) = temp_answer->valType;
                answer[0] = temp_answer->num;
                break;
            }
            case OBJECT_NODE:{
                temp_answer = getObjectValue(starter, context, isNode, flag_type, context_needed, prefix);
                answer = (void**)allocate_ptr_array(2, sizeof(void*));
                answer[1] = (int*)allocate_ptr_for_size(sizeof(int));
                *((int*)answer[1]) = temp_answer->valType;
                answer[0] = temp_answer->num;
                break;
            }
            default:
                answer = search_from_context(context, key);
        }
        
		if(
            answer[1] != NULL && *((int*)answer[1]) != TT_ERROR && 
            *((int*)answer[1]) != TT_ARRAY
        ){
			// token = (Token*)malloc(sizeof(Token));
            token = (Token*)allocate_ptr_for_size(sizeof(Token));
			token->type = *((int*)answer[1]);
			token->val = answer[0];
			token->line_no = node->val->line_no, token->col_no = node->val->col_no;
			val2 = construct_Value(token);
			switch(node->val->type){
				case TT_INCREMENT:
					val = operateValues(val2, val, TT_ADD, 1);
					break;
				case TT_DECREMENT:
					val = operateValues(val2, val, TT_SUB, 1);
					break;
				case TT_PRODUCT_INCREMENT:
					val = operateValues(val2, val, TT_MUL, 1);
					break;
				case TT_PRODUCT_DECREMENT:
					val = operateValues(val2, val, TT_DIV, 1);
					break;
			}

			if(val->valType == TT_ERROR)
				return val;
		}
        else if(answer[1] != NULL && *((int*)answer[1]) == TT_ARRAY){
            return construct_Value(
                make_error(
                    InvalidArrayOperationError(
                        key, (char*)(node->val->val), line_no, col_no
                    ), line_no, col_no
                )
            );
        }
		else{
			return construct_Value(
				make_error(
					ValueNotFoundError(key, line_no, col_no), line_no, col_no
				)
			);
        }
	}
	
	// printValue(val);
    int type_of_node = starter != NULL ? starter->nodeType : -1;
    int isStatic = 0, isAmbigious = 0;
    switch(type_of_node){
        case INDEX_NODE:{
            Node* array = (Node*)starter->left;
            Value* array_value = viewNode(array, context, isNode, flag_type, context_needed, prefix);
            if(array_value->valType != TT_ARRAY){
                return construct_Value(
                    make_error(
                        InvalidArrayOperationError(
                            key, (char*)(node->val->val), line_no, col_no
                        ), line_no, col_no
                    )
                );
            }
            int count = starter->rightType;
            Node** indices = (Node**)(starter->right);
            Value *temp_key;
            ArrayValue* arrayValue = (ArrayValue*)(array_value->num);
            int i;
            if(count <= 1){
                temp_key = viewNode(*indices, context, isNode, flag_type, context_needed, prefix);
                if(temp_key->valType == TT_ERROR)
                    return temp_key;
                modify_ArrayValue(arrayValue, temp_key, val);
            }
            else{
                if(val->valType != TT_ARRAY){
                    return construct_Value(
                        make_error(
                            MisMatchError(key, line_no, col_no), line_no, col_no
                        )
                    );
                }

                ArrayValue* array_vals = (ArrayValue*)(val->num);
                int vals_count = array_vals->curr_size;
                if(count != vals_count){
                    return construct_Value(
                        make_error(
                            MisMatchError(key, line_no, col_no), line_no, col_no
                        )
                    );
                }
                // Token* token = (Token*)malloc(sizeof(Token));
                Token* token = (Token*)allocate_ptr_for_size(sizeof(Token));
                token->type = TT_INT;
                token->line_no = val->line_no, token->col_no = val->col_no;
                // token->val = malloc(sizeof(int));
                token->val = allocate_ptr_for_size(sizeof(int));
                temp_key = construct_Value(token);
                Value* temp_val;
                for(i = 0; i < count; i++){
                    *((int*)temp_key->num) = i;
                    temp_val = find_in_ArrayValue(array_vals, temp_key);
                    if(temp_val->valType == TT_ERROR)
                        return temp_val;
                    
                    temp_key = viewNode(indices[i], context, isNode, flag_type, context_needed, prefix);
                    if(temp_key->valType == TT_ERROR)
                        return temp_key;
                    
                    modify_ArrayValue(arrayValue, temp_key, temp_val);
                }
            }
            break;
        }
        case OBJECT_NODE:{
            temp = (Node*)(starter->left);
            char* prefix_copy = prefix == NULL ? NULL : allocate_ptr_array(strlen(prefix) + 5, sizeof(char));
            if(prefix != NULL)
                strcpy(prefix_copy, prefix);
            switch(temp->nodeType){
                case THIS_NODE:{
                    if(isNode[3] > 0 && (flag_type == IS_INSTANCE_METHOD || flag_type == IS_CONSTRUCTOR) && context_needed != NULL){
                        objectContext = (ObjectContext*)(context_needed);
                        classContext = objectContext->static_context;
                    }
                    else{
                        return construct_Value(
                            make_error(
                                ClassSyntaxError(
                                    "Cannot use 'this' keyword outside instance method or constructor\n",
                                    temp->val->line_no, temp->val->col_no
                                ), temp->val->line_no, temp->val->col_no
                            )
                        );
                    }
                    break;
                }
                case VAR_NODE:{
                    key = (char*)(temp->val->val);
                    answer = search_from_context(context, key);
                    isStatic = 0;
                    int valType_answer = -1;
                    if(*((int*)answer[1]) != TT_ERROR){
                        valType_answer = *((int*)answer[1]);
                        switch(valType_answer){
                            case TT_OBJECT:{
                                temp_answer = (Value*)(answer[0]);
                                objectContext = (ObjectContext*)(temp_answer->num);
                                classContext = objectContext->static_context;
                                key = NULL;
                                break;
                            }
                            case TT_CLASS:{
                                if(isNode[3] > 0 && (flag_type == IS_INSTANCE_METHOD || flag_type == IS_CONSTRUCTOR) 
                                    && context_needed != NULL){
                                    objectContext = (ObjectContext*)(context_needed);
                                    classContext = objectContext->static_context;
                                    if(find_superclass_in_ClassContext(classContext, key) != NULL){
                                        strcat(key, ".");
                                        classContext2 = (ClassContext*)answer[0];
                                        isAmbigious = 1;
                                    }
                                    else{
                                        isStatic = 1;
                                        objectContext = NULL;
                                        classContext = (ClassContext*)(answer[0]);
                                        key = NULL;
                                    }
                                }
                                else{
                                    isStatic = 1;
                                    objectContext = NULL;
                                    classContext = (ClassContext*)(answer[0]);
                                    key = NULL;
                                }
                                break;
                            }
                            default:{
                                return construct_Value(
                                    make_error(
                                        SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                        temp->val->line_no, temp->val->col_no
                                    )
                                );
                            }
                        }
                    }
                    else{
                        if(isNode[3] > 0 && context_needed != NULL){
                            switch(flag_type){
                                case IS_INSTANCE_METHOD:{
                                    objectContext = (ObjectContext*)(context_needed);
                                    classContext = objectContext->static_context;
                                    temp_answer = (Value*)find_from_ObjectContext(objectContext, key, VAR_NODE, context, &prefix_copy);
                                    break;
                                }
                                case IS_STATIC_METHOD:{
                                    objectContext = NULL;
                                    classContext = (ClassContext*)(context_needed);
                                    temp_answer = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                                    break;
                                }
                                default:{
                                    return construct_Value(
                                        make_error(
                                            ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                                            temp->val->line_no, temp->val->col_no
                                        )
                                    );
                                }
                            }
                            key = NULL;
                        }
                        else{
                            return construct_Value(
                                make_error(
                                    ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                    }
                    break;
                }
                case INDEX_NODE:{
                    temp_answer = getIndexValue(temp, context, isNode, flag_type, context_needed, prefix);
                    if(temp_answer == NULL || temp_answer->valType != TT_OBJECT){
                        return temp_answer != NULL && temp_answer->valType == TT_ERROR ? temp_answer : 
                            construct_Value(
                                make_error(
                                    SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                    }
                    objectContext = (ObjectContext*)(temp_answer->num);
                    classContext = (ClassContext*)(objectContext->static_context);
                    key = NULL;
                    break;
                }
                default:{
                    return construct_Value(
                        make_error(
                            SyntaxError("varName", temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
                }
            }
            temp = (Node*)(starter->right);
            Node *fp = NULL, *temp_node = NULL, *right = NULL;
            char *name, *num_params, *temp_name;
            while(temp != NULL && temp->nodeType == OBJECT_NODE){
                fp = temp;
                temp = (Node*)(temp->left);
                switch(temp->nodeType){
                    case VAR_NODE:{
                        name = (char*)(temp->val->val);
                        key = key == NULL ? name : merge_strings(key, name);
                        answer = search_from_context(context, name);
                        if(*((int*)answer[1]) == TT_CLASS)
                            strcat(key, ".");
                        else{
                            temp_answer = isStatic ? 
                            find_static_var_in_ClassContext(classContext, key, context, &prefix_copy) : 
                            find_instance_var_from_ObjectContext(objectContext, key, prefix_copy, context);
                            if(!isStatic && temp_answer == NULL)
                                temp_answer = find_static_var_in_ClassContext(classContext, key, context, &prefix_copy);
                            

                            if(temp_answer == NULL){
                                if(isAmbigious){
                                    temp_answer = find_static_var_in_ClassContext(classContext2, key, context, &prefix_copy);
                                    isAmbigious = 0;
                                }

                                if(temp_answer != NULL){
                                    classContext = classContext2;
                                    isStatic = 1;
                                }
                                else{
                                    return construct_Value(
                                        make_error(
                                            ValueNotFoundError(key, temp->val->line_no, temp->val->col_no),
                                            temp->val->line_no, temp->val->col_no
                                        )
                                    );
                                }
                            }
                            else if(temp_answer->valType == TT_OBJECT){
                                objectContext = (ObjectContext*)(temp_answer->num);
                                classContext = objectContext->static_context;
                            }
                            else{
                                return construct_Value(
                                    make_error(
                                        SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                        temp->val->line_no, temp->val->col_no
                                    )
                                );
                            }

                            key = NULL;
                            isAmbigious = 0;
                        }
                        break;
                    }
                    case INDEX_NODE:{
                        temp_answer = extract_IndexNode_from_Object(
                                        temp, context, isNode, 
                                        isStatic ? IS_STATIC_METHOD : IS_INSTANCE_METHOD, 
                                        isStatic ? (void*)classContext : (void*)objectContext,
                                        key, &prefix_copy
                                    );
                        if(temp_answer == NULL || temp_answer->valType != TT_OBJECT){
                            return construct_Value(
                                make_error(
                                    SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                        objectContext = (ObjectContext*)(temp_answer->num);
                        classContext = objectContext->static_context;
                        key = NULL;
                        break;
                    }
                    case FUNCTION_CALL_NODE:{
                        name = (char*)(temp->val->val);
                        num_params = INT_TO_STR(temp->leftType);
                        temp_name = (char*)allocate_ptr_array(strlen(name) + strlen(num_params) + 5, sizeof(char));
                        strncpy(temp_name, name, strlen(name));
                        temp_name[strlen(name)] = '\0';
                        strcat(temp_name, num_params);
                        name = temp_name;
                        key = key == NULL ? merge_strings("", name) : merge_strings(key, name);
                        temp_node = isStatic ? find_static_method_in_ClassContext(classContext, key, context, &prefix_copy) : 
                                    (Node*)find_from_ObjectContext(objectContext, key, FUNCTION_CALL_NODE, context, &prefix_copy);
                        

                        if(temp_node == NULL){
                            if(isAmbigious){
                                temp_node = find_static_method_in_ClassContext(classContext2, key, context, &prefix_copy);
                                isAmbigious = 0;
                            }

                            if(temp_node != NULL){
                                classContext = classContext2;
                                isStatic = 1;
                            }
                            else{
                                return construct_Value(
                                    make_error(
                                        MethodNotFoundError(
                                            (char*)(temp->val->val), num_params, temp->val->line_no, temp->val->col_no
                                        ), temp->val->line_no, temp->val->col_no
                                    )
                                );
                            }
                        }

                        isStatic = isStatic ? isStatic : 
                                    find_instance_method_in_ClassContext(classContext, key, context, &prefix_copy) == NULL ? 1 : 
                                    isStatic;

                        temp_answer = getMethodValue(
                                        temp_node, context, isNode, isStatic ? IS_STATIC_METHOD : IS_INSTANCE_METHOD,
                                        isStatic ? (void*)classContext : (void*)objectContext, temp, prefix, prefix_copy
                                    );
                        
                        if(temp_answer == NULL || temp_answer->valType == TT_ERROR)
                            return temp_answer;
                        else if(temp_answer->valType == TT_OBJECT){
                            objectContext = (ObjectContext*)(temp_answer->num);
                            classContext = objectContext->static_context;
                            key = NULL;
                        }
                        else{
                            return construct_Value(
                                make_error(
                                    SyntaxError("variable of type 'Object'", temp->val->line_no, temp->val->col_no),
                                    temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                        break;
                    }
                }
                temp = (Node*)(fp->right);
            }

            if(temp == NULL)
                return NULL;
            else{
                if(temp->nodeType == VAR_NODE){
                    name = (char*)(temp->val->val);
                    name = key == NULL ? name : merge_strings(key, name);
                    Value* valuex = NULL;
                    if(isStatic){
                        valuex = find_static_var_in_ClassContext(classContext, name, context, &prefix_copy);
                        if(valuex == NULL){
                            return construct_Value(
                                make_error(
                                    StaticVarNotFoundError(
                                        name, classContext->className, temp->val->line_no, temp->val->col_no
                                    ), temp->val->line_no, temp->val->col_no
                                )
                            );
                        }
                        modify_static_var_in_ClassContext(classContext, name, val);
                    }
                    else{
                        if(flag_type != IS_CONSTRUCTOR){
                            valuex = find_instance_var_from_ObjectContext(objectContext, name, prefix_copy, context);
                            if(valuex == NULL)
                                valuex = find_static_var_in_ClassContext(objectContext->static_context, name, context, &prefix_copy);
                            
                            if(valuex == NULL){
                                return construct_Value(
                                    make_error(
                                        InstanceVarNotFoundError(
                                            name, objectContext->static_context->className, 
                                            temp->val->line_no, temp->val->col_no
                                        ), temp->val->line_no, temp->val->col_no
                                    )
                                );
                            }
                        }
                        modify_instance_var_from_ObjectContext(objectContext, name, val);
                    }
                }
                else{
                    return construct_Value(
                        make_error(
                            SyntaxError("varName", temp->val->line_no, temp->val->col_no),
                            temp->val->line_no, temp->val->col_no
                        )
                    );
                }
            }
            break;
        }
        default:{
            modify_context(context, key, val->num, val->valType);
        }
    }
	if(node->isVarNode != 0){
		return val;
    }
	// token = (Token*)malloc(sizeof(Token));
    token = (Token*)allocate_ptr_for_size(sizeof(Token));
	// memcpy(token, node->val, sizeof(Token));
    copy_heap_alloced_memory(token, node->val, sizeof(Token));
	token->type = TT_NULL;
	// token->val = (char*)calloc(5, sizeof(char));
    token->val = (char*)allocate_ptr_array(5, sizeof(char));
	strcpy((char*)(token->val), "null");
	return construct_Value(token);
}

Interpreter* Interpret(Node** nodes, int no_of_nodes, Context* context, int* isNode, int flag_type, void* context_needed, char* prefix){
    Value **values, **temp_values;
    int no_of_values = 1, total_no_of_values = 0;
    int size = DEFAULT_NO_OF_VALUES;
    int i = 0;
    // values = (Value**)calloc(size, sizeof(Value*));
    values = (Value**)allocate_ptr_array(size, sizeof(Value*));
    int breaker = 0;
    for(i = 0; i < no_of_nodes; i++){
        no_of_values = 1;
        switch(nodes[i]->nodeType){
            case CONDITIONAL_NODE:{
                temp_values = getConditionalNodeValue(nodes[i], &no_of_values, context, isNode, flag_type, context_needed, prefix);
                break;
            }
            case LOOP_NODE:{
                temp_values = getLoopNodeValue(nodes[i], &no_of_values, context, isNode, flag_type, context_needed, prefix);
                break;
            }
            case BREAK_NODE:{
                breaker++;
                Token* token = NULL;
                if(isNode[0] == 0){
                    no_of_values = 1;
                    token = make_error(
                        SyntaxError(
                            "statement instead of break in a non loop",
                            nodes[i]->val->line_no, nodes[i]->val->col_no
                        ), nodes[i]->val->line_no, nodes[i]->val->col_no
                    );
                    isNode[2]++;
                }
                else{
                    isNode[0] *= -1;
                    // token = (Token*)malloc(sizeof(Token));
                    token = (Token*)allocate_ptr_for_size(sizeof(Token));
                    token->type = TT_NULL, token->line_no = nodes[i]->val->line_no;
                    token->col_no = nodes[i]->val->col_no;
                    // token->val = (char*)calloc(5, sizeof(char));
                    token->val = (char*)allocate_ptr_array(5, sizeof(char));
                    strcpy((char*)(token->val), "null");
                }
                no_of_values = 1;
                // temp_values = (Value**)malloc(sizeof(Value*));
                temp_values = (Value**)allocate_ptr_for_size(sizeof(Value*));
                *temp_values = construct_Value(token);
                break;
            }
            case RETURN_NODE:{
                breaker++;
                Token* token = NULL;
                // temp_values = (Value**)malloc(sizeof(Value*));
                temp_values = (Value**)allocate_ptr_for_size(sizeof(Value*));
                if(isNode[1] == 0){
                    no_of_values = 1;
                    token = make_error(
                        SyntaxError(
                            "statement instead of return outside of any function",
                            nodes[i]->val->line_no, nodes[i]->val->col_no
                        ), nodes[i]->val->line_no, nodes[i]->val->col_no
                    );
                    no_of_values = 1;
                    *temp_values = construct_Value(token);
                    isNode[2]++;
                }
                else{
                    no_of_values = 1;
                    isNode[1] *= -1;
                    *temp_values = viewNode((Node*)(nodes[i]->right), context, isNode, flag_type, context_needed, prefix);
                }
                break;
            }
            default:{
                no_of_values = 1;
                // temp_values = (Value**)malloc(sizeof(Value*));
                temp_values = (Value**)allocate_ptr_for_size(sizeof(Value*));
                // printf("Hamon\n");
                *temp_values = viewNode(nodes[i], context, isNode, flag_type, context_needed, prefix);
            }
        }

        if(no_of_values == 1 && (*temp_values)->valType == TT_ERROR){
            isNode[2]++;
        }

        if(total_no_of_values + no_of_values >= size)
            expand_Values(&values, &size, total_no_of_values + no_of_values);
        // memmove(values + total_no_of_values, temp_values, no_of_values * sizeof(Value*));
        if(isNode[1] < 0 || isNode[2] > 0){
            total_no_of_values = 1;
            *values = *temp_values;
        }
        else{
            move_heap_alloced_memory(values + total_no_of_values, temp_values, no_of_values * sizeof(Value*));
            total_no_of_values += no_of_values;
        }

        if(breaker > 0 || isNode[2] > 0)
            break;
    }
    return construct_Interpreter(values, total_no_of_values, breaker);
}

void printValues(Interpreter* interpreter){
    Value** values = interpreter->values;
    int no_of_values = interpreter->no_of_values;
    for(int i = 0; i < no_of_values; i++){
        printValue(values[i]);
        printf("\n");
    }
}