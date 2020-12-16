#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "node_pool.h"

const char *ast_types_display_names[] = 
{
    "",            // AST_DUMMY 

    "",            // AST_NUMBER
    "",            // AST_IDENTIFIER

    "+",           // AST_OPER_ADD
    "-",           // AST_OPER_SUB
    "*",           // AST_OPER_MUL
    "/",           // AST_OPER_DIV
    "%",           // AST_OPER_MOD
    "=",           // AST_OPER_ASSIGN
    "==",          // AST_OPER_EQUAL
    "!=",          // AST_OPER_NQUAL
    "<",           // AST_OPER_LESS
    ">",           // AST_OPER_MORE
    "<=",          // AST_OPER_ELESS
    ">=",          // AST_OPER_EMORE
    "()",          // AST_OPER_CALL

    "IF",
    "WHILE",
    "RETURN",
    "VAR_DECL",
    "FUNC_DECL",
    "FUNC_HEAD",
    "EXPR_STMT",

    "COMPOUND"
};

static void ast_visualize_rec(ast_node_t *node, size_t node_id, FILE *file);

void ast_visualize(ast_node_t *tree_root)
{
    FILE *file = fopen("tree.dot", "w");

    fprintf(file, "digraph\n{\n");

    ast_visualize_rec(tree_root, 1, file);

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tsvg tree.dot > tree.svg");
    system("firefox tree.svg");
}

void ast_destroy(ast_node_t *node, node_pool_t *pool)
{
    if (node == NULL)
        return;

    if (node->left_branch != NULL)
        ast_destroy(node->left_branch, pool);

    if (node->right_branch != NULL)
        ast_destroy(node->right_branch, pool);
    
    node_pool_free(node, pool);
}

static void ast_visualize_rec(ast_node_t *node, size_t node_id, FILE *file)
{
    if (node == NULL)
        return;

    if (node->type == AST_NUMBER)
    {
        fprintf(file, "%zu [shape=box, style=filled, color=\"#7c9ccf\", label=%d]\n",
                node_id, node->number);
        return;
    }

    if (node->type == AST_IDENTIFIER)
    {
        fprintf(file, "%zu [shape=box, style=filled, color=\"#7c9ccf\", label=\"%.*s\"]\n",
                node_id, (int)node->ident.length, node->ident.str);
        return;
    }

    if (AST_OPER_ADD <= node->type && node->type <= AST_OPER_CALL)
        fprintf(file, "%zu [shape=diamond, label=\"%s\"]\n", node_id, ast_types_display_names[node->type]);
    else
        fprintf(file, "%zu [shape=box, label=\"%s\"]\n", node_id, ast_types_display_names[node->type]);

    ast_visualize_rec(node->left_branch , node_id * 2    , file);
    ast_visualize_rec(node->right_branch, node_id * 2 + 1, file);

    if (node->left_branch != NULL)
        fprintf(file, "%zu->%zu\n", node_id, node_id * 2);
    if (node->right_branch != NULL)
        fprintf(file, "%zu->%zu\n", node_id, node_id * 2 + 1);
}