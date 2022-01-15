#include <stdio.h>
#include "symbols.h"

int add(int a, int b)
{
	return a + b;
}

extern void exit(int __status) __THROW __attribute__ ((__noreturn__));

void overflow(void) __THROW __attribute__((noreturn));

int main(){
	start_Dynamic_Mem();
	char c = 'a';
	int count = 0;
	printf("%s", INT_TO_STR(0));
	while((c = getchar()) != EOF)
		if(c != '\n') printf("%c%d", c, count++);
		else break;
	
	char cc = 'b';
	void* ptr1 = allocate_ptr_for_size(566);
	void* ptr2 = allocate_ptr_for_size(1000);
	printf("\n%p %p\n", &c, &cc);
	printf("Outcome: %d\n", (char*)ptr1 - (char*)ptr2);
	printf("Conversion " __HEAP_ALLOCED_HASHMAP_INDEX_TYPE__FORMAT__ " \n", HEAP_PTR_INT(ptr1));
	printf("Conversion " __HEAP_ALLOCED_HASHMAP_INDEX_TYPE__FORMAT__ " \n", HEAP_PTR_INT(ptr2));
	printf("Finally %d\n", HEAP_PTR_LESSER(ptr1, ptr2));
	printf("Finally %d\n", HEAP_PTR_LESSER_EQ(ptr1, ptr2));
	printf("Finally %d\n", HEAP_PTR_GREATER(ptr1, ptr2));
	printf("Finally %d\n", HEAP_PTR_GREATER_EQ(ptr1, ptr2));
	printf("Size of heap list %d\n", sizeof(heap_block));
	exit(0);
	overflow();
	return 0;
}

void overflow(void){
	printf("Made it to overflow\n");
	while(1);
}