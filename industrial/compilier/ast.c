#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "node_pool.h"

const char *ast_types_display_names[] = 
{
    "",
    "",
    "",
    "+",
    "-",
    "*",
    "/",
    "%",
    "=",
    "==",
    "!=",
    "<",
    ">",
    "<=",
    ">=",
    "()"
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
                node_id, node->ident.length, node->ident.str);
        return;
    }

    if (node->type == AST_VAR_DECL)
        fprintf(file, "%zu [shape=box, label=\"VAR_DECL\"]\n", node_id);
    else if (node->type == AST_FUNC_DECL)
        fprintf(file, "%zu [shape=box, label=\"FUNC_DECL\"]\n", node_id);
    else if (node->type == AST_COMPOUND)
        fprintf(file, "%zu [shape=box, label=\"COMPOUND\"]\n", node_id);
    else if (node->type == AST_IF)
        fprintf(file, "%zu [shape=box, label=\"IF\"]\n", node_id);
    else if (node->type == AST_WHILE)
        fprintf(file, "%zu [shape=box, label=\"WHILE\"]\n", node_id);
    else if (AST_OPER_ADD <= node->type && node->type <= AST_OPER_CALL)
    {
        fprintf(file, "%zu [shape=box, shape=diamond, label=\"%s\"]\n",
                node_id, ast_types_display_names[node->type]);
    }

    ast_visualize_rec(node->left_branch , node_id * 2    , file);
    ast_visualize_rec(node->right_branch, node_id * 2 + 1, file);

    if (node->left_branch != NULL)
        fprintf(file, "%zu->%zu\n", node_id, node_id * 2);
    if (node->right_branch != NULL)
        fprintf(file, "%zu->%zu\n", node_id, node_id * 2 + 1);
}