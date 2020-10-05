#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

//------------------------------------------

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

//---приватный интерфейс------------------------------

static stack_status_t OVERLOAD(stack_expand   )(OVERLOAD(my_stack) *self);
static stack_status_t OVERLOAD(stack_shrink   )(OVERLOAD(my_stack) *self);
static stack_status_t OVERLOAD(stack_validate )(OVERLOAD(my_stack) *self);
static           void OVERLOAD(stack_dump     )(OVERLOAD(my_stack) *self);

#ifdef STACK_SEC_HASHING
static size_t OVERLOAD(stack_calc_struct_hash)(OVERLOAD(my_stack) *self);
static size_t OVERLOAD(stack_calc_data_hash  )(OVERLOAD(my_stack) *self);
#endif

// должно быть реализовано в stack_impl.c

// функция печати
static void OVERLOAD(stack_print)(TYPE *a);

// константы настроек
extern const size_t SECURITY_MARKER;
extern const size_t HASH_BASE;
extern const size_t HASH_MOD;
extern const size_t SHRINK_THRESHOLD;

extern const TYPE   OVERLOAD(POISON);

//---реализация--------------------------------------

stack_status_t OVERLOAD(stack_construct)(OVERLOAD(my_stack) *self, size_t initial_capacity)
{
    self->data = (TYPE*)calloc(initial_capacity, sizeof(TYPE));
    if (self->data == NULL)
        return STACK_ERROR;

    self->capacity = initial_capacity;
    self->size = 0;

#ifdef STACK_SEC_CANARY
    self->security_marker1 = SECURITY_MARKER;
    self->security_marker2 = SECURITY_MARKER;
#endif
#ifdef STACK_SEC_POISON
    for (size_t i = 0; i < self->capacity; i++)
        self->data[i] = OVERLOAD(POISON);
#endif
#ifdef STACK_SEC_HASHING
    self->struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    self->data_hash   = OVERLOAD(stack_calc_data_hash  )(self);
#endif
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_push)(OVERLOAD(my_stack) *self, TYPE elem)
{
    STACK_ASSERT(self);

    if (self->size == self->capacity)
        if (OVERLOAD(stack_expand)(self) != STACK_OK)
            return STACK_ERROR;
    
    self->data[self->size++] = elem;

#ifdef STACK_SEC_HASHING
    self->struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    self->data_hash   = OVERLOAD(stack_calc_data_hash  )(self);
#endif

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_pop)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    if (self->size == 0)
        return STACK_EMPTY;

    self->size--;

#ifdef STACK_SEC_POISON
    self->data[self->size] = OVERLOAD(POISON);
#endif
#ifdef STACK_SEC_HASHING
    self->struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    self->data_hash   = OVERLOAD(stack_calc_data_hash  )(self);
#endif

    OVERLOAD(stack_shrink)(self);

    STACK_ASSERT(self);
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_size)(OVERLOAD(my_stack) *self, size_t *size)
{
    STACK_ASSERT(self);

    *size = self->size;
    return STACK_OK;
}

stack_status_t OVERLOAD(stack_top)(OVERLOAD(my_stack) *self, TYPE *elem)
{
    STACK_ASSERT(self);

    if (self->size == 0)
        return STACK_EMPTY;

    *elem = self->data[self->size - 1];
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

    TYPE *new_data = (TYPE*)realloc(self->data, sizeof(TYPE) * self->capacity * 2);
    if (new_data == NULL)
        return STACK_ERROR;

    self->data = new_data;

#ifdef STACK_SEC_POISON
    for (size_t i = self->capacity; i < self->capacity * 2; i++)
        self->data[i] = OVERLOAD(POISON);
#endif

    self->capacity *= 2;

#ifdef STACK_SEC_HASHING
    self->struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    self->data_hash   = OVERLOAD(stack_calc_data_hash  )(self);
#endif

    STACK_ASSERT(self);
    return STACK_OK;
}

static stack_status_t OVERLOAD(stack_shrink)(OVERLOAD(my_stack) *self)
{
    STACK_ASSERT(self);

    if (self->capacity / 2 < self->size + SHRINK_THRESHOLD)
        return STACK_SHRINK_DENIED;
    
    TYPE *new_data = (TYPE*)realloc(self->data,
                                        sizeof(TYPE) * (self->capacity / 2));
    if (new_data == NULL)
        return STACK_ERROR;

    self->data = new_data;
    self->capacity = self->capacity / 2;

#ifdef STACK_SEC_HASHING
    self->struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    self->data_hash   = OVERLOAD(stack_calc_data_hash  )(self);
#endif

    STACK_ASSERT(self);
    return STACK_OK;
}

