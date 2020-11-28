#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>

/**
 * Simple alloc-only memory allocator
 * Works great with small allocations, degenerates to malloc with big allocations
 */

#define BLOCK_SIZE       32768
#define MAX_BLOCKS_COUNT 1024

typedef struct 
{
    void  *memory_blocks[MAX_BLOCKS_COUNT];

    size_t current_offset;
    size_t current_block;
} memory_pool_t;

/**
 * Behaves as usual calloc
 * LERR-affecting
 */
void* calloc_custom(size_t cnt, size_t size, memory_pool_t *pool);

/**
 * LERR-affecting
 */
void pool_construct(memory_pool_t *pool);

/**
 * LERR-affecting
 */
void pool_destruct(memory_pool_t *pool);

/**
 * Returns 0 if passed pool is correct, else -1
 * LERR-affecting
 */
int pool_validate(memory_pool_t *pool);

#endif