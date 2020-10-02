#include <stdlib.h>

#define __overload(name, type) name ## _ ## type 
#define __overload2(name, type) __overload(name, type)
#define OVERLOAD(name) __overload2(name, TYPE)

typedef enum
{
    STACK_OK,
    STACK_EMPTY,
    STACK_ERROR,
    STACK_FATAL,
    STACK_SHRINK_DENIED,
    STACK_CORRUPTED
} stack_status_t;

typedef struct
{
    size_t security_marker1;

    size_t size;
    size_t capacity;
    elem_t *data;

    size_t security_marker2;
} OVERLOAD(my_stack);

       stack_status_t OVERLOAD(stack_construct)(OVERLOAD(my_stack) *self, size_t initial_capacity);
       stack_status_t OVERLOAD(stack_push     )(OVERLOAD(my_stack) *self, elem_t elem);
       stack_status_t OVERLOAD(stack_pop      )(OVERLOAD(my_stack) *self);
       stack_status_t OVERLOAD(stack_size     )(OVERLOAD(my_stack) *self, size_t *size);
       stack_status_t OVERLOAD(stack_top      )(OVERLOAD(my_stack) *self, elem_t *elem);
       stack_status_t OVERLOAD(stack_destruct )(OVERLOAD(my_stack) *self);

static stack_status_t OVERLOAD(stack_expand   )(OVERLOAD(my_stack) *self);
static stack_status_t OVERLOAD(stack_shrink   )(OVERLOAD(my_stack) *self);
static stack_status_t OVERLOAD(stack_validate )(OVERLOAD(my_stack) *self);
static           void OVERLOAD(stack_dump     )(OVERLOAD(my_stack) *self);