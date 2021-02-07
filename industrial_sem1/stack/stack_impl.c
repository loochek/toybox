#include <stdio.h>

#include "stack_common.h"

// переключатели защиты
// убедитесь, что при подключении обьявления они были в одинаковом состоянии!
#define STACK_SEC_HASHING
#define STACK_SEC_CANARY
#define STACK_SEC_POISON

// константы настроек
static const size_t SECURITY_MARKER = 0xABADBABE;
static const size_t HASH_BASE = 31;
static const size_t HASH_MOD = 1000000033;
static const size_t SHRINK_THRESHOLD = 5;

// тут ваши включения стеков

// int
static void stack_print_int(int *a)
{
    printf("%d", *a);
}

#define TYPE int
static const TYPE OVERLOAD(POISON) = 0xDEAD;
#include "stack_impl.h"
#undef TYPE

// double
static void stack_print_double(double *a)
{
    printf("%lf", *a);
}

#define TYPE double
static const TYPE OVERLOAD(POISON) = 0xDEAD;
#include "stack_impl.h"
#undef TYPE