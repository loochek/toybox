#include <stdlib.h>

typedef struct
{
#ifdef STACK_SEC_CANARY
    size_t security_marker1;
#endif
#ifdef STACK_SEC_HASHING
    size_t struct_hash;
    size_t data_hash;
#endif

    elem_t *data;
    size_t size;
    size_t capacity;

#ifdef STACK_SEC_CANARY
    size_t security_marker2;
#endif
} OVERLOAD(my_stack);

stack_status_t OVERLOAD(stack_construct)(OVERLOAD(my_stack) *self, size_t initial_capacity);
stack_status_t OVERLOAD(stack_push     )(OVERLOAD(my_stack) *self, elem_t elem);
stack_status_t OVERLOAD(stack_pop      )(OVERLOAD(my_stack) *self);
stack_status_t OVERLOAD(stack_size     )(OVERLOAD(my_stack) *self, size_t *size);
stack_status_t OVERLOAD(stack_top      )(OVERLOAD(my_stack) *self, elem_t *elem);
stack_status_t OVERLOAD(stack_destruct )(OVERLOAD(my_stack) *self);
void           OVERLOAD(stack_dump     )(OVERLOAD(my_stack) *self);
stack_status_t OVERLOAD(stack_validate )(OVERLOAD(my_stack) *self);