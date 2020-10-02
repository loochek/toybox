#include <stdio.h>

const size_t SECURITY_MARKER = 0xABADBABE;

// int
#define TYPE int
typedef int elem_t;
const elem_t POISON = 0xDEAD;
#include "stack_impl.h"
#undef TYPE