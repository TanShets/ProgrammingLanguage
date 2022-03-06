#pragma once
#include "../value.h"

typedef struct OOP_INHERITANCE_LINKED_LIST{
    void* ptr;
    struct OOP_INHERITANCE_LINKED_LIST *child;
    struct OOP_INHERITANCE_LINKED_LIST *parent;
    int isObject;
} OopInheritanceLinkedList;

OopInheritanceLinkedList* construct_OopInheritanceLinkedList(void* ptr, int isObject){
    OopInheritanceLinkedList* head = allocate_ptr_for_size(sizeof(OopInheritanceLinkedList));
    head->ptr = ptr;
    head->isObject = isObject;
    head->child = NULL, head->parent = NULL;
    return head;
}

void add_super_to_OopInheritanceLinkedList(OopInheritanceLinkedList** head_ptr, void* ptr, int isObject){
    OopInheritanceLinkedList* node = construct_OopInheritanceLinkedList(ptr, isObject);
    if(*head_ptr != NULL)
        (*head_ptr)->parent = node;
    node->child = *head_ptr;
    *head_ptr = node;
}