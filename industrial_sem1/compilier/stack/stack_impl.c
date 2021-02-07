#include <stdio.h>
#include <stdint.h>

#include "stack_common.h"
#include "../lexer.h"
#include "../string_view.h"

static const size_t SHRINK_THRESHOLD = 5;

// тут ваши включения стеков

#define elem_t lexem_t

static void stack_print_lexem(elem_t *elem)
{
}

#define TYPE lexem
#include "stack_impl.h"
#undef TYPE
#undef elem_t

#define elem_t string_view_t

static void stack_print_strview(elem_t *elem)
{
}

#define TYPE strview
#include "stack_impl.h"
#undef TYPE
#undef elem_t

#define elem_t my_stack_strview

static void stack_print_table(elem_t *elem)
{
}

#define TYPE table
#include "stack_impl.h"
#undef TYPE
#undef elem_t