// предназначен для однократного включения

#include <stdlib.h>

#define __overload(name, type) name ## _ ## type 
#define __overload2(name, type) __overload(name, type)
#define OVERLOAD(name) __overload2(name, TYPE)

static const char* stack_status_text[] = 
{
    "ok",
    "empty",
    "Stack error!",
    "Stack corrupted!",
    "Stack shrink denied!",
    "Stack canary fault!",
    "Stack poison fault!",
    "Stack hash mismatch!",
    "Stack null pointer!"
};

typedef enum
{
    STACK_OK,
    STACK_EMPTY,
    STACK_ERROR,
    STACK_CORRUPTED,
    STACK_SHRINK_DENIED,
    STACK_CANARY_FAULT,
    STACK_POISON_FAULT,
    STACK_HASH_MISMATCH,
    STACK_NULL_POINTER
} stack_status_t;