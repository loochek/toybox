#define NEW_NUM(VALUE)                                  \
({                                                      \
    expr_node_t *node = calloc(1, sizeof(expr_node_t)); \
    node->type   = TYPE_NUM;                            \
    node->number = VALUE;                               \
                                                        \
    node;                                               \
})

/**
 * To access current node in the rule, use CURR_NODE
 * You should put a pointer to RESULT as a result of your rule's work
 */

SIMPLER_RULE(TYPE_ADD,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number + CURR_NODE->second_arg->number);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = CURR_NODE->second_arg;

        expr_destroy(CURR_NODE->first_arg);
        free(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = CURR_NODE->first_arg;

        expr_destroy(CURR_NODE->second_arg);
        free(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_SUB,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number - CURR_NODE->second_arg->number);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = CURR_NODE->second_arg;

        expr_destroy(CURR_NODE->first_arg);
        free(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = CURR_NODE->first_arg;

        expr_destroy(CURR_NODE->second_arg);
        free(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_MUL,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number * CURR_NODE->second_arg->number);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = CURR_NODE->second_arg;

        expr_destroy(CURR_NODE->first_arg);
        free(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 1)
    {
        RESULT = CURR_NODE->first_arg;

        expr_destroy(CURR_NODE->second_arg);
        free(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})