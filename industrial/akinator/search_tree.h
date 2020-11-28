#ifndef SEARCH_TREE_H
#define SEARCH_TREE_H

/**
 * Binary decision tree with basic interface
 */

#include "allocator.h"
#include "stack/stack_common.h"

#define MAX_NODE_NAME_LENGTH 100

typedef struct tree_node_t
{
    char *node_name;
    
    struct tree_node_t *yes_branch;
    struct tree_node_t *no_branch;
} tree_node_t;

#define TYPE node
#define elem_t tree_node_t*
#include "stack/stack.h"
#undef elem_t
#undef TYPE

/**
 * Method messes passed buffer. Created tree depends on the buffer.
 * So keep buffer while using the tree
 * LERR-affecting
 */
tree_node_t *tree_create_from_buffer(char *buf, size_t buf_size, memory_pool_t *pool);

/**
 * Save tree to file
 * LERR-affecting
 */
void tree_dump(tree_node_t *tree_root, const char *file_name);

/**
 * Draw tree with Graphvis
 * LERR-affecting
 */
void tree_visualize(tree_node_t *tree_root);

/**
 * Returns 0 if passed tree is correct, else -1
 * LERR-affecting
 */
int tree_validate(tree_node_t *tree_root);

/**
 * Returns a pointer to the found node or NULL if node with passed node_name wasn't found in the tree
 * You also need to provide a stack for the path to the found node
 * LERR-affecting
 */
tree_node_t* tree_search(tree_node_t* tree_root, const char *thing, my_stack_node *stack);

#define TREE_CHECK_RET(tree, to_ret) \
{                                \
    tree_validate(tree);         \
    if (LERR_PRESENT())          \
        return to_ret;           \
}

#endif