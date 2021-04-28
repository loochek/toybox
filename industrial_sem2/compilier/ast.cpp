#include <cstdio>
#include <cstring>

#include "ast.hpp"

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
    "&lt;",        // AST_OPER_LESS
    "&gt;",        // AST_OPER_MORE
    "&lt;=",       // AST_OPER_ELESS
    "&gt;=",       // AST_OPER_EMORE
    "()",          // AST_OPER_CALL

    "IF",
    "WHILE",
    "RETURN",
    "VAR_DECL",
    "FUNC_DECL",
    "FUNC_HEAD",
    "EXPR_STMT",
    "SCOPE",

    "COMPOUND"
};

static void ast_visualize_rec(ast_node_t *node, int node_id, FILE *file);

void ast_visualize(ast_node_t *tree_root)
{
    FILE *file = fopen("tree.dot", "w");

    fprintf(file, "digraph\n"
                  "{\n"
                  "node [shape=record];\n");

    ast_visualize_rec(tree_root, 1, file);

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tsvg tree.dot > tree.svg");
    system("google-chrome tree.svg");
}

lstatus_t ast_optimize(ast_node_t *node, ast_node_t **node_out, memory_pool_t<ast_node_t> *pool)
{
    if (node == nullptr)
        return LSTATUS_OK;

    LSCHK(ast_optimize(node->left_branch , &node->left_branch , pool));
    LSCHK(ast_optimize(node->right_branch, &node->right_branch, pool));

    switch (node->type)
    {
    case AST_OPER_ADD:
        if (node->left_branch->type == AST_NUMBER && node->right_branch->type == AST_NUMBER)
        {
            node->type = AST_NUMBER;
            node->number = node->left_branch->number + node->right_branch->number;
            *node_out = node;
        }
        else if (node->left_branch->type == AST_NUMBER && node->left_branch->number == 0)
        {
            LSCHK(memory_pool_free(pool, node->left_branch));
            *node_out = node->right_branch;
        }
        else if (node->right_branch->type == AST_NUMBER && node->right_branch->number == 0)
        {
            LSCHK(memory_pool_free(pool, node->right_branch));
            *node_out = node->left_branch;
        }
        break;

    case AST_OPER_SUB:
        if (node->left_branch->type == AST_NUMBER && node->right_branch->type == AST_NUMBER)
        {
            node->type = AST_NUMBER;
            node->number = node->left_branch->number - node->right_branch->number;
            *node_out = node;
        }
        else if (node->right_branch->type == AST_NUMBER && node->right_branch->number == 0)
        {
            LSCHK(memory_pool_free(pool, node->right_branch));
            *node_out = node->left_branch;
        }
        break;
    
    case AST_OPER_MUL:
        if (node->left_branch->type == AST_NUMBER && node->right_branch->type == AST_NUMBER)
        {
            node->type = AST_NUMBER;
            node->number = node->left_branch->number * node->right_branch->number;
            *node_out = node;
        }
        else if (node->left_branch->type  == AST_NUMBER && node->left_branch->number  == 0 ||
                 node->right_branch->type == AST_NUMBER && node->right_branch->number == 0)
        {
            node->type = AST_NUMBER;
            node->number = 0;
            *node_out = node;
        }
        else if (node->left_branch->type == AST_NUMBER && node->left_branch->number == 1)
        {
            LSCHK(memory_pool_free(pool, node->left_branch));
            *node_out = node->right_branch;
        }
        else if (node->right_branch->type == AST_NUMBER && node->right_branch->number == 1)
        {
            LSCHK(memory_pool_free(pool, node->right_branch));
            *node_out = node->left_branch;
        }
        break;

    case AST_OPER_DIV:
        if (node->left_branch->type == AST_NUMBER && node->right_branch->type == AST_NUMBER &&
            node->right_branch->number != 0)
        {
            node->type = AST_NUMBER;
            node->number = node->left_branch->number / node->right_branch->number;
            *node_out = node;
        }
        else if (node->left_branch->type == AST_NUMBER && node->left_branch->number == 0 &&
                 !(node->right_branch->type == AST_NUMBER && node->right_branch->number == 0))
        {
            node->type = AST_NUMBER;
            node->number = 0;
            *node_out = node;
        }
        else if (node->right_branch->type == AST_NUMBER && node->right_branch->number == 1)
        {
            LSCHK(memory_pool_free(pool, node->right_branch));
            *node_out = node->left_branch;
        }
        break;

    case AST_OPER_MOD:
        if (node->left_branch->type == AST_NUMBER && node->right_branch->type == AST_NUMBER &&
            node->right_branch->number != 0)
        {
            node->type = AST_NUMBER;
            node->number = node->left_branch->number % node->right_branch->number;
            *node_out = node;
        }
        if (node->right_branch->type == AST_NUMBER && node->right_branch->number == 1)
        {
            node->type = AST_NUMBER;
            node->number = 0;
            *node_out = node;
        }
        break;

    default:
        *node_out = node;
        break;
    }

    if ((*node_out)->type == AST_NUMBER)
    {
        LSCHK(memory_pool_free(pool, node->left_branch));
        LSCHK(memory_pool_free(pool, node->right_branch));
        node->left_branch  = nullptr;
        node->right_branch = nullptr;
        *node_out = node;
    }

    return LSTATUS_OK;
}

