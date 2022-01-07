#ifdef __linux
#include <sys/mman.h>
#elif _WIN32
#include <memoryapi.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#define HEAP_ALLOC_BLOCK 50000
#define LIST_ALLOC_BLOCK 5000

#define ALLOC_SIZE_ADJUST(x) (x % 8 == 0 ? x : x + 8 - (x % 8))

typedef struct HEAP_BLOCK{
    void* addr;
    size_t size;
    struct HEAP_BLOCK* next;
}heap_block;

void* mem_alloc(size_t size){
    void* new_p;
    size_t x_size = ALLOC_SIZE_ADJUST(size);
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

void create_heap_list(void* ptr, size_t* rem_size_ptr, int flag){
    heap_block* head = (heap_block*)ptr;
    head->addr = flag ? mem_alloc(HEAP_ALLOC_BLOCK) : NULL;
    head->size = flag ? HEAP_ALLOC_BLOCK : 0;
    head->next = NULL;
    *rem_size_ptr -= sizeof(heap_block);
}

void start_Dynamic_Mem(){
    heap_free_pointer_list = HEAP_LIST(0);
    heap_alloced_pointer_list = HEAP_LIST(0);
    create_heap_list(heap_free_pointer_list, &LIST_FREE_BLOCK_SIZE_REMAINING, 1);
    create_heap_list(heap_alloced_pointer_list, &LIST_ALLOCED_BLOCK_SIZE_REMAINING, 0);
}

void add_to_alloced_list_by_node(heap_block* node){
    heap_block* head = (heap_block*)heap_alloced_pointer_list;
    heap_block *temp = head, *fp = NULL;
    int result = 0;
    while(temp != NULL && node->addr > temp->addr){
        fp = temp;
        temp = temp->next;
    }
    if(fp == NULL){
        void* temp_ptr = head->addr;
        size_t temp_size = head->size;
        head->addr = node->addr;
        head->size = node->size;
        node->next = head->next;
        head->next = node;
        node->addr = temp_ptr;
        node->size = temp_size;
        return;
    }
    fp->next = node;
    node->next = temp;
}

void add_to_alloced_list_by_pointer(void* ptr, size_t size){
    if(LIST_ALLOCED_BLOCK_SIZE_REMAINING < sizeof(heap_block)){}
    heap_block* new_ptr = (heap_block*)(heap_alloced_pointer_list + HEAP_ALLOCED_POINTER_LIST_SIZE - LIST_ALLOCED_BLOCK_SIZE_REMAINING);
    LIST_ALLOCED_BLOCK_SIZE_REMAINING -= sizeof(heap_block);
    new_ptr->addr = ptr;
    new_ptr->size = size;
    new_ptr->next = NULL;
    add_to_alloced_list_by_node(new_ptr);
}

void* allocate_ptr_for_size(size_t size){
    heap_block* head = (heap_block*)heap_free_pointer_list;
    heap_block *temp = head, *fp = NULL;
    while(temp != NULL){
        if(temp->size >= size)
            break;
        else{
            fp = temp;
            temp = temp->next;
        }
    }
    void* answer = NULL;
    if(temp != NULL){
        answer = temp->size == size ? temp->addr : temp->addr + temp->size - ALLOC_SIZE_ADJUST(size);
        if(temp->size == size){
            fp->next = temp->next;
            temp->next = NULL;
            add_to_alloced_list_by_node(temp);
        }
        else{
            temp->size -= ALLOC_SIZE_ADJUST(size);
            add_to_alloced_list_by_pointer(answer, ALLOC_SIZE_ADJUST(size));
        }
        return answer;
    }
    else{
        if(LIST_FREE_BLOCK_SIZE_REMAINING < sizeof(heap_block)){}

        size_t new_block_size = HEAP_ALLOC_BLOCK;
        while(size >= new_block_size)
            new_block_size *= 2;
        
        void* new_block_addr = mem_alloc(new_block_size);
        if(new_block_addr == NULL){
            printf("Heap Overflow: Space of size %d bytes not available\n", new_block_size);
            exit(0);
        }
        temp = (heap_block*)heap_free_pointer_list;
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = heap_free_pointer_list + HEAP_FREE_POINTER_LIST_SIZE - LIST_FREE_BLOCK_SIZE_REMAINING;
        temp = (heap_block*)(temp->next);
        temp->addr = new_block_addr;
        temp->size = new_block_size - ALLOC_SIZE_ADJUST(size);
        temp->next = NULL;
        LIST_FREE_BLOCK_SIZE_REMAINING -= sizeof(heap_block);

        answer = temp->addr + temp->size;
        return answer;
    }
}