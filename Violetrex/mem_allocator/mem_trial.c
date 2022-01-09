#include "mem_alloc.h"
#include <stdio.h>

int main(){
    start_Dynamic_Mem();
    void* pt1 = allocate_ptr_for_size(sizeof(int));
    void* pt2 = allocate_ptr_for_size(50004);
    void* pt3 = allocate_ptr_for_size(sizeof(int));
    void* pt4 = allocate_ptr_for_size(40000);
    void* pt5 = allocate_ptr_for_size(20000);
    view_heap_pointer_status();
    free_pointer(pt4);
    view_heap_pointer_status();
    expand_heap_allocated_heap_list(0);
    expand_heap_allocated_heap_list(1);
    view_heap_pointer_status();
    // printf("Size: %d", new_sizeof(char));
    return 0;
}