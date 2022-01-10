#include "mem_lib.h"
#include <stdio.h>

int main(){
    start_Dynamic_Mem();
    void* pt1 = allocate_ptr_for_size(sizeof(int));
    view_heap_alloced_hashmap_status();
    void* pt2 = allocate_ptr_for_size(50004);
    view_heap_alloced_hashmap_status();
    void* pt3 = allocate_ptr_for_size(sizeof(int));
    view_heap_alloced_hashmap_status();
    void* pt4 = allocate_ptr_for_size(40000);
    view_heap_alloced_hashmap_status();
    void* pt5 = allocate_ptr_for_size(20000);
    view_heap_alloced_hashmap_status();
    // view_heap_pointer_status();
    // free_pointer(pt1);
    // free_pointer(pt2);
    // free_pointer(pt3);
    // free_pointer(pt4);
    // free_pointer(pt5);
    // view_heap_pointer_status();
    // free_pointer(pt1);
    view_heap_pointer_status();
    expand_heap_allocated_heap_list(0);
    expand_heap_allocated_heap_list(1);
    view_heap_pointer_status();
    // modify_heap_alloced_hashmap_with_key(pt1, pt2, 0);
    // modify_heap_alloced_hashmap_with_key(pt3, pt4, 0);
    expand_heap_alloced_hashmap();
    HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE h1, h2;
    h1 = find_from_heap_alloced_hashmap(pt1);
    h2 = find_from_heap_alloced_hashmap(pt3);
    view_heap_pointer_status();
    view_heap_alloced_hashmap_status();
    printf("This?? %p\n", pt1);
    pt1 = reallocate_heap_alloced_ptr(pt1, 5 * sizeof(int));
    printf("Outcome %d %d\n", h1.ptr == pt2, h2.ptr == pt4);
    printf("Outcome chars: %c %c\n", h1.type, h2.type);
    delete_key_from_heap_alloced_hashmap(pt1);
    delete_key_from_heap_alloced_hashmap(pt3);

    int x[100];
    set_heap_alloced_memory(x, -1, 100 * sizeof(int));
    for(int i = 0; i < 100; i++)
        printf("%d ", x[i]);
    printf("\n");
    return 0;
}