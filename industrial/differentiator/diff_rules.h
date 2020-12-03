

DIFF_RULE(TYPE_NUM,
{
    RESULT->type   = TYPE_NUM;
    RESULT->number = 0;
})

DIFF_RULE(TYPE_VAR,
{
    RESULT->type   = TYPE_NUM;
    RESULT->number = 1;
})

DIFF_RULE(TYPE_ADD,
{
    RESULT->type       = TYPE_ADD;
    RESULT->first_arg  = expr_diff(CURR_NODE->first_arg);
    RESULT->second_arg = expr_diff(CURR_NODE->second_arg);
})

DIFF_RULE(TYPE_MUL,
{
    expr_node_t *first_node  = calloc(1, sizeof(expr_node_t));
    expr_node_t *second_node = calloc(1, sizeof(expr_node_t));

    first_node->type  = TYPE_MUL;
    first_node->first_arg  = expr_diff(CURR_NODE->first_arg);
    first_node->second_arg = expr_deep_copy(CURR_NODE->second_arg);

    second_node->type = TYPE_MUL;
    second_node->first_arg  = expr_deep_copy(CURR_NODE->first_arg);
    second_node->second_arg = expr_diff(CURR_NODE->second_arg);

    RESULT->type = TYPE_ADD;
    RESULT->first_arg  = first_node;
    RESULT->second_arg = second_node;
  
})