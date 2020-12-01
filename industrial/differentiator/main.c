#include <stdio.h>

#include "parser.h"
#include "lerror.h"


int main()
{
    expr_node_t *tree_root = expr_load_from_file("expr_example.txt");
    if (tree_root == NULL)
    {
        printf("Error: %s\n", __lerr_str);
        return 0;
    }

    expr_latex_dump(tree_root);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return 0;
    }

    expr_visualize(tree_root);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return 0;
    }

    expr_destroy(tree_root);

    return 0;
}