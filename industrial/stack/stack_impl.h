#include <stdlib.h>
#include <stdio.h>

#define __overload(name, type) name ## _ ## type 
#define __overload2(name, type) __overload(name, type)
#define OVERLOAD(name) __overload2(name, TYPE)

#define __sharp(a) #a
#define __lit_pair(a, b) __sharp(a) __sharp(b)
#define STACK_NAME_LIT() __lit_pair(my_stack_, TYPE)

#define STACK_ASSERT(self)                          \
{                                                   \
    if (OVERLOAD(stack_validate)(self) != STACK_OK) \
    {                                               \
        printf("Stack corruption detected!\n");     \
        OVERLOAD(stack_dump)(self);                 \
        return STACK_CORRUPTED;                     \
    }                                               \
}

#define SECURITY_MARKER 0xABADBABE

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

stack_status_t OVERLOAD(stack_construct)(OVERLOAD(my_stack) *self, size_t initial_capacity)
{
    self->size = 0;
    self->security_marker1 = SECURITY_MARKER;
    self->security_marker2 = SECURITY_MARKER;

    self->data = (elem_t*)calloc(initial_capacity, sizeof(elem_t));
    if (self->data == NULL)
        return STACK_ERROR;

    self->capacity = initial_capacity;
    for (int i = 0; i < self->capacity; i++)
        self->data[i] = POISON;

    return STACK_OK;
}

stack_status_t OVERLOAD(stack_push)(OVERLOAD(my_stack) *self, elem_t elem)
{
    STACK_ASSERT(self);

    if (self->size == self->capacity)
        if (OVERLOAD(stack_expand)(self) != STACK_OK)
            return STACK_ERROR;
    
    self->data[self->size++] = elem;

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_pop)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    if (self->size == 0)
        return STACK_EMPTY;

    self->size--;
    self->data[self->size] = POISON;
    OVERLOAD(stack_shrink)(self);

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_size)(OVERLOAD(my_stack) *self, size_t *size)
{
    STACK_ASSERT(self);

    *size = self->size;

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_top)(OVERLOAD(my_stack) *self, elem_t *elem)
{
    STACK_ASSERT(self);

    if (self->size == 0)
        return STACK_EMPTY;

    *elem = self->data[self->size - 1];

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_destruct)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    free(self->data);
    return STACK_OK;
}

static stack_status_t OVERLOAD(stack_expand)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    elem_t *new_data = (elem_t*)realloc(self->data, sizeof(elem_t) * self->capacity * 2);
    if (new_data == NULL)
        return STACK_ERROR;

    self->data = new_data;

    for (size_t i = self->capacity; i < self->capacity * 2; i++)
        self->data[i] = POISON;

    self->capacity *= 2;

    STACK_ASSERT(self);
    return STACK_OK;
}

static stack_status_t OVERLOAD(stack_shrink)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    if (self->capacity / 2 < self->size + 10)
        return STACK_SHRINK_DENIED;
    
    elem_t *new_data = (elem_t*)realloc(self->data, sizeof(elem_t) * (self->capacity / 2 - 10));
    if (new_data == NULL)
        return STACK_ERROR;

    self->data = new_data;
    self->capacity = self->capacity / 2 - 10;

    STACK_ASSERT(self);
    return STACK_OK;
}

static stack_status_t OVERLOAD(stack_validate)(OVERLOAD(my_stack) *self)
{
    if (self->security_marker1 != SECURITY_MARKER)
        return STACK_CORRUPTED;
    if (self->security_marker2 != SECURITY_MARKER)
        return STACK_CORRUPTED;
    return STACK_OK;
}

static void OVERLOAD(stack_dump)(OVERLOAD(my_stack) *self)
{
    stack_status_t status = OVERLOAD(stack_validate)(self);

    printf(STACK_NAME_LIT() "[%s] at %p\n",
            (status == STACK_OK) ? "ok" : "ERROR!", self);

    printf("{\n"
           "    size = %zu\n"
           "    capacity = %zu\n"
           "    data[%p]\n"
           "    {\n",
           self->size, self->capacity, self->data);
    for (size_t i = 0; i < self->capacity; i++)
    {
        printf("   %c[%zu] = %d %s\n", (i < self->size) ? '*' : ' ', i, self->data[i],
                                        (self->data[i] == POISON) ? "[poison value]" : "");
    }
    printf("    }\n}\n");
}