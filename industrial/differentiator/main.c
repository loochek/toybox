#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "lerror.h"

/*

PROTOTYPE
(but quite great and clear)

*/

typedef enum
{
    TYPE_NUM,
    TYPE_VAR,
    TYPE_ADD = '+',
    TYPE_SUB = '-',
    TYPE_MUL = '*',
    TYPE_DIV = '/',
    TYPE_POW = '^'
} expr_node_type_t;

typedef struct expr_node_t
{
    expr_node_type_t type;

    struct expr_node_t *first_arg;
    struct expr_node_t *second_arg;

    union
    {
        char var;
        int number;
    };

} expr_node_t;


/*

Grammatik:

G ::= E'\0'
E ::= M{[+-]M}*
M ::= PW{[*\]PW}*
PW ::= P{'^'P}*
P ::= '('E')' | N
N ::= [0-9]+

*/

typedef struct parser_state_t
{
    const char *text;
    size_t      curr_pos;

} parser_state_t;

expr_node_t *grammar_e(parser_state_t *state);

expr_node_t *grammar_n(parser_state_t *state)
{
    int val = 0;
    bool empty = true;
    while (state->text[state->curr_pos] >= '0' && state->text[state->curr_pos] <= '9')
    {
        empty = false;
        val = val * 10 + state->text[state->curr_pos] - '0';
        state->curr_pos++;
    }
    if (empty)
    {
        LERR(LERR_EXPR_PARSING, "empty number");
        return NULL;
    }

    expr_node_t *node = calloc(1, sizeof(expr_node_t));
    node->type  = TYPE_NUM;
    node->number = val;

    return node;
}

expr_node_t *grammar_p(parser_state_t *state)
{
    if (state->text[state->curr_pos] == '(')
    {
        state->curr_pos++;
        expr_node_t *node = grammar_e(state);
        if (node == NULL)
            return NULL;

        if (state->text[state->curr_pos] != ')')
        {
            LERR(LERR_EXPR_PARSING, "no closing parenthesis");
            return NULL;
        }
        else
            state->curr_pos++;

        return node;
    }
    return grammar_n(state);
}

expr_node_t *grammar_pw(parser_state_t *state)
{
    expr_node_t *subtree_root = grammar_p(state);
    if (subtree_root == NULL)
        return NULL;

    while (state->text[state->curr_pos] == '^')
    {
        state->curr_pos++;

        expr_node_t *second_arg = grammar_p(state);
        if (second_arg == NULL)
            return NULL;
        
        expr_node_t *oper_node = calloc(1, sizeof(expr_node_t));
        oper_node->type       = TYPE_POW;
        oper_node->first_arg  = subtree_root;
        oper_node->second_arg = second_arg;

        subtree_root = oper_node;
    }

    return subtree_root;
}

expr_node_t *grammar_m(parser_state_t *state)
{
    expr_node_t *subtree_root = grammar_pw(state);
    if (subtree_root == NULL)
        return NULL;

    while (state->text[state->curr_pos] == '*' || state->text[state->curr_pos] == '/')
    {
        char op = state->text[state->curr_pos];
        state->curr_pos++;

        expr_node_t *second_arg = grammar_pw(state);
        if (second_arg == NULL)
            return NULL;
        
        expr_node_t *oper_node = calloc(1, sizeof(expr_node_t));
        oper_node->first_arg  = subtree_root;
        oper_node->second_arg = second_arg;

        if (op == '*')
            oper_node->type = TYPE_MUL;
        else
            oper_node->type = TYPE_DIV;

        subtree_root = oper_node;
    }

    return subtree_root;
}

expr_node_t *grammar_e(parser_state_t *state)
{
    expr_node_t *subtree_root = grammar_m(state);
    if (subtree_root == NULL)
        return NULL;

    while (state->text[state->curr_pos] == '+' || state->text[state->curr_pos] == '-')
    {
        char op = state->text[state->curr_pos];
        state->curr_pos++;

        expr_node_t *second_arg = grammar_m(state);
        if (second_arg == NULL)
            return NULL;
        
        expr_node_t *oper_node = calloc(1, sizeof(expr_node_t));
        oper_node->first_arg  = subtree_root;
        oper_node->second_arg = second_arg;

        if (op == '+')
            oper_node->type = TYPE_ADD;
        else
            oper_node->type = TYPE_SUB;

        subtree_root = oper_node;
    }

    return subtree_root;
}

expr_node_t *grammar_g(parser_state_t *state)
{
    expr_node_t *node = grammar_e(state);
    if (node == NULL)
        return NULL;

    if (state->text[state->curr_pos] != '\0')
    {
        LERR(LERR_EXPR_PARSING, "no null-terminator at the end");
    }
    else
        state->curr_pos++;

    return node;
}

void tree_visualize_rec(expr_node_t *node, FILE *file, size_t node_id)
{
    if (node->type == TYPE_NUM)
    {
        fprintf(file, "%zu [label=\"%d\", color=green];\n", node_id, node->number);
        return;
    }

    tree_visualize_rec(node->first_arg , file, node_id * 2);
    tree_visualize_rec(node->second_arg, file, node_id * 2 + 1);

    fprintf(file, "%zu [label=\"%c\"];\n", node_id, node->type);
    fprintf(file, "%zu->%zu;\n", node_id, node_id * 2);
    fprintf(file, "%zu->%zu;\n", node_id, node_id * 2 + 1);
}

void tree_visualize(expr_node_t *tree_root)
{
    FILE *file = fopen("tree.dot", "w");
    fprintf(file, "digraph\n{\n");

    tree_visualize_rec(tree_root, file, 1);

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tsvg tree.dot > tree.svg");
    system("firefox tree.svg");
}

int main()
{
    const char *src = "123+";
    parser_state_t state = { src, 0 };
    expr_node_t *expr_tree = grammar_g(&state);
    if (expr_tree == NULL)
    {
        printf("Error: %s\n", __lerr_str);
        return 0;
    }
    tree_visualize(expr_tree);

    return 0;
}