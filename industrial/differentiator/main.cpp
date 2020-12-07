#include <stdio.h>

#include "parser.hpp"
#include "diff.hpp"
#include "simpler.hpp"
#include "lerror.hpp"

#define ERROR_CHECK()                      \
{                                          \
    if (LERR_PRESENT())                    \
    {                                      \
        printf("Error: %s\n", __lerr_str); \
        node_pool_destroy(&pool);          \
        return -1;                         \
    }                                      \
}
    

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: differ <diff|simplify> <input file>\n"
               "diff - differentiate by x\n"
               "simplify - just simplify\n"
               "Result is saved to result.txt and shown in Firefox\n");
        return 0;
    }

    node_pool_t pool = {0};
    node_pool_construct(&pool);

    expr_node_t *source_tree = expr_load_from_file(argv[2], &pool);
    ERROR_CHECK();

    if (strcmp(argv[1], "diff") == 0)
    {
        expr_node_t *diff_tree = expr_diff(source_tree, 'x', &pool);
        ERROR_CHECK();

        expr_simplify(&diff_tree, &pool);
        ERROR_CHECK();
        
        expr_dump      (diff_tree, "result.txt");
        expr_latex_dump(diff_tree);
        expr_visualize (diff_tree);

        expr_destroy(diff_tree, &pool);
    }
    else if (strcmp(argv[1], "simplify") == 0)
    {
        expr_simplify(&source_tree, &pool);
        ERROR_CHECK();
        
        expr_dump      (source_tree, "result.txt");
        expr_latex_dump(source_tree);
        expr_visualize (source_tree);
    }
    
    expr_destroy(source_tree, &pool);
    node_pool_destroy(&pool);
    return 0;
}