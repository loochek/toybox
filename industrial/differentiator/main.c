#include <stdio.h>

#include "parser.h"
#include "diff.h"
#include "simpler.h"
#include "lerror.h"

int main()
{
    expr_node_t *tree_root = expr_load_from_file("expr_example.txt");
    if (tree_root == NULL)
    {
        printf("Error: %s\n", __lerr_str);
        return 0;
    }

    expr_node_t *diff_tree = expr_diff(tree_root, 'x');
    //expr_node_t *diff_tree = expr_deep_copy(tree_root);
    expr_destroy(tree_root);

    expr_simplify(&diff_tree);

    expr_dump(diff_tree);
    expr_visualize(diff_tree);
    expr_latex_dump(diff_tree);
    

    expr_destroy(diff_tree);
    
    return 0;
}