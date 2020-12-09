#ifndef AST_H
#define AST_H

#include <stdlib.h>

/**
 * Abstract syntax tree and generic methods
 */

typedef enum ast_node_type
{
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
    AST_OPER_CALL,
    AST_COMPOUND,
    AST_VAR_DECL,
    AST_FUNC_DECL
} ast_node_type_t;

typedef struct ast_node
{
    ast_node_type_t type;

    struct ast_node *left_branch;
    struct ast_node *right_branch;

    int number;

    const char *ident;
    size_t      ident_length;

} ast_node_t;

#endif