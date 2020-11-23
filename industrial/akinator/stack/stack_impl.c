#include <stdio.h>
#include <stdint.h>

#include "stack_common.h"
 
// переключатели защиты
// убедитесь, что при подключении обьявления они были в одинаковом состоянии!
//#define STACK_SEC_HASHING
//#define STACK_SEC_CANARY
//#define STACK_SEC_POISON

// константы настроек
static const size_t SECURITY_MARKER = 0xABADBABE;
static const size_t HASH_BASE = 31;
static const size_t HASH_MOD = 1000000033;
static const size_t SHRINK_THRESHOLD = 5;

// тут ваши включения стеков

#define elem_t void*

static void stack_print_node(elem_t *elem)
{
    // TODO
}

#define TYPE node
static const elem_t OVERLOAD(POISON) = 0xDEAD;
#include "stack_impl.h"
#undef TYPE
#undef elem_t
