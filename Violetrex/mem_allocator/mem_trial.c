#include "mem_lib.h"
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
    insert_into_heap_alloced_hashmap(pt1, pt2);
    insert_into_heap_alloced_hashmap(pt3, pt4);
    expand_heap_alloced_hashmap();
    void* ptx = find_from_heap_alloced_hashmap(pt1);
    void* pty = find_from_heap_alloced_hashmap(pt3);
    printf("Outcome %d %d\n", ptx == pt2, pty == pt4);
    delete_key_from_heap_alloced_hashmap(pt1);
    delete_key_from_heap_alloced_hashmap(pt3);
    return 0;
}