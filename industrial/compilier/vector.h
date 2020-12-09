#include <stdlib.h>

#include "lexer.h"

// TODO: rewrite

struct lexem;

typedef struct vector
{
    lexem_t *data;

    size_t size;
    size_t capacity;

} vector_view_t;

void     vector_view_construct(lexem_vector_t *vector, size_t capacity);
void     vector_view_push     (lexem_vector_t *vector, lexem_t elem);
lexem_t *vector_view_at       (lexem_vector_t *vector);
void     vector_view_destruct (lexem_vector_t *vector);