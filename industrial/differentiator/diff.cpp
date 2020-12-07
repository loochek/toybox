#include <stdio.h>

#include "diff.hpp"
#include "lerror.hpp"

#define DIFF_RULE(TYPE, BODY) \
    case TYPE:                \
        BODY;                 \
        break;             

static expr_node_t *expr_diff_rec(expr_node_t *CURR_NODE, char var, node_pool_t *pool)
{
    expr_node_t *RESULT = NULL;

    switch (CURR_NODE->type)
    {
    #include "diff_rules.hpp"

    default:
        printf("Warning: differentiation rule is not present!!! Keep as is\n");
        RESULT = expr_deep_copy(CURR_NODE, pool);
        break;
    }

    return RESULT;
}

expr_node_t *expr_diff(expr_node_t *node, char var, node_pool_t *pool)
{
    LERR_RESET();
    EXPR_CHECK_RET(node, NULL);

    return expr_diff_rec(node, var, pool);
}
