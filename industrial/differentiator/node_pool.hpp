#ifndef NODE_POOL_H
#define NODE_POOL_H

#include <stdlib.h>

#include "expr_tree.hpp"

/**
 * Effective node allocator
 */

const int BLOCK_SIZE       = 10000000;
const int MAX_BLOCKS_COUNT = 100;

struct pool_node_t
{
    expr_node_t  payload;
    pool_node_t *next_free;
};

struct node_pool_t
{
    pool_node_t  *blocks[MAX_BLOCKS_COUNT];
    size_t        blocks_count;

    pool_node_t  *free_head;

};

/**
 * LERR-affecting
 */
void node_pool_construct(node_pool_t *pool);

/**
 * LERR-affecting
 */
expr_node_t *node_pool_claim(node_pool_t *pool);

/**
 * LERR-affecting
 */
void node_pool_free(expr_node_t *ptr, node_pool_t *pool);

/**
 * LERR-affecting
 */
void node_pool_destroy(node_pool_t *pool);

#endif