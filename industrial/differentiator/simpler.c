#include "simpler.h"
#include "lerror.h"

#define SIMPLER_RULE(TYPE, BODY) \
    case TYPE:                   \
        BODY;                    \
        break;                   

void expr_simplify(expr_node_t **node_ptr)
{
    if (node_ptr == NULL)
    {
        LERR(LERR_BAD_ARG, "null pointer passed");
        return;
    }

    expr_node_t *CURR_NODE = *node_ptr;

    if (CURR_NODE == NULL)
        return;

    expr_simplify(&CURR_NODE->first_arg);
    expr_simplify(&CURR_NODE->second_arg);

    expr_node_t *RESULT = NULL;

    switch (CURR_NODE->type)
    {
    #include "simpler_rules.h"

    default:
        RESULT = CURR_NODE;
        break;
    }

    *node_ptr = RESULT;
}