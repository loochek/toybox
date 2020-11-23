#include "search_tree.h"

// throws LERR
tree_node_t *akinator_add  (tree_node_t *node, memory_pool_t *pool);
// throws LERR
void         akinator_guess(tree_node_t *node, memory_pool_t *pool);

// throws LERR
void         akinator_find (tree_node_t *node);