#include <stdio.h>

#include "diff.hpp"
#include "lerror.hpp"
#include "simpler.hpp"

const int PHRASES_CNT = 3;

const char *phrases[] = {"Based on the previously calculated derivatives, we can say that the derivative of\n",
                         "Using the greatest achievements of mathematics and previously proven facts, we can say that the derivative of\n",
                         "Using our previous calculations, it is easy to understand that the result of differentiation of\n"
                        };


#define DIFF_RULE(TYPE, BODY) \
    case TYPE:                \
        BODY;                 \
        break;             

static expr_node_t *expr_diff_rec(expr_node_t *CURR_NODE, char VAR, node_pool_t *pool, FILE *file)
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

    if (file != NULL)
    {
        fprintf(file, phrases[rand() % PHRASES_CNT]);
        expr_latex_put(CURR_NODE, file);
        fprintf(file, "is equals to\n");

        expr_simplify(&RESULT, pool);
        if (LERR_PRESENT())
            return NULL;

        expr_latex_put(RESULT, file);
    }

    return RESULT;
}

expr_node_t *expr_diff(expr_node_t *node, char var, node_pool_t *pool)
{
    LERR_RESET();
    EXPR_CHECK_RET(node, NULL);

    return expr_diff_rec(node, var, pool, NULL);
}

expr_node_t *expr_diff_output(expr_node_t *node, char var, node_pool_t *pool, FILE *file)
{
    LERR_RESET();
    EXPR_CHECK_RET(node, NULL);
    
    fprintf(file, "So, let's get started on the greatest scientific study of the last millennium.\n"
                  "Source expression is\n");
    expr_latex_put(node, file);

    fprintf(file, "Our greatest goal is to differentiate it by %c. Let's start!\n", var);

    expr_node_t *res = expr_diff_rec(node, var, pool, file);

    fprintf(file, "Q.E.D.\n");
    return res;
}