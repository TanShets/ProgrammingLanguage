#pragma once
#ifdef __linux
#include <sys/mman.h>
#elif _WIN32
#include <memoryapi.h>
#endif

#include <stdio.h>

#define HEAP_ALLOC_BLOCK 50000
#define LIST_ALLOC_BLOCK 5000

#define HEAP_LIST_TRAVERSAL_COUNTER 80000

#define ALLOC_SIZE_ADJUST(x) (x % 8 == 0 ? x : x + 8 - (x % 8))

#if (_WIN64 || __x86_64__)
#define HEAP_ALLOCED_HASHMAP_INDEX_TYPE long long unsigned int
#define __HEAP_ALLOCED_HASHMAP_INDEX_TYPE__FORMAT__ "%llu"
#else
#define HEAP_ALLOCED_HASHMAP_INDEX_TYPE long unsigned int
#define __HEAP_ALLOCED_HASHMAP_INDEX_TYPE__FORMAT__ "%lu"
#endif

#define HEAP_PTR_INT(ptr) ((HEAP_ALLOCED_HASHMAP_INDEX_TYPE)((HEAP_ALLOCED_HASHMAP_INDEX_TYPE*)ptr))
#define HEAP_PTR_GREATER(ptr1, ptr2) (HEAP_PTR_INT(ptr1) > HEAP_PTR_INT(ptr2))
#define HEAP_PTR_GREATER_EQ(ptr1, ptr2) (HEAP_PTR_INT(ptr1) >= HEAP_PTR_INT(ptr2))
#define HEAP_PTR_LESSER(ptr1, ptr2) (HEAP_PTR_INT(ptr1) < HEAP_PTR_INT(ptr2))
#define HEAP_PTR_LESSER_EQ(ptr1, ptr2) (HEAP_PTR_INT(ptr1) <= HEAP_PTR_INT(ptr2))

#ifndef _INC_STDLIB
extern void exit(int __status) __THROW __attribute__ ((__noreturn__));
#endif

typedef struct HEAP_BLOCK{
    void* addr;
    size_t size;
    struct HEAP_BLOCK* next;
}heap_block;

typedef struct {
    void* ptr;
    char type;
} HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE;

void initialize_heap_alloc_hashmap();
void insert_into_heap_alloced_hashmap(void* key, void* value, int isHead);
void modify_heap_alloced_hashmap_with_key(void* key, void* new_value, int isHead);
void delete_key_from_heap_alloced_hashmap(void* key);
HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE find_from_heap_alloced_hashmap(void* key);
int detect_loop_in_heap_list(heap_block* head);
void view_heap_pointer_status();
void add_to_free_list_by_pointer(void* ptr, size_t size);

