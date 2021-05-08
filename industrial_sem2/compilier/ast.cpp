#include <cstdio>
#include <cstring>

#include "ast.hpp"

const char *ast_types_display_names[] = 
{
    "",            // AST_DUMMY 

    "",            // AST_NUMBER
    "",            // AST_IDENTIFIER
    "[]",          // AST_INDEX

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
    "IF_BRANCHES",
    "WHILE",
    "RETURN",
    "VAR_DECL",
    "ARR_DECL",
    "FUNC_DECL",
    "FUNC_HEAD",
    "EXPR_STMT",
    "SCOPE",

    "COMPOUND"
};

/**
 * Helper function for recursive AST visualizing
 */
static void ast_visualize_rec(ast_node_t *node, int node_id, FILE *file);

/**
 * Helper function for AST validation
 */
static lstatus_t ast_validate_root(ast_node_t *node);

/**
 * Helper function for AST validation
 */
static lstatus_t ast_validate_func_decl_args(ast_node_t *node);

/**
 * Helper function for AST validation
 */
static lstatus_t ast_validate_stmt(ast_node_t *node);

/**
 * Helper function for AST validation
 */
static lstatus_t ast_validate_expr(ast_node_t *node);

/**
 * Helper function for AST validation
 */
static lstatus_t ast_validate_func_call_args(ast_node_t *node);


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

lstatus_t ast_validate(ast_node_t *node)
{
    return ast_validate_root(node);
}

