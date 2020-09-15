#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int *mem;
    size_t size;
    size_t curr_head;
} stack;

// sdf

//typedef enum { STACK_OK, STACK_NULL, STACK_OVERFLOW, STACK_UNDERFLOW } stack_code;

stack *stack_new(size_t size)
{
    stack *st = (stack*)malloc(sizeof(stack));
    if (st == NULL)
        return NULL;
    st->size = size;
    st->curr_head = 0;
    st->mem = (int*)malloc(sizeof(int) * size);
    if (st->mem == NULL)
        return NULL;
    return st;
}

stack_code stack_delete(stack *st)
{
    if (st == NULL)
        return STACK_NULL;
    if (st->mem != NULL)
        free(st->mem);
    free(st);
    return STACK_OK;
}

stack_code stack_push(stack *st, int value)
{
    if (st == NULL)
        return STACK_NULL;
    if (st->mem == NULL)
        return STACK_NULL;
    if (st->curr_head == st->size)
        return STACK_OVERFLOW;
    st->mem[st->curr_head] = value;
    st->curr_head++;
    return STACK_OK;
}

stack_code stack_pop(stack *st, int *value)
{
    if (st == NULL)
        return STACK_NULL;
    if (st->mem == NULL)
        return STACK_NULL;
    if (st->curr_head == 0)
        return STACK_UNDERFLOW;
    if (value == NULL)
        return STACK_NULL;
    st->curr_head--;
    *value = st->mem[st->curr_head];
    return STACK_OK;
}

// int main()
// {
//     stack *st = stack_new(10);
//     for (int i = 0; i < 10; i++)
//         stack_push(st, i);
//     stack_code e = stack_push(st, 1);
//     for (int i = 0; i < 10; i++)
//     {
//         int tmp;
//         stack_code e1 = stack_pop(st, &tmp);
//         printf("%d ", tmp);
//     }
//     stack_code e2 = stack_delete(st);
// }