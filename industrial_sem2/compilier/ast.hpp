#ifndef AST_HPP
#define AST_HPP

#include <stdlib.h>

#include "utils/string_view.hpp"
#include "utils/memory_pool.hpp"

/**
 * Abstract syntax tree definition and generic methods
 */

enum ast_node_type_t
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

    AST_CALL,

    AST_IF,
    AST_WHILE,
    AST_RETURN,

    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_FUNC_HEAD,
    AST_EXPR_STMT,
    AST_SCOPE,

    AST_COMPOUND
};

struct ast_node_t
{
    ast_node_type_t type;

    ast_node_t *left_branch;
    ast_node_t *right_branch;

    union
    {
        int64_t number;
        string_view_t ident;
    };

    int row;
    int col;
    
    int regs_used;
};

/**
 * Graphically represents an AST
 * \param \c tree_root AST root node
 */
void ast_visualize(ast_node_t *tree_root);

/**
 * Optimizes an AST by constant folding
 * 
 * \param \c node AST node
 * \param \c node_out Where to write pointer to optimized node
 * \param \c pool Node pool
 */
lstatus_t ast_optimize(ast_node_t *node, ast_node_t **node_out, memory_pool_t<ast_node_t> *pool);

/**
 * Recursively destroys the AST
 * \param \c node AST node
 * \param \c pool Node pool
 */
lstatus_t ast_destroy(ast_node_t *node, memory_pool_t<ast_node_t> *pool);

#endif