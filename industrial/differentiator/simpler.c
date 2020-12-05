#include "simpler.h"
#include "lerror.h"

#define SIMPLER_RULE(TYPE, BODY) \
    case TYPE:                   \
        BODY;                    \
        break;                   

static int pow(int a, int n)
{
	int res = 1;
	while (n)
    {
		if (n & 1)
			res *= a;
		a *= a;
		n >>= 1;
	}
	return res;
}

static expr_node_t *expr_simplify_rec(expr_node_t *CURR_NODE)
{
    if (CURR_NODE == NULL)
        return NULL;

    CURR_NODE->first_arg  = expr_simplify_rec(CURR_NODE->first_arg);
    CURR_NODE->second_arg = expr_simplify_rec(CURR_NODE->second_arg);

    expr_node_t *RESULT = NULL;

    switch (CURR_NODE->type)
    {
    #include "simpler_rules.h"

    default:
        RESULT = CURR_NODE;
        break;
    }

    return RESULT;
}

void expr_simplify(expr_node_t **node_ptr)
{
    if (node_ptr == NULL)
    {
        LERR(LERR_BAD_ARG, "null pointer passed");
        return;
    }

    EXPR_CHECK_RET(*node_ptr,);

    *node_ptr = expr_simplify_rec(*node_ptr);
}