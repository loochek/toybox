#include "search_tree.h"

// these functions throws LERR
tree_node_t *akinator_add    (tree_node_t *node, memory_pool_t *pool);
tree_node_t *akinator_guess  (tree_node_t *node, memory_pool_t *pool);
void         akinator_find   (tree_node_t *node);
void         akinator_compare(tree_node_t *node);