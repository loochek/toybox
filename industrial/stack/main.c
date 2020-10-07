#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stack_common.h"

// давай ты не будешь трогать это, ок?

// переключатели защиты
#define STACK_SEC_HASHING
#define STACK_SEC_CANARY
#define STACK_SEC_POISON

// тут уже можно трогать

// int
#define TYPE int
#include "stack.h"
#undef TYPE

int main()
{
    // пример:
    my_stack_int stack = {};
    
    stack_construct_int(&stack, 4);
    
    stack_push_int(&stack, 1);
    stack_push_int(&stack, 2);
    stack_push_int(&stack, 3);
    stack_push_int(&stack, 4);
    size_t size = 0;
    stack_size_int(&stack, &size); // size1 = 4
    stack_pop_int(&stack);
    int elem = 0;
    stack_top_int(&stack, &elem); // elem = 3

    printf("%zu %d\n", size, elem); // 4 3 
    stack_destruct_int(&stack);
    return 0;
}