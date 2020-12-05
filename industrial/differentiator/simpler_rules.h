#define NEW_NODE(TYPE, FIRST_ARG, SECOND_ARG)           \
({                                                      \
    expr_node_t *node = calloc(1, sizeof(expr_node_t)); \
    node->type       = TYPE;                            \
    node->first_arg  = FIRST_ARG;                       \
    node->second_arg = SECOND_ARG;                      \
                                                        \
    node;                                               \
})

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
        RESULT = NEW_NODE(TYPE_MUL, CURR_NODE->second_arg, NEW_NUM(-1));

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

SIMPLER_RULE(TYPE_DIV,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
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

SIMPLER_RULE(TYPE_POW,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(pow(CURR_NODE->first_arg->number, CURR_NODE->second_arg->number));

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 1)
    {
        RESULT = CURR_NODE->first_arg;

        expr_destroy(CURR_NODE->second_arg);
        free(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = NEW_NUM(1);

        expr_destroy(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type  == TYPE_NUM && CURR_NODE->first_arg->number == 0 &&
             CURR_NODE->second_arg->type == TYPE_NUM)
    {
        if (CURR_NODE->second_arg->number == 0)
            RESULT = NEW_NUM(1);
        else
            RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_SIN,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_COS,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        expr_destroy(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_EXP,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        expr_destroy(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_LN,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = NEW_NUM(0);

        expr_destroy(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})