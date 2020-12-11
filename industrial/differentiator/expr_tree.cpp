#include "expr_tree.hpp"
#include "lerror.hpp"
#include "node_pool.hpp"

const char *node_types_display_labels[] =
{
    "",
    "",
    "",
    "+",
    "-",
    "*",
    "/",
    "^",
    "sin",
    "cos",
    "ln",
    "exp"
};

const int priority[] = { 4, 4, 4, 0, 0, 1, 1, 2, 3, 3, 3, 3 };

static void expr_dump_rec      (expr_node_t *node, FILE *file);
static void expr_visualize_rec (expr_node_t *node, FILE *file, size_t node_id);
static void expr_latex_dump_rec(expr_node_t *node, FILE *file);

void expr_dump(expr_node_t *tree_root, const char *file_name)
{
    LERR_RESET();
    EXPR_CHECK_RET(tree_root,);

    FILE *file = fopen(file_name, "w");

    expr_dump_rec(tree_root, file);

    fclose(file);
}

void expr_visualize(expr_node_t *tree_root)
{
    LERR_RESET();
    EXPR_CHECK_RET(tree_root,);

    FILE *file = fopen("expr.dot", "w");
    fprintf(file, "digraph\n{\n");

    expr_visualize_rec(tree_root, file, 1);

    fprintf(file, "}\n");
    fclose(file);

    system("dot -Tsvg expr.dot > expr.svg");
    system("firefox expr.svg");
}

void expr_latex_put(expr_node_t *tree_root, FILE *file)
{
    LERR_RESET();
    EXPR_CHECK_RET(tree_root,);

    fprintf(file, "$$");
    expr_latex_dump_rec(tree_root, file);
    fprintf(file, "$$\n");
}

int expr_validate(expr_node_t *node)
{
    LERR_RESET();

    if (node == NULL)
    {
        LERR(LERR_EXPR_TREE_VALIDATION, "null pointer passed");
        return -1;
    }

    if (node->type == TYPE_NONE)
    {
        LERR(LERR_EXPR_TREE_VALIDATION, "none type node");
        return -1;
    }

    if ((node->type == TYPE_NUM || node->type == TYPE_VAR) &&
        (node->first_arg != NULL || node->second_arg != NULL))
    {
        LERR(LERR_EXPR_TREE_VALIDATION, "var/num node has children");
        return -1;
    }

    if (node->type == TYPE_SIN || node->type == TYPE_COS ||
         node->type == TYPE_LN || node->type == TYPE_EXP)
    {
        if (node->first_arg == NULL || node->second_arg != NULL)
        {
            LERR(LERR_EXPR_TREE_VALIDATION, "bad argument for unary function");
            return -1;
        }

        if (expr_validate(node->first_arg) != 0)
            return -1;
    }
    

    if (node->type == TYPE_ADD || node->type == TYPE_SUB || node->type == TYPE_POW ||
         node->type == TYPE_MUL|| node->type == TYPE_DIV)
    {
        if (node->first_arg == NULL || node->second_arg == NULL)
        {
            LERR(LERR_EXPR_TREE_VALIDATION, "bad argument for binary operator");
            return -1;
        }

        if (expr_validate(node->first_arg) != 0)
            return -1;
        
        if (expr_validate(node->second_arg) != 0)
            return -1;
    }

    return 0;
}

void expr_destroy(expr_node_t *node, struct node_pool_t *pool)
{
    if (node == NULL)
        return;

    if (node->first_arg != NULL)
        expr_destroy(node->first_arg, pool);
    if (node->second_arg != NULL)
        expr_destroy(node->second_arg, pool);

    node_pool_free(node, pool);
}

expr_node_t *expr_deep_copy(expr_node_t *node, struct node_pool_t *pool)
{
    LERR_RESET();

    if (node == NULL)
        return NULL;

    expr_node_t *copy = node_pool_claim(pool);
    if (LERR_PRESENT())
        return NULL;

    *copy = *node;

    copy->first_arg  = expr_deep_copy(node->first_arg, pool);
    if (LERR_PRESENT())
        return NULL;

    copy->second_arg = expr_deep_copy(node->second_arg, pool);
    if (LERR_PRESENT())
        return NULL;

    return copy;
}

bool expr_is_constant(expr_node_t *node, char var)
{
    if (node == NULL)
        return true;

    if (node->type == TYPE_NUM)
        return true;

    if (node->type == TYPE_VAR)
    {
        if (node->var == var)
            return false;
        else
            return true;
    }
        
    if (!expr_is_constant(node->first_arg, var))
        return false;

    if (!expr_is_constant(node->second_arg, var))
        return false;

    return true;
}