#ifdef STACK_SEC_HASHING
static size_t OVERLOAD(stack_calc_struct_hash)(OVERLOAD(my_stack) *self)
{
    size_t old_struct_hash = self->struct_hash;
    size_t old_data_hash   = self->data_hash;
    self->struct_hash = 0;
    self->data_hash   = 0;
    
    size_t hash = 0;

    char *mem = (char*)self;
    for (size_t i = 0; i < sizeof(OVERLOAD(my_stack)); i++)
        hash = ((hash * HASH_BASE) % HASH_MOD + mem[i]) % HASH_MOD;

    self->struct_hash = old_struct_hash;
    self->data_hash   = old_data_hash;
    return hash;
}

static size_t OVERLOAD(stack_calc_data_hash)(OVERLOAD(my_stack) *self)
{
    size_t old_struct_hash = self->struct_hash;
    size_t old_data_hash   = self->data_hash;
    self->struct_hash = 0;
    self->data_hash   = 0;
    
    size_t hash = 0;

    char *mem = (char*)self->data;
    for (size_t i = 0; i < self->capacity * sizeof(TYPE); i++)
        hash = ((hash * HASH_BASE) % HASH_MOD + mem[i]) % HASH_MOD;

    self->struct_hash = old_struct_hash;
    self->data_hash   = old_data_hash;
    return hash;
}
#endif

static stack_status_t OVERLOAD(stack_validate)(OVERLOAD(my_stack) *self)
{
    if (self == NULL)
            return STACK_NULL_POINTER;

#ifdef STACK_SEC_HASHING
    if (self->security_marker1 != SECURITY_MARKER)
        return STACK_CANARY_FAULT;
    if (self->security_marker2 != SECURITY_MARKER)
        return STACK_CANARY_FAULT;
#endif
#ifdef STACK_SEC_POISON
    for (size_t i = self->size; i < self->capacity; i++)
        if (self->data[i] != OVERLOAD(POISON))
            return STACK_POISON_FAULT;
#endif
#ifdef STACK_SEC_HASHING
    if (self->struct_hash != OVERLOAD(stack_calc_struct_hash)(self))
        return STACK_HASH_MISMATCH;
    if (self->data_hash   != OVERLOAD(stack_calc_data_hash  )(self))
        return STACK_HASH_MISMATCH;
#endif
    return STACK_OK;
}

static void OVERLOAD(stack_dump)(OVERLOAD(my_stack) *self)
{
    stack_status_t status = OVERLOAD(stack_validate)(self);

    printf(STACK_NAME_LIT() "[%s] at %p\n",
            stack_status_text[status], self);

    if (status == STACK_NULL_POINTER)
    {
        printf("{ null pointer! }\n");
        return;
    }
       
    printf("{\n"
           "    size = %zu\n"
           "    capacity = %zu\n",
            self->size, self->capacity);

#ifdef STACK_SEC_HASHING
    size_t real_struct_hash = OVERLOAD(stack_calc_struct_hash)(self);
    if (self->struct_hash == real_struct_hash)
    {
        printf("    struct_hash = %zu\n"
               "    real_struct_hash = %zu\n"
               "    data_hash = %zu\n"
               "    real_data_hash = %zu\n",
                self->struct_hash, real_struct_hash,
                self->data_hash  , OVERLOAD(stack_calc_data_hash)(self));
    }   
    else
    {
        printf("    struct_hash = %zu\n"
               "    real_struct_hash = %zu\n"
               "    data_hash is not calculated due the trashed struct\n"
               "    data[%p] is not displayed due the trashed struct\n",
                self->struct_hash, real_struct_hash, self->data);
        printf("}\n");
        return;
    }
#endif
    
    printf("    data[%p]\n"
           "    {\n",
            self->data);
    for (size_t i = 0; i < self->capacity; i++)
    {
        printf("   %c[%zu] = ", (i < self->size) ? '*' : ' ', i);
        OVERLOAD(stack_print)(&self->data[i]);
#ifdef STACK_SEC_POISON
        printf(" %s", (self->data[i] == OVERLOAD(POISON)) ? "[poison value]" : "");
#endif
        printf("\n");
    }
    printf("    }\n}\n");
}