lstatus_t ast_destroy(ast_node_t *node, memory_pool_t<ast_node_t> *pool)
{
    if (node == nullptr)
        return LSTATUS_OK;

    if (node->left_branch != NULL)
        LSCHK(ast_destroy(node->left_branch, pool));

    if (node->right_branch != NULL)
        LSCHK(ast_destroy(node->right_branch, pool));
    
    LSCHK(memory_pool_free(pool, node));
    return LSTATUS_OK;
}

static void ast_visualize_rec(ast_node_t *node, int node_id, FILE *file)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case AST_NUMBER:
        fprintf(file, "%d [fillcolor=cyan, style=filled, label=\"%d | { %d | %d }\"]\n",
                node_id, node->number, node->row, node->col);
        break;

    case AST_IDENTIFIER:
        fprintf(file, "%d [fillcolor=darkseagreen1, style=filled, label=\"%.*s | { %d | %d }\"]\n",
                node_id, node->ident.length, node->ident.str, node->row, node->col);
        break;

    case AST_COMPOUND:
        fprintf(file, "%d [fillcolor=azure2, style=filled, label=\"%s\"]\n",
                node_id, ast_types_display_names[node->type]);
        break;

    case AST_SCOPE:
        fprintf(file, "%d [fillcolor=bisque3, style=filled, label=\"%s\"]\n",
                node_id, ast_types_display_names[node->type]);
        break;
    
    case AST_OPER_ADD:
    case AST_OPER_SUB:
    case AST_OPER_MUL:
    case AST_OPER_DIV:
    case AST_OPER_MOD:
    case AST_OPER_ASSIGN:
    case AST_OPER_EQUAL:
    case AST_OPER_NEQUAL:
    case AST_OPER_LESS:
    case AST_OPER_MORE:
    case AST_OPER_ELESS:
    case AST_OPER_EMORE:
    case AST_CALL:
        fprintf(file, "%d [fillcolor=coral, style=filled, label=\"%s | { %d | %d }\"]\n",
                node_id, ast_types_display_names[node->type], node->row, node->col);
        break;

    default:
        fprintf(file, "%d [fillcolor=gold, style=filled, label=\"%s | { %d | %d }\"]\n",
                node_id, ast_types_display_names[node->type], node->row, node->col);
        break;
    }

    ast_visualize_rec(node->left_branch , node_id * 2    , file);
    ast_visualize_rec(node->right_branch, node_id * 2 + 1, file);

    if (node->left_branch != NULL)
        fprintf(file, "%d->%d\n", node_id, node_id * 2);
    if (node->right_branch != NULL)
        fprintf(file, "%d->%d\n", node_id, node_id * 2 + 1);
}