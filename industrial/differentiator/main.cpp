#include <stdio.h>

#include "parser.hpp"
#include "diff.hpp"
#include "simpler.hpp"
#include "lerror.hpp"

int main()
{
    node_pool_t pool = {0};
    node_pool_construct(&pool);

    expr_node_t *tree_root = expr_load_from_file("expr_example.txt", &pool);

    for (size_t i = 0; i < 8; i++)
    {

        expr_node_t *diff_tree = expr_diff(tree_root, 'x', &pool);
        expr_destroy(tree_root, &pool);

        expr_simplify(&diff_tree, &pool);

        tree_root = diff_tree;
        
        printf("%zu!\n", i);
    }

    printf("done!\n");
    
    expr_dump(tree_root);
    printf("basic dump is done!\n");

    expr_destroy(tree_root, &pool);
    node_pool_destroy(&pool);
    
    return 0;
}