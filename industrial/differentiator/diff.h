#include "expr_tree.h"

/**
 * Differentiation methods
 */

/**
 * Returs differentiated by var COPY! of tree
 * LERR-affecting
 */
expr_node_t *expr_diff(expr_node_t *node, char var);