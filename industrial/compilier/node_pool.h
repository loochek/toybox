#ifndef NODE_POOL_H
#define NODE_POOL_H

#include <stdlib.h>

#include "ast.h"

/**
 * Effective node allocator
 */

#define BLOCK_SIZE       10000000
#define MAX_BLOCKS_COUNT 100

typedef struct pool_node
{
    ast_node_t        payload;
    struct pool_node *next_free;
} pool_node_t;

typedef struct node_pool
{
    pool_node_t  *blocks[MAX_BLOCKS_COUNT];
    size_t        blocks_count;

    pool_node_t  *free_head;

} node_pool_t;

/**
 * LERR-affecting
 */
void node_pool_construct(node_pool_t *pool);

/**
 * LERR-affecting
 */
ast_node_t *node_pool_claim(node_pool_t *pool);

/**
 * LERR-affecting
 */
void node_pool_free(ast_node_t *ptr, node_pool_t *pool);

/**
 * LERR-affecting
 */
void node_pool_destroy(node_pool_t *pool);

#endif