#include "allocator.h"
#include "lerror.h"

#define POOL_CHECK_RET(pool, ret_val)                                            \
{                                                                            \
    if (pool_validate(pool) != 0)                                            \
    {                                                                        \
        fprintf(stderr, "Memory pool validation failed: %s\n", __lerr_str);  \
        return ret_val;                                                      \
    }                                                                        \
}

static void pool_expand(memory_pool_t *pool);

void pool_construct(memory_pool_t *pool)
{
    LERR_RESET();

    pool->current_offset = 0;
    pool->current_block  = 0;


    pool->memory_blocks[0] = calloc(BLOCK_SIZE, 1);
    if (pool->memory_blocks[0] == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory block");
        return;
    }
}

void* calloc_custom(size_t cnt, size_t size, memory_pool_t *pool)
{
    POOL_CHECK_RET(pool, NULL);

    if (cnt * size > BLOCK_SIZE)
    {
        LERR(LERR_ALLOC, "too big memory request");
        return NULL;
    }
    
    if (BLOCK_SIZE - pool->current_offset < cnt * size)
    {
        pool_expand(pool);
        if (LERR_PRESENT())
            return NULL;
    }

    void *to_ret = (char*)pool->memory_blocks[pool->current_block] + pool->current_offset;
    pool->current_offset += size * cnt;
    return to_ret;
}

static void pool_expand(memory_pool_t *pool)
{
    LERR_RESET();
    POOL_CHECK_RET(pool,);

    if (pool->current_block + 1 >= MAX_BLOCKS_COUNT)
    {
        LERR(LERR_ALLOC, "pool is full");
        return;
    }

    pool->memory_blocks[pool->current_block + 1] = calloc(BLOCK_SIZE, 1);
    if (pool->memory_blocks[pool->current_block + 1] == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory pool");
        return;
    }

    pool->current_block++;
    pool->current_offset = 0;
}

void pool_destruct(memory_pool_t *pool)
{
    POOL_CHECK_RET(pool,);

    for (size_t i = 0; i < pool->current_block + 1; i++)
        free(pool->memory_blocks[i]);
}

int pool_validate(memory_pool_t *pool)
{
    LERR_RESET();

    if (pool == NULL)
    {
        LERR(LERR_POOL_VALIDATION, "null pointer");
        return -1;
    }
    if (pool->current_block >= MAX_BLOCKS_COUNT)
    {
        LERR(LERR_POOL_VALIDATION, "too much blocks");
        return -1;
    }
    if (pool->current_offset >= BLOCK_SIZE)
    {
        LERR(LERR_POOL_VALIDATION, "offset is greater than block size");
        return -1;
    }
    for (size_t i = 0; i < pool->current_block + 1; i++)
    {
        if (pool->memory_blocks[i] == NULL)
        {
            LERR(LERR_POOL_VALIDATION, "null pointer to memory block");
            return -1;
        }
    }

    return 0;
}