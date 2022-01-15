#pragma once
#include "mem_alloc.h"
#include "mem_hasher.h"

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