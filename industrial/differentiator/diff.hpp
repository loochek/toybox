#include <stdio.h>

#include "expr_tree.hpp"
#include "node_pool.hpp"

/**
 * Differentiation methods
 */

/**
 * Returs differentiated by var COPY! of tree
 * LERR-affecting
 */
expr_node_t *expr_diff(expr_node_t *node, char var, node_pool_t *pool);

/**
 * Returs differentiated by var COPY! of tree and describes differentiation process to file
 * LERR-affecting
 */
expr_node_t *expr_diff_output(expr_node_t *node, char var, node_pool_t *pool, FILE *file);