static void ast_visualize_rec(ast_node_t *node, int node_id, FILE *file)
{
    if (node == nullptr)
        return;

    switch (node->type)
    {
    case AST_NUMBER:
        fprintf(file, "%d [fillcolor=cyan, style=filled, label=\"%ld | { %d | %d }\"]\n",
                node_id, node->number, node->row, node->col);
        break;

    case AST_IDENTIFIER:
        fprintf(file, "%d [fillcolor=darkseagreen1, style=filled, label=\"%.*s | { %d | %d }\"]\n",
                node_id, node->ident.length, node->ident.str, node->row, node->col);
        break;

    case AST_INDEX:
        fprintf(file, "%d [fillcolor=wheat2, style=filled, label=\"%s | { %d | %d }\"]\n",
                node_id, ast_types_display_names[node->type], node->row, node->col);
        break;

    case AST_COMPOUND:
        fprintf(file, "%d [fillcolor=azure2, style=filled, label=\"%s\"]\n",
                node_id, ast_types_display_names[node->type]);
        break;

    case AST_SCOPE:
        fprintf(file, "%d [fillcolor=bisque3, style=filled, label=\"%s\"]\n",
                node_id, ast_types_display_names[node->type]);
        break;

    case AST_FUNC_HEAD:
    case AST_IF_BRANCHES:
        fprintf(file, "%d [fillcolor=darkturquoise, style=filled, label=\"%s\"]\n",
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

static lstatus_t ast_validate_expr(ast_node_t *node)
{
    lstatus_t status = LSTATUS_OK;
    
    if (node == nullptr)
        return LSTATUS_OK;

    switch (node->type)
    {
    case AST_DUMMY:
        LSTATUS(LSTATUS_BAD_AST, "dummy node in AST")
        return status;

    case AST_NUMBER:
        return LSTATUS_OK;

    case AST_IDENTIFIER:
        if (node->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        return LSTATUS_OK;

    case AST_INDEX:
        if (node->left_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer left child of index node")
            return status;
        }

        if (node->left_branch->type != AST_IDENTIFIER)
        {
            LSTATUS(LSTATUS_BAD_AST, "non-identifier left branch of index")
            return status;
        }

        if (node->left_branch->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        LSCHK(ast_validate_expr(node->right_branch));
        return LSTATUS_OK;

    case AST_OPER_ADD:
    case AST_OPER_SUB:
    case AST_OPER_MUL:
    case AST_OPER_DIV:
    case AST_OPER_MOD:
    case AST_OPER_EQUAL:
    case AST_OPER_NEQUAL:
    case AST_OPER_LESS:
    case AST_OPER_MORE:
    case AST_OPER_ELESS:
    case AST_OPER_EMORE:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of operator node")
            return status;
        }

        LSCHK(ast_validate_expr(node->left_branch));
        LSCHK(ast_validate_expr(node->right_branch));
        return LSTATUS_OK;

    case AST_OPER_ASSIGN:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of operator node")
            return status;
        }

        if (node->left_branch->type != AST_IDENTIFIER)
        {
            LSTATUS(LSTATUS_BAD_AST, "non-identifier left branch of assignment")
            return status;
        }

        if (node->left_branch->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        LSCHK(ast_validate_expr(node->right_branch));
        return LSTATUS_OK;

    case AST_CALL:
        if (node->left_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer left child of function call node")
            return status;
        }

        if (node->left_branch->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        LSCHK(ast_validate_func_call_args(node->right_branch));
        return LSTATUS_OK;

    default:
        LSTATUS(LSTATUS_BAD_AST, "bad expression node")
        return status;
    }
}

static lstatus_t ast_validate_func_call_args(ast_node_t *node)
{
    lstatus_t status = LSTATUS_OK;
    
    if (node == nullptr)
        return LSTATUS_OK;

    switch (node->type)
    {
    case AST_COMPOUND:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of compound node")
            return status;
        }

        LSCHK(ast_validate_func_call_args(node->left_branch));
        LSCHK(ast_validate_func_call_args(node->right_branch));
        return LSTATUS_OK;

    default:
        LSCHK(ast_validate_expr(node));
        return LSTATUS_OK;
    }
}

static lstatus_t ast_validate_stmt(ast_node_t *node)
{
    lstatus_t status = LSTATUS_OK;
    
    if (node == nullptr)
        return LSTATUS_OK;

    switch (node->type)
    {
    case AST_COMPOUND:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of compound node")
            return status;
        }

        LSCHK(ast_validate_stmt(node->left_branch));
        LSCHK(ast_validate_stmt(node->right_branch));
        return LSTATUS_OK;

    case AST_SCOPE:
        LSCHK(ast_validate_stmt(node->left_branch));
        return LSTATUS_OK;

    case AST_EXPR_STMT:
        LSCHK(ast_validate_expr(node->left_branch));
        return LSTATUS_OK;

    case AST_VAR_DECL:
        if (node->left_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer left child of var decl node")
            return status;
        }

        if (node->left_branch->type != AST_IDENTIFIER)
        {
            LSTATUS(LSTATUS_BAD_AST, "non-identifier left branch of var decl")
            return status;
        }

        if (node->left_branch->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        LSCHK(ast_validate_expr(node->right_branch));
        return LSTATUS_OK;

    case AST_ARR_DECL:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer children of arr decl node")
            return status;
        }

        if (node->left_branch->type != AST_IDENTIFIER)
        {
            LSTATUS(LSTATUS_BAD_AST, "non-identifier left branch of arr decl")
            return status;
        }

        if (node->left_branch->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        if (node->right_branch->type != AST_NUMBER)
        {
            LSTATUS(LSTATUS_BAD_AST, "non-number right branch of arr decl")
            return status;
        }

        return LSTATUS_OK;

    case AST_RETURN:
        LSCHK(ast_validate_expr(node->left_branch));
        return LSTATUS_OK;

    case AST_IF:
        LSCHK(ast_validate_expr(node->left_branch));

        if (node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer right child of if node")
            return status;
        }

        if (node->right_branch->type != AST_IF_BRANCHES)
        {
            LSTATUS(LSTATUS_BAD_AST, "right child of if node is not AST_IF_BRANCHES")
            return status;
        }

        LSCHK(ast_validate_stmt(node->right_branch->left_branch));
        LSCHK(ast_validate_stmt(node->right_branch->right_branch));
        return LSTATUS_OK;

    case AST_WHILE:
        LSCHK(ast_validate_expr(node->left_branch));
        LSCHK(ast_validate_stmt(node->right_branch));
        return LSTATUS_OK;

    default:
        LSTATUS(LSTATUS_BAD_AST, "bad statement node");
        return status;
    }
}

static lstatus_t ast_validate_root(ast_node_t *node)
{
    lstatus_t status = LSTATUS_OK;
    
    if (node == nullptr)
        return LSTATUS_OK;

    switch (node->type)
    {
    case AST_COMPOUND:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of compound node")
            return status;
        }

        LSCHK(ast_validate_root(node->left_branch));
        LSCHK(ast_validate_root(node->right_branch));
        return LSTATUS_OK;

    case AST_FUNC_DECL:
        if (node->left_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer left child of func decl node")
            return status;
        }

        if (node->left_branch->type != AST_FUNC_HEAD)
        {
            LSTATUS(LSTATUS_BAD_AST, "left child of func decl node is not AST_FUNC_HEAD")
            return status;
        }

        if (node->left_branch->left_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer left child of func head node")
            return status;
        }

        if (node->left_branch->left_branch->type != AST_IDENTIFIER)
        {
            LSTATUS(LSTATUS_BAD_AST, "left child of func head node is not identifier")
            return status;
        }

        LSCHK(ast_validate_func_decl_args(node->left_branch->right_branch));
        LSCHK(ast_validate_stmt(node->right_branch));
        return LSTATUS_OK;

    default:
        LSTATUS(LSTATUS_BAD_AST, "bad root node");
        return status;
    }
}

static lstatus_t ast_validate_func_decl_args(ast_node_t *node)
{
    lstatus_t status = LSTATUS_OK;
    
    if (node == nullptr)
        return LSTATUS_OK;

    switch (node->type)
    {
    case AST_COMPOUND:
        if (node->left_branch == nullptr || node->right_branch == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer child of compound node")
            return status;
        }

        LSCHK(ast_validate_func_decl_args(node->left_branch));
        LSCHK(ast_validate_func_decl_args(node->right_branch));
        return LSTATUS_OK;
    
    case AST_IDENTIFIER:
        if (node->ident.str == nullptr)
        {
            LSTATUS(LSTATUS_BAD_AST, "null-pointer identifier string")
            return status;
        }

        return LSTATUS_OK;

    default:
        LSTATUS(LSTATUS_BAD_AST, "bad func decl arg node");
        return status;
    }
}