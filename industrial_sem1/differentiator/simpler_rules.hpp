#define NEW_NODE(TYPE, FIRST_ARG, SECOND_ARG)           \
({                                                      \
    expr_node_t *node = node_pool_claim(pool);          \
    if (node == NULL)                                   \
        return NULL;                                    \
                                                        \
    node->type       = TYPE;                            \
    node->first_arg  = FIRST_ARG;                       \
    node->second_arg = SECOND_ARG;                      \
                                                        \
    node;                                               \
})

#define NEW_NUM(VALUE)                                  \
({                                                      \
    expr_node_t *node = node_pool_claim(pool);          \
    if (node == NULL)                                   \
        return NULL;                                    \
    node->type   = TYPE_NUM;                            \
    node->number = VALUE;                               \
                                                        \
    node;                                               \
})

#define DESTRUCT(NODE) expr_destroy(NODE, pool)

/**
 * To access current node in the rule, use CURR_NODE
 * You should put a pointer to RESULT as a result of your rule's work
 */

SIMPLER_RULE(TYPE_ADD,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number + CURR_NODE->second_arg->number);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = CURR_NODE->second_arg;

        DESTRUCT(CURR_NODE->first_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = CURR_NODE->first_arg;

        DESTRUCT(CURR_NODE->second_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_SUB,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number - CURR_NODE->second_arg->number);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NODE(TYPE_MUL, CURR_NODE->second_arg, NEW_NUM(-1));

        DESTRUCT(CURR_NODE->first_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = CURR_NODE->first_arg;

        DESTRUCT(CURR_NODE->second_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_MUL,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(CURR_NODE->first_arg->number * CURR_NODE->second_arg->number);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = CURR_NODE->second_arg;

        DESTRUCT(CURR_NODE->first_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 1)
    {
        RESULT = CURR_NODE->first_arg;

        DESTRUCT(CURR_NODE->second_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_DIV,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 1)
    {
        RESULT = CURR_NODE->first_arg;

        DESTRUCT(CURR_NODE->second_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_POW,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->second_arg->type == TYPE_NUM)
    {
        RESULT = NEW_NUM(pow(CURR_NODE->first_arg->number, CURR_NODE->second_arg->number));

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->second_arg->type == TYPE_NUM && CURR_NODE->second_arg->number == 1)
    {
        RESULT = CURR_NODE->first_arg;

        DESTRUCT(CURR_NODE->second_arg);
        node_pool_free(CURR_NODE, pool);
    }
    else if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = NEW_NUM(1);

        DESTRUCT(CURR_NODE);
    }
    else if (CURR_NODE->first_arg->type  == TYPE_NUM && CURR_NODE->first_arg->number == 0 &&
             CURR_NODE->second_arg->type == TYPE_NUM)
    {
        if (CURR_NODE->second_arg->number == 0)
            RESULT = NEW_NUM(1);
        else
            RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_SIN,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_COS,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        DESTRUCT(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_EXP,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 0)
    {
        RESULT = NEW_NUM(1);

        DESTRUCT(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})

SIMPLER_RULE(TYPE_LN,
{
    if (CURR_NODE->first_arg->type == TYPE_NUM && CURR_NODE->first_arg->number == 1)
    {
        RESULT = NEW_NUM(0);

        DESTRUCT(CURR_NODE);
    }
    else
        RESULT = CURR_NODE;
})