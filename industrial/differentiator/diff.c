#include <stdio.h>
#include "diff.h"

#define DIFF_RULE(TYPE, BODY) \
    case TYPE:                \
        BODY;                 \
        break;                \

expr_node_t *expr_diff(expr_node_t *CURR_NODE)
{
    if (CURR_NODE == NULL)
        return NULL;

    expr_node_t *RESULT = calloc(1, sizeof(expr_node_t));

    switch (CURR_NODE->type)
    {
    #include "diff_rules.h"

    default:
        printf("expr_diff: Unknown type!\n");
        break;
    }

    return RESULT;
}