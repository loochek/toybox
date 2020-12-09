#include "vector.h"

// TODO: rewrite

static void lexem_vector_expand(lexem_vector_t *vector);

void vector_view_construct(lexem_vector_t *vector, lexem_t *buffer, size_t capacity)
{
    vector->data = buffer;
    //vector->data     = calloc(capacity, sizeof(lexem_t));
    vector->capacity = capacity;
    vector->size     = 0;
}

void vector_view_push(lexem_vector_t *vector, lexem_t elem)
{
    if (vector->size >= vector->capacity)
        return;

    vector->data[vector->size] = elem;

    vector->size++;
}

lexem_t *vector_view_at(lexem_vector_t *vector, size_t pos)
{
    return &vector->data[pos];
}

void vector_view_destruct(lexem_vector_t *vector)
{
    //free(vector->data);
}

static void lexem_vector_expand(lexem_vector_t *vector)
{
    vector->data = realloc(vector->data, vector->capacity * 2 * sizeof(lexem_t));
    vector->capacity *= 2;
}