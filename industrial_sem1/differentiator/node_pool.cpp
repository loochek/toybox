#include "node_pool.hpp"
#include "lerror.hpp"

static void node_pool_expand   (node_pool_t *pool);
static void node_pool_add_block(node_pool_t *pool);
static int  node_pool_validate (node_pool_t *pool);

#define POOL_CHECK_RET(pool, ret_val)  \
{                                      \
    if (node_pool_validate(pool) != 0) \
        return ret_val;                \
}
    

void node_pool_construct(node_pool_t *pool)
{
    LERR_RESET();

    if (pool == NULL)
    {
        LERR(LERR_BAD_ARG, "null pointer passed");
        return;
    }
    
    *pool = (node_pool_t){};

    node_pool_add_block(pool);
    if (LERR_PRESENT())
        return;

    pool->free_head = &pool->blocks[0][BLOCK_SIZE - 1];

    POOL_CHECK_RET(pool,)
}

expr_node_t *node_pool_claim(node_pool_t *pool)
{
    POOL_CHECK_RET(pool, NULL)

    if (pool->free_head == NULL)
    {
        node_pool_expand(pool);
        if (LERR_PRESENT())
            return NULL;
    }

    expr_node_t *to_ret = &pool->free_head->payload;

    pool->free_head = pool->free_head->next_free;

    POOL_CHECK_RET(pool, NULL)

    return to_ret;
}

void node_pool_free(expr_node_t *ptr, node_pool_t *pool)
{
    POOL_CHECK_RET(pool,)

    // little hack (according to pool_node_t internal representation)
    pool_node_t *pool_node = (pool_node_t*)ptr;

    pool_node->payload   = (expr_node_t){};
    pool_node->next_free = pool->free_head;

    pool->free_head = pool_node;

    POOL_CHECK_RET(pool,)
}

void node_pool_destroy(node_pool_t *pool)
{
    POOL_CHECK_RET(pool,)
    
    for (size_t i = 0; i < pool->blocks_count; i++)
        free(pool->blocks[i]);
}

static void node_pool_expand(node_pool_t *pool)
{
    LERR_RESET();
    POOL_CHECK_RET(pool,)

    node_pool_add_block(pool);
    if (LERR_PRESENT())
        return;

    if (pool->free_head != NULL)
    {
        pool->blocks[pool->blocks_count - 2][0].next_free =
                                                &pool->blocks[pool->blocks_count - 1][BLOCK_SIZE - 1];
    }
    else
        pool->free_head = &pool->blocks[pool->blocks_count - 1][BLOCK_SIZE - 1];

    POOL_CHECK_RET(pool,)
}

static void node_pool_add_block(node_pool_t *pool)
{
    LERR_RESET();
    POOL_CHECK_RET(pool,)

    if (pool->blocks_count == MAX_BLOCKS_COUNT)
    {
        LERR(LERR_ALLOC, "out of memory");
        return;
    }

    pool->blocks[pool->blocks_count] = (pool_node_t*)calloc(BLOCK_SIZE, sizeof(pool_node_t));
    if (pool->blocks[pool->blocks_count] == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory");
        return;
    }

    pool->blocks[pool->blocks_count][0].next_free = NULL;
    for (size_t i = 1; i < BLOCK_SIZE; i++)
        pool->blocks[pool->blocks_count][i].next_free = &pool->blocks[pool->blocks_count][i - 1];

    pool->blocks_count++;

    POOL_CHECK_RET(pool,)
}

static int node_pool_validate(node_pool_t *pool)
{
    LERR_RESET();

    if (pool == NULL)
    {
        LERR(LERR_POOL_VALIDATION, "null pointer passed");
        return -1;
    }

    if (pool->blocks_count > MAX_BLOCKS_COUNT)
    {
        LERR(LERR_POOL_VALIDATION, "blocks_count greater than MAX_BLOCKS_COUNT");
        return -1;
    }

    for (size_t i = 0; i < pool->blocks_count; i++)
    {
        if (pool->blocks[i] == NULL)
        {
            LERR(LERR_POOL_VALIDATION, "bad block");
            return -1;
        }
    }

    return 0;
}