#include "search_tree.h"

/**
 * Implementations of Akinator game modes
 */

/**
 * Returns modified tree root or NULL if error occured
 * LERR-affecting
 */
tree_node_t *akinator_add(tree_node_t *tree_root, memory_pool_t *pool);

/**
 * Returns modified tree root or NULL if error occured
 * LERR-affecting
 */
tree_node_t *akinator_guess(tree_node_t *tree_root, memory_pool_t *pool);

/**
 * LERR-affecting
 */
void akinator_find(tree_node_t *tree_root);

/**
 * LERR-affecting
 */
void akinator_compare(tree_node_t *tree_root);

void akinator_tell(const char *sentence);
void read_line    (char *buffer, size_t max_length);