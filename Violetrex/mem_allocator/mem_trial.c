#include "mem_alloc.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    start_Dynamic_Mem();
    void* xx = mem_alloc(5 * sizeof(int));
    int df = 0;
    printf("Pointer assigned: %p %d %d\n", xx, df, sizeof(xx));
    return 0;
}