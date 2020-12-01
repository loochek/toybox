#include "stdbool.h"

#include "lerror.h"
#include "parser.h"
#include "file_utils.h"

/*

Each function presents one grammar rule:
(G is main rule)

G ::= E'\0'
E ::= M{[+-]M}*
M ::= PW{[*\]PW}*
PW ::= P{'^'P}*
P ::= ['sin'|'cos'|'ln'|'exp']? '('E')' | N | V
N ::= [0-9]+
V ::= [a-z]

*/

typedef struct parser_state_t
{
    const char *text;
    size_t      curr_pos;

} parser_state_t;

static expr_node_t *grammar_g (parser_state_t *state);
static expr_node_t *grammar_e (parser_state_t *state);
static expr_node_t *grammar_m (parser_state_t *state);
static expr_node_t *grammar_pw(parser_state_t *state);
static expr_node_t *grammar_p (parser_state_t *state);
static expr_node_t *grammar_n (parser_state_t *state);
static expr_node_t *grammar_v (parser_state_t *state);


expr_node_t *expr_load_from_file(const char *file_name)
{
    char *buffer = NULL;
    int buffer_size = create_buffer_from_file(file_name, &buffer);
    if (buffer_size == -1)
        return NULL;

    if (buffer[buffer_size - 1] == '\n')
        buffer[buffer_size - 1] = '\0';

    parser_state_t state     = { buffer, 0 };
    expr_node_t   *tree_root = grammar_g(&state);
    free(buffer);

    return tree_root;
}

static expr_node_t *grammar_g(parser_state_t *state)
{
    expr_node_t *node = grammar_e(state);
    if (node == NULL)
        return NULL;

    if (state->text[state->curr_pos] != '\0')
    {
        LERR(LERR_EXPR_PARSING, "no null-terminator at the end");
        return NULL;
    }
    else
        state->curr_pos++;

    return node;
}

static expr_node_t *grammar_n(parser_state_t *state)
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

static expr_node_t *grammar_v(parser_state_t *state)
{
    if (state->text[state->curr_pos] >= 'a' && state->text[state->curr_pos] <= 'z')
    {
        expr_node_t *node = calloc(1, sizeof(expr_node_t));
        node->type  = TYPE_VAR;
        node->var = state->text[state->curr_pos];
        state->curr_pos++;

        return node;
    }

    LERR(LERR_EXPR_PARSING, "empty variable");
    return NULL;
}

static expr_node_t *grammar_p(parser_state_t *state)
{
    expr_node_type_t func_type = TYPE_NONE;

    const char      *keywords  [] = { "sin"   , "cos"   , "ln"   , "exp"    };
    expr_node_type_t word_match[] = { TYPE_SIN, TYPE_COS, TYPE_LN, TYPE_EXP };

    for (size_t i = 0; i < 4; i++)
    {
        bool match = true;
        size_t curr_pos = state->curr_pos;
        for (const char *c = keywords[i]; *c != '\0'; c++)
        {
            if (*c != state->text[curr_pos])
            {
                match = false;
                break;
            }
            curr_pos++;
        }
        if (match)
        {
            func_type = word_match[i];
            state->curr_pos += strlen(keywords[i]);
            break;
        }
    }

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

        if (func_type == TYPE_NONE)
            return node;

        expr_node_t *func_node = calloc(1, sizeof(expr_node_t));
        func_node->type       = func_type;
        func_node->first_arg  = node;

        return func_node;
    }

    if (func_type != TYPE_NONE)
    {
        LERR(LERR_EXPR_PARSING, "function requires parenthesized expression");
        return NULL;
    }

    expr_node_t *node = grammar_n(state);
    if (node == NULL)
        return grammar_v(state);
    else
        return node;
}

static expr_node_t *grammar_pw(parser_state_t *state)
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

static expr_node_t *grammar_m(parser_state_t *state)
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

static expr_node_t *grammar_e(parser_state_t *state)
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