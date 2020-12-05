#ifndef EXPR_TREE_H
#define EXPR_TREE_H

#include <stdlib.h>
#include <stdbool.h>

/*

Mathematical expression tree

*/

// forward declaration
struct node_pool_t;

enum expr_node_type_t
{
    TYPE_NONE,
    TYPE_NUM,
    TYPE_VAR,
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_POW,
    TYPE_SIN,
    TYPE_COS,
    TYPE_LN,
    TYPE_EXP
};

struct expr_node_t
{
    expr_node_type_t type;

    expr_node_t *first_arg;
    expr_node_t *second_arg;

    union
    {
        char var;
        int  number;
    };
};

#define EXPR_CHECK_RET(tree_root, to_ret) if (expr_validate(tree_root) != 0) return to_ret;

/**
 * LERR-affecting
 */
void expr_dump(expr_node_t *tree_root);

/**
 * Shows tree with Graphvis
 * LERR-affecting
 */
void expr_visualize(expr_node_t *tree_root);

/**
 * LERR-affecting
 */
void expr_latex_dump(expr_node_t *tree_root);

/**
 * Returns 0 if tree is correct? else -1
 * LERR-affecting
 */
int expr_validate(expr_node_t *node);

/**
 * Checks is subtree a constant
 */ 
bool expr_is_constant(expr_node_t *node);

/**
 * LERR-affecting
 */
expr_node_t *expr_deep_copy(expr_node_t *node, node_pool_t *pool);

void expr_destroy(expr_node_t *node, node_pool_t *pool);

#endif