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

void expand_heap_allocated_heap_list(int flag){
    size_t size, occupied_size, new_size, new_remaining;
    void* ptr;
    heap_block *head, *temp, *fp;
    heap_block *new_head, *new_temp;
    heap_block* heads[2] = {heap_free_pointer_list, heap_alloced_pointer_list};

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
    for(int i = 0; i < 2; i++){
        temp = heads[i];
        fp = NULL;
        while(temp != NULL){
            if(temp - head < occupied_size && temp >= head){
                new_temp = new_head + new_size - new_remaining;
                new_remaining -= sizeof(heap_block);
                new_temp->addr = temp->addr;
                new_temp->size = temp->size;
                if(fp != NULL)
                    fp->next = new_temp;
                new_temp->next = temp->next;
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
}

void add_to_alloced_list_by_node(heap_block* node){
    heap_block* head = (heap_block*)heap_alloced_pointer_list;
    if(head->addr == NULL){
        head->addr = node->addr;
        head->size = node->size;
        return;
    }
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
    if(LIST_ALLOCED_BLOCK_SIZE_REMAINING < sizeof(heap_block))
        expand_heap_allocated_heap_list(0);
    heap_block* new_ptr = (heap_block*)(heap_alloced_pointer_list + HEAP_ALLOCED_POINTER_LIST_SIZE - LIST_ALLOCED_BLOCK_SIZE_REMAINING);
    LIST_ALLOCED_BLOCK_SIZE_REMAINING -= sizeof(heap_block);
    new_ptr->addr = ptr;
    new_ptr->size = size;
    new_ptr->next = NULL;
    add_to_alloced_list_by_node(new_ptr);
}

void* allocate_ptr_for_size(size_t size){
    heap_block* head = (heap_block*)heap_free_pointer_list;
    heap_block *temp = head, *fp = NULL, *xp = NULL, *yp = NULL;
    while(temp != NULL){
        if(temp->size >= size){
            if(xp == NULL || xp->size > temp->size){
                yp = fp;
                xp = temp;
            }
        }
        fp = temp;
        temp = temp->next;
    }
    void* answer = NULL;
    if(xp != NULL){
        temp = xp;
        fp = yp;
        answer = temp->size == size ? temp->addr : temp->addr + temp->size - ALLOC_SIZE_ADJUST(size);
        if(temp->size == size && temp != head){
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
        add_to_alloced_list_by_pointer(answer, ALLOC_SIZE_ADJUST(size));
        return answer;
    }
}

void add_to_free_list_by_node(heap_block* node){
    heap_block* head = (heap_block*)heap_free_pointer_list;
    heap_block *temp = head, *fp = NULL;

    while(temp != NULL && temp->addr < node->addr){
        fp = temp;
        temp = temp->next;
    }

    if(temp == head){
        if(head->addr + head->size == node->addr)
            head->size += node->size;
        else if(node->addr + node->size == head->addr){
            head->addr = node->addr;
            head->size += node->size;
        }
        else if(head->addr < node->addr){
            node->next = head->next;
            head->next = node;
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
        }
    }
    else{
        if(fp->addr + fp->size == node->addr){
            fp->size += node->size;
        }
        else if(temp != NULL && node->addr + node->size == temp->addr){
            temp->size += node->size;
            temp->addr = node->addr;
        }
        else{
            node->next = fp->next;
            fp->next = node;
        }
    }
}

void add_to_free_list_by_pointer(void* ptr, size_t size){
    if(LIST_FREE_BLOCK_SIZE_REMAINING < sizeof(heap_block))
        expand_heap_allocated_heap_list(1);
    heap_block* new_ptr = (heap_block*)(heap_free_pointer_list + HEAP_FREE_POINTER_LIST_SIZE - LIST_FREE_BLOCK_SIZE_REMAINING);
    LIST_FREE_BLOCK_SIZE_REMAINING -= sizeof(heap_block);
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
    heap_block *temp = head1, *fp = NULL;
    while(temp != NULL && ptr > temp->addr){
        fp = temp;
        temp = temp->next;
    }
    
    if(temp == NULL || temp->addr != ptr){
        printf("HeapError: Pointer does not exist\n");
        exit(0);
    }

    size_t size = temp->size;
    if(temp == head1){
        if(head1->next == NULL){
            head1->addr = NULL;
            head1->size = 0;
        }
        else{
            head1->addr = head1->next->addr;
            head1->size = head1->next->size;
            head1->next = head1->next->next;
        }
        add_to_free_list_by_pointer(ptr, size);
    }
    else{
        fp->next = temp->next;
        temp->next = NULL;
        add_to_free_list_by_node(temp);
    }
}