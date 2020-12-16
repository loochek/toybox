#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "string_view.h"

/**
 * Abstract syntax tree and generic methods
 */

// forward declaration
struct node_pool;

typedef enum ast_node_type
{
    AST_DUMMY,

    AST_NUMBER,
    AST_IDENTIFIER,
    AST_OPER_ADD,
    AST_OPER_SUB,
    AST_OPER_MUL,
    AST_OPER_DIV,
    AST_OPER_MOD,
    AST_OPER_ASSIGN,
    AST_OPER_EQUAL,
    AST_OPER_NEQUAL,
    AST_OPER_LESS,
    AST_OPER_MORE,
    AST_OPER_ELESS,
    AST_OPER_EMORE,
    AST_OPER_CALL,

    AST_IF,
    AST_WHILE,
    AST_RETURN,

    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_FUNC_HEAD,
    AST_EXPR_STMT,

    AST_COMPOUND
} ast_node_type_t;

typedef struct ast_node
{
    ast_node_type_t type;

    struct ast_node *left_branch;
    struct ast_node *right_branch;

    union
    {
        int           number;
        string_view_t ident;
    };
} ast_node_t;

void ast_visualize(ast_node_t *tree_root);
void ast_destroy  (ast_node_t *node, struct node_pool *pool);

#endif