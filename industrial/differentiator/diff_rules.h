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

DIFF_RULE(TYPE_NUM,
{
    RESULT = NEW_NUM(0);
})

DIFF_RULE(TYPE_VAR,
{
    RESULT = NEW_NUM(1);
})

DIFF_RULE(TYPE_ADD,
{
    RESULT = NEW_NODE(TYPE_ADD,
                      expr_diff(CURR_NODE->first_arg),
                      expr_diff(CURR_NODE->second_arg));
})

DIFF_RULE(TYPE_SUB,
{
    RESULT = NEW_NODE(TYPE_SUB,
                      expr_diff(CURR_NODE->first_arg),
                      expr_diff(CURR_NODE->second_arg));
})

DIFF_RULE(TYPE_MUL,
{
    expr_node_t *first_mul  = NEW_NODE(TYPE_MUL,
                                        expr_diff     (CURR_NODE->first_arg),
                                        expr_deep_copy(CURR_NODE->second_arg));

    expr_node_t *second_mul = NEW_NODE(TYPE_MUL,
                                       expr_deep_copy(CURR_NODE->first_arg),
                                       expr_diff     (CURR_NODE->second_arg));

    RESULT = NEW_NODE(TYPE_ADD, first_mul, second_mul);
})

DIFF_RULE(TYPE_DIV,
{
    expr_node_t *first_mul  = NEW_NODE(TYPE_MUL,
                                       expr_diff     (CURR_NODE->first_arg),
                                       expr_deep_copy(CURR_NODE->second_arg));

    expr_node_t *second_mul = NEW_NODE(TYPE_MUL,
                                       expr_deep_copy(CURR_NODE->first_arg),
                                       expr_diff     (CURR_NODE->second_arg));
    
    expr_node_t *divident   = NEW_NODE(TYPE_SUB, first_mul, second_mul);

    expr_node_t *divider    = NEW_NODE(TYPE_POW,
                                       expr_deep_copy(CURR_NODE->second_arg),
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
                          expr_deep_copy(CURR_NODE->first_arg),
                          expr_deep_copy(CURR_NODE->second_arg));
    }
    else if (second_arg_const)
    {
        // x^a

        expr_node_t *first_mul = NEW_NODE(TYPE_MUL,
                                          expr_deep_copy(CURR_NODE->second_arg),
                                          expr_diff     (CURR_NODE->first_arg));
        
        expr_node_t *pow_sub   = NEW_NODE(TYPE_SUB,
                                          expr_deep_copy(CURR_NODE->second_arg),
                                          NEW_NUM(1));

        expr_node_t *pow_node  = NEW_NODE(TYPE_POW,
                                          expr_deep_copy(CURR_NODE->first_arg),
                                          pow_sub);
        
        RESULT = NEW_NODE(TYPE_MUL, pow_node, first_mul);
    }
    else if (first_arg_const)
    {
        // a^x

        expr_node_t *ln_node  = NEW_NODE(TYPE_LN,
                                         expr_deep_copy(CURR_NODE->second_arg),
                                         NULL);

        expr_node_t *mul_node = NEW_NODE(TYPE_MUL,
                                         ln_node,
                                         expr_diff(CURR_NODE->second_arg));

        RESULT = NEW_NODE(TYPE_MUL,
                          expr_deep_copy(CURR_NODE),
                          mul_node);
    }
    else
        printf("Can't differ x^x!\n");
})