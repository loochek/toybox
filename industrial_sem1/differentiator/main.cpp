#include <stdio.h>

#include "parser.hpp"
#include "diff.hpp"
#include "simpler.hpp"
#include "lerror.hpp"
#include "article.hpp"

#define ERROR_CHECK()                      \
{                                          \
    if (LERR_PRESENT())                    \
    {                                      \
        printf("Error: %s\n", __lerr_str); \
        node_pool_destroy(&pool);          \
        return -1;                         \
    }                                      \
}

const char *DEFAULT_OUT_FILE_NAME = "result.txt";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: differ <article|graphvis|txt> <input file> [output file]\n");
        return 0;
    }

    const char *out_file_name = DEFAULT_OUT_FILE_NAME;
    if (argc >= 4)
        out_file_name = argv[3];

    node_pool_t pool = {0};
    node_pool_construct(&pool);

    expr_node_t *source_tree = expr_load_from_file(argv[2], &pool);
    ERROR_CHECK();

    if (strcmp(argv[1], "article") == 0)
    {
        make_article(source_tree, 'x', &pool);
        ERROR_CHECK();

        system("firefox article.pdf");
    }
    else
    {
        expr_node_t *diff_tree = expr_diff(source_tree, 'x', &pool);
        ERROR_CHECK();

        expr_simplify(&diff_tree, &pool);
        ERROR_CHECK();

        if (strcmp(argv[1], "graphvis") == 0)
            expr_visualize(diff_tree);
        else
            expr_dump(diff_tree, out_file_name);

        expr_destroy(diff_tree, &pool);
    }
    
    expr_destroy(source_tree, &pool);
    node_pool_destroy(&pool);
    return 0;
}