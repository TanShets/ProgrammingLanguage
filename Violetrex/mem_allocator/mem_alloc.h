#ifdef __linux
#include <sys/mman.h>
#elif _WIN32
#include <memoryapi.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#define HEAP_ALLOC_BLOCK 50000
#define LIST_ALLOC_BLOCK 5000

typedef struct HEAP_BLOCK{
    void* addr;
    size_t size;
    struct HEAP_BLOCK* next;
}heap_block;

void* mem_alloc(size_t size){
    void* new_p;
    size_t x_size = size % 8 == 0 ? size : size + 8 - (size % 8);
    printf("Size assigned: %d %d\n", x_size, size);
#ifdef __linux
    new_p = mmap(
        NULL, x_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
        -1, 0);
#elif _WIN32
    new_p = VirtualAlloc(NULL, x_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    new_p = NULL;
    printf("Right here\n");
#endif
    return new_p;
}

#define HEAP_LIST(x) (x == 0 ? mem_alloc(LIST_ALLOC_BLOCK) : mem_alloc(HEAP_ALLOC_BLOCK))
void* heap_free_pointer_list = NULL;
void* heap_alloced_pointer_list = NULL;

size_t HEAP_FREE_POINTER_LIST_SIZE = LIST_ALLOC_BLOCK;
size_t HEAP_ALLOCED_POINTER_LIST_SIZE = LIST_ALLOC_BLOCK;

size_t LIST_FREE_BLOCK_SIZE_REMAINING = LIST_ALLOC_BLOCK;
size_t LIST_ALLOCED_BLOCK_SIZE_REMAINING = LIST_ALLOC_BLOCK;

void create_heap_list(void* ptr, size_t* rem_size_ptr){
    heap_block* head = (heap_block*)ptr;
    head->addr = mem_alloc(HEAP_ALLOC_BLOCK);
    head->size = HEAP_ALLOC_BLOCK;
    head->next = NULL;
    *rem_size_ptr -= sizeof(heap_block);
}

void start_Dynamic_Mem(){
    heap_free_pointer_list = HEAP_LIST(0);
    heap_alloced_pointer_list = HEAP_LIST(0);
    create_heap_list(heap_free_pointer_list, LIST_FREE_BLOCK_SIZE_REMAINING);
}