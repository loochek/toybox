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

#define DIFF(NODE)                                      \
({                                                      \
    expr_node_t *tmp = expr_diff_rec(NODE, var, pool);  \
    if (tmp == NULL)                                    \
        return NULL;                                    \
                                                        \
    tmp;                                                \
})

#define COPY(NODE)                                      \
({                                                      \
    expr_node_t *tmp = expr_deep_copy(NODE, pool);      \
    if (tmp == NULL)                                    \
        return NULL;                                    \
                                                        \
    tmp;                                                \
})

/**
 * To access current node in the rule, use CURR_NODE
 * You should put a pointer to RESULT as a result of your rule's work
 */

DIFF_RULE(TYPE_NUM,
{
    RESULT = NEW_NUM(0);
})

DIFF_RULE(TYPE_VAR,
{
    if (CURR_NODE->var == var)
        RESULT = NEW_NUM(1);
    else
        RESULT = NEW_NUM(0);
})

DIFF_RULE(TYPE_ADD,
{
    RESULT = NEW_NODE(TYPE_ADD,
                      DIFF(CURR_NODE->first_arg),
                      DIFF(CURR_NODE->second_arg));
})

DIFF_RULE(TYPE_SUB,
{
    RESULT = NEW_NODE(TYPE_SUB,
                      DIFF(CURR_NODE->first_arg),
                      DIFF(CURR_NODE->second_arg));
})

DIFF_RULE(TYPE_MUL,
{
    expr_node_t *first_mul  = NEW_NODE(TYPE_MUL,
                                        DIFF(CURR_NODE->first_arg),
                                        COPY(CURR_NODE->second_arg));

    expr_node_t *second_mul = NEW_NODE(TYPE_MUL,
                                       COPY(CURR_NODE->first_arg),
                                       DIFF(CURR_NODE->second_arg));

    RESULT = NEW_NODE(TYPE_ADD, first_mul, second_mul);
})

DIFF_RULE(TYPE_DIV,
{
    expr_node_t *first_mul  = NEW_NODE(TYPE_MUL,
                                       DIFF(CURR_NODE->first_arg),
                                       COPY(CURR_NODE->second_arg));

    expr_node_t *second_mul = NEW_NODE(TYPE_MUL,
                                       COPY(CURR_NODE->first_arg),
                                       DIFF(CURR_NODE->second_arg));
    
    expr_node_t *divident   = NEW_NODE(TYPE_SUB, first_mul, second_mul);

    expr_node_t *divider    = NEW_NODE(TYPE_POW,
                                       COPY(CURR_NODE->second_arg),
                                       NEW_NUM(2));

    RESULT = NEW_NODE(TYPE_DIV, divident, divider);
})

DIFF_RULE(TYPE_POW, 
{
    bool first_arg_const  = expr_is_constant(CURR_NODE->first_arg);
    bool second_arg_const = expr_is_constant(CURR_NODE->second_arg);

    if (first_arg_const && second_arg_const)
    {
        // just pow

        RESULT = NEW_NODE(TYPE_POW,
                          COPY(CURR_NODE->first_arg),
                          COPY(CURR_NODE->second_arg));
    }
    else if (second_arg_const)
    {
        // x^a

        expr_node_t *first_mul = NEW_NODE(TYPE_MUL,
                                          COPY(CURR_NODE->second_arg),
                                          DIFF(CURR_NODE->first_arg));
        
        expr_node_t *pow_sub   = NEW_NODE(TYPE_SUB,
                                          COPY(CURR_NODE->second_arg),
                                          NEW_NUM(1));

        expr_node_t *pow_node  = NEW_NODE(TYPE_POW,
                                          COPY(CURR_NODE->first_arg),
                                          pow_sub);
        
        RESULT = NEW_NODE(TYPE_MUL, pow_node, first_mul);
    }
    else if (first_arg_const)
    {
        // a^x

        expr_node_t *ln_node  = NEW_NODE(TYPE_LN,
                                         COPY(CURR_NODE->second_arg),
                                         NULL);

        expr_node_t *mul_node = NEW_NODE(TYPE_MUL,
                                         ln_node,
                                         DIFF(CURR_NODE->second_arg));

        RESULT = NEW_NODE(TYPE_MUL,
                          COPY(CURR_NODE),
                          mul_node);
    }
    else
    {
        printf("Warning: can't differ x^x! Keep as is\n");
        RESULT = COPY(CURR_NODE);
    }
        
})

DIFF_RULE(TYPE_SIN, 
{
    expr_node_t *cos_node = NEW_NODE(TYPE_COS, COPY(CURR_NODE->first_arg), NULL);

    RESULT = NEW_NODE(TYPE_MUL, cos_node, DIFF(CURR_NODE->first_arg));
})

DIFF_RULE(TYPE_COS, 
{
    expr_node_t *sin_node = NEW_NODE(TYPE_SIN, COPY(CURR_NODE->first_arg), NULL);
    expr_node_t *mul_node = NEW_NODE(TYPE_MUL, DIFF(CURR_NODE->first_arg), NEW_NUM(-1));

    RESULT = NEW_NODE(TYPE_MUL, sin_node, mul_node);
})

DIFF_RULE(TYPE_LN,
{
    RESULT = NEW_NODE(TYPE_DIV, DIFF(CURR_NODE->first_arg), COPY(CURR_NODE->first_arg));
})

DIFF_RULE(TYPE_EXP,
{
    RESULT = NEW_NODE(TYPE_MUL, COPY(CURR_NODE), DIFF(CURR_NODE->first_arg));
})