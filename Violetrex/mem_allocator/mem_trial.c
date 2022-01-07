#include "mem_alloc.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    start_Dynamic_Mem();
    void* xx = allocate_ptr_for_size(5 * sizeof(int));
    int df = 0;
    printf("Pointer assigned: %p %d %p %p\n", xx, df, heap_free_pointer_list, heap_alloced_pointer_list);
    return 0;
}