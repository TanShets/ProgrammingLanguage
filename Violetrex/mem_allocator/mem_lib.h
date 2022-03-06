#pragma once
#include "mem_alloc.h"
#include "mem_hasher.h"

typedef struct STRING_STACK{
    char *val, *prefix;
    struct STRING_STACK *next;
} StringStack;

void push_StringStack(StringStack** top, char* val, char* prefix){
    if(top == NULL) return;
    StringStack* node = NULL;
    node = (StringStack*)allocate_ptr_for_size(sizeof(StringStack));
    node->val = val;
    node->prefix = prefix;
    node->next = *top;
    *top = node;
}

char* pop_StringStack(StringStack** top){
    if(top == NULL || *top == NULL) return NULL;

    char* answer = (*top)->val;
    *top = (*top)->next;
    return answer;
}

char* peek_StringStack(StringStack* top){
    return top == NULL ? NULL : top->val;
}

int detect_loop_in_heap_list(heap_block* head){
    heap_block *fp = head, *sp = head;
    do{
        sp = sp->next;
        fp = fp->next == NULL ? fp->next : fp->next->next;
    }while(fp != NULL && fp != sp);

    return fp != NULL ? 1 : 0;
}

void check_byte_content_of_pointer(void* ptr, size_t size){
    char* char_ptr = (char*)ptr;
    printf("Contents of %p: ", ptr);
    for(size_t i = 0; i < size; i++)
        printf("%c->", char_ptr[i]);
    printf("\n");
}