static void expr_dump_rec(expr_node_t *node, FILE *file)
{
    if (node->type == TYPE_NUM)
    {
        if (node->number < 0)
            fprintf(file, "(0%d)", node->number);
        else
            fprintf(file, "%d", node->number);

        return;
    }

    if (node->type == TYPE_VAR)
    {
        fprintf(file, "%c", node->var);
        return;
    }

    if (node->second_arg != NULL)
    {
        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_dump_rec(node->first_arg, file);

        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, ")");

        fprintf(file, "%s", node_types_display_labels[node->type]);

        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_dump_rec(node->second_arg, file);
        
        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, ")");
    }
    else
    {
        fprintf(file, "%s", node_types_display_labels[node->type]);

        fprintf(file, "(");
        expr_dump_rec(node->first_arg, file);
        fprintf(file, ")");        
    }   
}

static void expr_visualize_rec(expr_node_t *node, FILE *file, size_t node_id)
{
    if (node->type == TYPE_NUM)
    {
        fprintf(file, "%zu [label=\"%d\", style=filled, color=green];\n", node_id, node->number);
        return;
    }

    if (node->type == TYPE_VAR)
    {
        fprintf(file, "%zu [label=\"%c\", style=filled, color=red];\n", node_id, node->var);
        return;
    }

    fprintf(file, "%zu [label=\"%s\"];\n", node_id, node_types_display_labels[node->type]);

    expr_visualize_rec(node->first_arg , file, node_id * 2);
    fprintf(file, "%zu->%zu;\n", node_id, node_id * 2);

    if (node->second_arg != NULL)
    {
        expr_visualize_rec(node->second_arg, file, node_id * 2 + 1);
        fprintf(file, "%zu->%zu;\n", node_id, node_id * 2 + 1);
    }   
}

static void expr_latex_dump_rec(expr_node_t *node, FILE *file)
{
    // easy operations - no parenthesis required
    
    if (node->type == TYPE_NUM)
    {
        fprintf(file, "%d", node->number);
        return;
    }

    if (node->type == TYPE_VAR)
    {
        fprintf(file, "%c", node->var);
        return;
    }

    if (node->type == TYPE_DIV)
    {
        fprintf(file, "\\frac{");
        expr_latex_dump_rec(node->first_arg, file);
        fprintf(file, "}{");
        expr_latex_dump_rec(node->second_arg, file);
        fprintf(file, "}");

        return;
    }

    if (node->type == TYPE_SIN)
    {
        fprintf(file, "\\sin(");
        expr_latex_dump_rec(node->first_arg, file);
        fprintf(file, ")");

        return;
    }

    if (node->type == TYPE_COS)
    {
        fprintf(file, "\\cos(");
        expr_latex_dump_rec(node->first_arg, file);
        fprintf(file, ")");

        return;
    }

    if (node->type == TYPE_LN)
    {
        fprintf(file, "\\ln(");
        expr_latex_dump_rec(node->first_arg, file);
        fprintf(file, ")");

        return;
    }
    
    if (node->type == TYPE_EXP)
    {
        fprintf(file, "e^{");
        expr_latex_dump_rec(node->first_arg, file);
        fprintf(file, "}");

        return;
    }


    if (node->type == TYPE_ADD || node->type == TYPE_SUB)
    {
        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_latex_dump_rec(node->first_arg, file);

        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, ")");

        fprintf(file, "%s", node_types_display_labels[node->type]);

        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_latex_dump_rec(node->second_arg, file);

        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, ")");

        return;
    }

    if (node->type == TYPE_POW)
    {
        fprintf(file, "{");

        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_latex_dump_rec(node->first_arg, file);

        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, ")");

        fprintf(file, "}^{");
        expr_latex_dump_rec(node->second_arg, file);
        fprintf(file, "}");

        return;
    }

    if (node->type == TYPE_MUL)
    {
        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_latex_dump_rec(node->first_arg, file);

        if (priority[node->first_arg->type] < priority[node->type])
            fprintf(file, ")");

        fprintf(file, "\\cdot ");

        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, "(");

        expr_latex_dump_rec(node->second_arg, file);

        if (priority[node->second_arg->type] < priority[node->type])
            fprintf(file, ")");

        return;
    }
}