void* mem_alloc(size_t size){
    void* new_p;
    size_t x_size = ALLOC_SIZE_ADJUST(size);
    // printf("Size assigned: %d %d\n", x_size, size);
#ifdef __linux
    new_p = mmap(
        NULL, x_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
        -1, 0);
#elif _WIN32
    new_p = VirtualAlloc(NULL, x_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    new_p = NULL;
    // printf("Right here\n");
#endif
    return new_p;
}

void mem_free_heap_allocated_pointer(void* ptr, size_t size){
    int num;
    // printf("Freeing %p\n", ptr);
#ifdef __linux
    num = munmap(ptr, size) == 0 ? 1 : 0;
#elif _WIN32
    num = (int)VirtualFree(ptr, 0, MEM_RELEASE);
#else
    num = 0;
#endif
    char* statement = num ? "Pointer successfully freed\n" : "Failure in freeing pointer\n";
    // printf("%s", statement);
}

#define HEAP_LIST(x) (x == 0 ? mem_alloc(LIST_ALLOC_BLOCK) : mem_alloc(HEAP_ALLOC_BLOCK))
void* heap_free_pointer_list = NULL;
void* heap_alloced_pointer_list = NULL;
heap_block* garbage_heap_alloced_pointer_list = NULL;

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

void add_to_garbage_heap_alloced_list(heap_block* node){
    node->addr = NULL, node->size = 0, node->next = garbage_heap_alloced_pointer_list;
    garbage_heap_alloced_pointer_list = node;
}

void start_Dynamic_Mem(){
    heap_free_pointer_list = HEAP_LIST(0);
    heap_alloced_pointer_list = HEAP_LIST(0);
    initialize_heap_alloc_hashmap();
    create_heap_list(heap_free_pointer_list, &LIST_FREE_BLOCK_SIZE_REMAINING, 1);
    create_heap_list(heap_alloced_pointer_list, &LIST_ALLOCED_BLOCK_SIZE_REMAINING, 0);
    // printf("%p %p %d\n", heap_free_pointer_list, heap_alloced_pointer_list, 
    //                     (char*)heap_free_pointer_list - (char*)heap_alloced_pointer_list);
    // view_heap_pointer_status();
}

void expand_heap_allocated_heap_list(int flag){
    size_t size, occupied_size, new_size, new_remaining;
    void* old_ptr;
    size_t old_size;
    void* ptr;
    heap_block *head, *temp, *fp;
    heap_block *new_head, *new_temp;
    heap_block* heads[3] = {heap_free_pointer_list, heap_alloced_pointer_list, garbage_heap_alloced_pointer_list};
    head = flag ? (heap_block*)heap_free_pointer_list : (heap_block*)heap_alloced_pointer_list;
    size = flag ? HEAP_FREE_POINTER_LIST_SIZE : HEAP_ALLOCED_POINTER_LIST_SIZE;
    occupied_size = size - (flag ? LIST_FREE_BLOCK_SIZE_REMAINING : LIST_ALLOCED_BLOCK_SIZE_REMAINING);
    ptr = mem_alloc(size * 2);
    if(ptr == NULL){
        printf("Heap Overflow: Space of size %d bytes not available\n", size * 2);
        exit(0);
    }
    new_size = ALLOC_SIZE_ADJUST(size * 2);
    new_remaining = new_size;
    new_head = (heap_block*)ptr;
    int count = 0;
    heap_block block_temp;
    for(int i = 0; i < 3; i++){
        temp = heads[i];
        fp = NULL;
        while(temp != NULL){
            if((char*)temp - (char*)head < occupied_size && HEAP_PTR_GREATER_EQ(temp, head)){ //temp >= head){
                new_temp = ((heap_block*)new_head) + count;
                // block_temp = *new_temp;
                new_remaining -= sizeof(heap_block);
                count++;
                // new_temp->size = 0;
                new_temp->addr = temp->addr;
                new_temp->size = temp->size;
                if(fp != NULL)
                    fp->next = new_temp;
                if(!flag && i == 1){
                    modify_heap_alloced_hashmap_with_key(
                        new_temp->addr, new_temp == new_head ? new_head : fp, new_temp == new_head
                    );
                }
                new_temp->next = temp->next;
                if(temp == garbage_heap_alloced_pointer_list)
                    garbage_heap_alloced_pointer_list = new_temp;
                temp = new_temp;
            }
            fp = temp;
            temp = temp->next;
        }
    }

    if(flag){
        heap_free_pointer_list = ptr;
        HEAP_FREE_POINTER_LIST_SIZE = new_size;
        LIST_FREE_BLOCK_SIZE_REMAINING = new_remaining;
    }
    else{
        heap_alloced_pointer_list = ptr;
        HEAP_ALLOCED_POINTER_LIST_SIZE = new_size;
        LIST_ALLOCED_BLOCK_SIZE_REMAINING = new_remaining;
    }
    // FILE* file = fopen("logs.txt", "a");
    // fprintf(file, "Heap expansion:\n");
    // fclose(file);
    // view_heap_pointer_status();
    mem_free_heap_allocated_pointer(head, size);
}

void add_to_alloced_list_by_node(heap_block* node){
    heap_block* head = (heap_block*)heap_alloced_pointer_list;
    if(head->addr == NULL){
        head->addr = node->addr;
        head->size = node->size;
        add_to_garbage_heap_alloced_list(node);
        modify_heap_alloced_hashmap_with_key(head->addr, head, 1);
        return;
    }
    heap_block *temp = head, *fp = NULL;
    int result = 0, list_counter = 1;
    while(temp != NULL && HEAP_PTR_GREATER(node->addr, temp->addr)){ //(char*)(node->addr) > (char*)(temp->addr)){
        if(list_counter % HEAP_LIST_TRAVERSAL_COUNTER == 0){
            if(detect_loop_in_heap_list(head)){
                printf("HeapError: Circular list detected while inserting node into alloced\n");
                exit(0);
            }
        }
        list_counter++;
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
        modify_heap_alloced_hashmap_with_key(head->addr, head, 1);
        modify_heap_alloced_hashmap_with_key(node->addr, head, 0);
        if(node->next != NULL){
            temp = node->next;
            modify_heap_alloced_hashmap_with_key(temp->addr, node, 0);
        }
        return;
    }
    fp->next = node;
    node->next = temp;
    modify_heap_alloced_hashmap_with_key(node->addr, fp, 0);
    if(temp != NULL)
        modify_heap_alloced_hashmap_with_key(temp->addr, node, 0);
}

void add_to_alloced_list_by_pointer(void* ptr, size_t size){
    if(garbage_heap_alloced_pointer_list == NULL && LIST_ALLOCED_BLOCK_SIZE_REMAINING < sizeof(heap_block))
        expand_heap_allocated_heap_list(0);
    heap_block* new_ptr = (heap_block*)((char*)heap_alloced_pointer_list + HEAP_ALLOCED_POINTER_LIST_SIZE - LIST_ALLOCED_BLOCK_SIZE_REMAINING);
    int isNull = garbage_heap_alloced_pointer_list == NULL;
    new_ptr = isNull ? new_ptr : garbage_heap_alloced_pointer_list;
    LIST_ALLOCED_BLOCK_SIZE_REMAINING -= isNull ? sizeof(heap_block) : 0;
    garbage_heap_alloced_pointer_list = isNull ? NULL : garbage_heap_alloced_pointer_list->next;
    new_ptr->addr = ptr;
    new_ptr->size = size;
    new_ptr->next = NULL;
    add_to_alloced_list_by_node(new_ptr);
}

void* allocate_ptr_for_size(size_t size){
    heap_block* head = (heap_block*)heap_free_pointer_list;
    heap_block *temp = head, *fp = NULL, *xp = NULL, *yp = NULL;
    int list_counter = 1;
    while(temp != NULL){
        if(temp->size >= size){
            if(xp == NULL || xp->size > temp->size){
                yp = fp;
                xp = temp;
            }
        }

        if(list_counter % HEAP_LIST_TRAVERSAL_COUNTER == 0){
            if(detect_loop_in_heap_list(head)){
                printf("HeapError: Detected Circular Linked list while "
                        "looking for free pointer of proper size\n");
                exit(0);
            }
        }
        list_counter++;
        fp = temp;
        temp = temp->next;
    }
    void* answer = NULL;
    if(xp != NULL){
        temp = xp;
        fp = yp;
        // FILE* file = fopen("logs.txt", "a");
        answer = temp->size == ALLOC_SIZE_ADJUST(size) ? temp->addr : (void*)((char*)temp->addr + temp->size - ALLOC_SIZE_ADJUST(size));
        if(temp->size == ALLOC_SIZE_ADJUST(size) && temp != head){
            fp->next = temp->next;
            temp->next = NULL;
            add_to_alloced_list_by_node(temp);
        }
        else if(temp->size == ALLOC_SIZE_ADJUST(size) && temp == head){
            add_to_alloced_list_by_pointer(answer, ALLOC_SIZE_ADJUST(size));
            if(head->next != NULL){
                temp = head->next;
                head->addr = temp->addr;
                head->size = temp->size;
                head->next = temp->next;
                temp->next = NULL;
                add_to_garbage_heap_alloced_list(temp);
            }
            else{
                size_t new_block_size = HEAP_ALLOC_BLOCK;
                void* new_block_addr = mem_alloc(new_block_size);
                head->addr = new_block_addr;
                head->size = new_block_size;
            }
        }
        else{
            temp->size -= ALLOC_SIZE_ADJUST(size);
            add_to_alloced_list_by_pointer(answer, ALLOC_SIZE_ADJUST(size));
        }
        // fprintf(file, "Allocate ptr for size found a free node:\n");
        // fclose(file);
        // view_heap_pointer_status();
        return answer;
    }
    else{
        if(LIST_FREE_BLOCK_SIZE_REMAINING < sizeof(heap_block)){
            expand_heap_allocated_heap_list(1);
        }

        size_t new_block_size = HEAP_ALLOC_BLOCK;
        while(size >= new_block_size)
            new_block_size *= 2;
        
        void* new_block_addr = mem_alloc(new_block_size);
        if(new_block_addr == NULL){
            printf("Heap Overflow: Space of size %d bytes not available\n", new_block_size);
            exit(0);
        }
        // temp = (heap_block*)heap_free_pointer_list;
        // while(temp->next != NULL)
        //     temp = temp->next;
        // temp->next = (heap_block*)((char*)heap_free_pointer_list + HEAP_FREE_POINTER_LIST_SIZE - LIST_FREE_BLOCK_SIZE_REMAINING);
        // temp = (heap_block*)(temp->next);
        // temp->addr = new_block_addr;
        // temp->size = new_block_size - ALLOC_SIZE_ADJUST(size);
        // temp->next = NULL;
        add_to_free_list_by_pointer(new_block_addr, new_block_size - ALLOC_SIZE_ADJUST(size));
        // LIST_FREE_BLOCK_SIZE_REMAINING -= sizeof(heap_block);
        answer = (void*)((char*)new_block_addr + new_block_size - ALLOC_SIZE_ADJUST(size));
        add_to_alloced_list_by_pointer(answer, ALLOC_SIZE_ADJUST(size));
        // FILE* file = fopen("logs.txt", "a");
        // fprintf(file, "Allocate ptr for size created a new free node:\n");
        // fclose(file);
        // view_heap_pointer_status();
        return answer;
    }
}

void* allocate_ptr_array(int count, size_t element_size){
    return allocate_ptr_for_size(count * element_size);
}

void add_to_free_list_by_node(heap_block* node){
    heap_block* head = (heap_block*)heap_free_pointer_list;
    heap_block *temp = head, *fp = NULL;
    int list_counter = 1;
    while(temp != NULL && HEAP_PTR_GREATER(node->addr, temp->addr)){ //(char*)(temp->addr) < (char*)(node->addr)){
        if(list_counter % HEAP_LIST_TRAVERSAL_COUNTER == 0){
            if(detect_loop_in_heap_list(head)){
                printf("HeapError: Cicular linked list detected "
                        "while adding to Free list by node\n");
                exit(0);
            }
        }
        list_counter++;
        fp = temp;
        temp = temp->next;
    }

    if(temp == head){
        // FILE* file = fopen("logs.txt", "a");
        // fprintf(file, "Addresses before %p %p\n", head->addr, node->addr);
        if((void*)((char*)head->addr + head->size) == node->addr){
            head->size += node->size;
            // fprintf(file, "Addresses after %p %p\n", head->addr, node->addr);
            add_to_garbage_heap_alloced_list(node);
        }
        else if((void*)((char*)node->addr + node->size) == head->addr){
            head->addr = node->addr;
            head->size += node->size;
            // fprintf(file, "Addresses after %p %p\n", head->addr, node->addr);
            add_to_garbage_heap_alloced_list(node);
        }
        else{
            void* temp_addr = head->addr;
            size_t temp_size = head->size;
            head->addr = node->addr;
            head->size = node->size;
            node->addr = temp_addr;
            node->size = temp_size;
            node->next = head->next;
            head->next = node;
            // fprintf(file, "Addresses after %p %p\n", head->addr, node->addr);
        }
        // fclose(file);
        // else if(HEAP_PTR_LESSER(head->addr, node->addr)){ //(char*)head->addr < (char*)node->addr){
        //     node->next = head->next;
        //     head->next = node;
        // }
    }
    else{
        if((void*)((char*)fp->addr + fp->size) == node->addr){
            fp->size += node->size;
            add_to_garbage_heap_alloced_list(node);
        }
        else if(temp != NULL && (void*)((char*)node->addr + node->size) == temp->addr){
            temp->size += node->size;
            temp->addr = node->addr;
            add_to_garbage_heap_alloced_list(node);
        }
        else{
            // FILE* file = fopen("logs.txt", "a");
            // fprintf(file, "Did you fall here?\n");
            node->next = fp->next;
            fp->next = node;
            // fclose(file);
        }
    }
}

void add_to_free_list_by_pointer(void* ptr, size_t size){
    if(garbage_heap_alloced_pointer_list == NULL && LIST_FREE_BLOCK_SIZE_REMAINING < sizeof(heap_block))
        expand_heap_allocated_heap_list(1);
    heap_block* new_ptr = (heap_block*)((char*)heap_free_pointer_list + HEAP_FREE_POINTER_LIST_SIZE - LIST_FREE_BLOCK_SIZE_REMAINING);
    int isNull = garbage_heap_alloced_pointer_list == NULL;
    new_ptr = isNull ? new_ptr : garbage_heap_alloced_pointer_list;
    LIST_FREE_BLOCK_SIZE_REMAINING -= isNull ? sizeof(heap_block) : 0;
    garbage_heap_alloced_pointer_list = isNull ? NULL : garbage_heap_alloced_pointer_list->next;
    new_ptr->addr = ptr;
    new_ptr->size = size;
    new_ptr->next = NULL;
    add_to_free_list_by_node(new_ptr);
}

void free_pointer(void* ptr){
    if(ptr == NULL){
        printf("HeapError: Null Pointer cannot be deleted\n");
        exit(0);
    }
    heap_block* head1 = (heap_block*)heap_alloced_pointer_list;
    heap_block *temp = NULL, *fp = NULL;
    // while(temp != NULL && ptr > temp->addr){
    //     fp = temp;
    //     temp = temp->next;
    // }

    HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE struct_val = find_from_heap_alloced_hashmap(ptr);
    // find from heap alloced hashmap return a struct of a pointer and a char
    // the pointer indicates the pointer of the node before the node which encloses the ptr
    // the only exception is the node enclosing ptr being the head, that's where the char value is used to check that
    // Basically pointer returned -> next is the node we need, this is done to ease the deletion process
    temp = (heap_block*)struct_val.ptr;
    int isHead = struct_val.type == 'h';
    
    if(temp == NULL){
        printf("HeapError: Pointer cannot be freed\n");
        exit(0);
    }

    size_t size = temp->size;
    if(temp == head1 && isHead){     //temp == head1){
        delete_key_from_heap_alloced_hashmap(head1->addr);
        if(head1->next == NULL){
            head1->addr = NULL;
            head1->size = 0;
        }
        else{
            head1->addr = head1->next->addr;
            head1->size = head1->next->size;
            temp = head1->next;
            modify_heap_alloced_hashmap_with_key(head1->addr, head1, 1);
            head1->next = head1->next->next;
            add_to_garbage_heap_alloced_list(temp);
        }
        add_to_free_list_by_pointer(ptr, size);
    }
    else{
        fp = temp;
        temp = temp->next;
        delete_key_from_heap_alloced_hashmap(ptr);
        fp->next = temp->next;
        temp->next = NULL;
        add_to_free_list_by_node(temp);
    }
}

void view_heap_pointer_status(){
    // FILE* fp = fopen("logs.txt", "a");
    heap_block* temp;
    heap_block* heads[2] = {heap_alloced_pointer_list, heap_free_pointer_list};
    char* lines[2] = {"Alloced blocks: ", "Free blocks: "};
    for(int i = 0; i < 2; i++){
        printf("%s\n", lines[i]);
        // fprintf(fp, "%s\n", lines[i]);
        temp = heads[i];
        if(detect_loop_in_heap_list(heads[i])){
            printf("HeapError: Circular linked list detected while "
                    "viewing heap pointer status\n");
            // fprintf(fp, "HeapError: Circular linked list detected while "
            //         "viewing heap pointer status\n");
            exit(0);
        }
        while(temp != NULL){
            printf("%p -> %p: %d\n", temp, temp->addr, temp->size);
            // fprintf(fp, "%p -> %p: %d -> next: %p\n", temp, temp->addr, temp->size, temp->next);
            temp = temp->next;
        }
        // fprintf(fp, "\n\n");
        printf("\n\n");
        // fclose(fp);
    }
    // fclose(fp);
}

void copy_heap_alloced_memory(void* des, void* src, size_t size){
    char *des_char = (char*)des, *src_char = (char*)src;
    for(size_t i = 0; i < size; i++)
        des_char[i] = src_char[i];
}

void move_heap_alloced_memory(void* des, void* src, size_t size){
    char *des_char = (char*)des, *src_char = (char*)src;
    // printf("Moving... %d\n", size);
    for(size_t i = 0; i < size; i++){
        // printf("Step %d\n", i);
        des_char[i] = src_char[i];
        src_char[i] = '\0';
    }
}

void* reallocate_heap_alloced_ptr(void* ptr, size_t new_size){
    HEAP_ALLOCED_HASHMAP_VALUE_RETURN_TYPE result = find_from_heap_alloced_hashmap(ptr);
    if(result.ptr == NULL){
        printf("HeapError: Given pointer cannot be reallocated\n");
        exit(0);
    }
    void* new_ptr = allocate_ptr_for_size(new_size);
    heap_block* node = (heap_block*)result.ptr;
    node = result.type == 'h' ? node : node->next;
    size_t size = node->size;
    // printf("Mem: %p %p\n", ptr, node);
    move_heap_alloced_memory(new_ptr, ptr, size);
    // printf("Made it here\n");
    free_pointer(ptr);
    // printf("%p successfully reallocated to %p\n", ptr, new_ptr);
    return new_ptr;
}

void set_heap_alloced_memory(void* ptr, int val, size_t size){
    if(ptr == NULL)
        return;
    size_t length = size;
    char* c_ptr = (char*)ptr;
    while(length > 0){
        *c_ptr = val;
        c_ptr++;
        length--;
    }
}