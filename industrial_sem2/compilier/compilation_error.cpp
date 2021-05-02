#include <cstdio>

#include "compilation_error.hpp"

void print_error(compilation_error_t *comp_error)
{
    if (comp_error->row == -1)
    {
        printf("\u001b[31mError\u001b[0m: %s\n", comp_error->err_string);
    }
    else
    {
        printf("\u001b[31mError \u001b[0mat line %d collumn %d: %s\n",
           comp_error->row, comp_error->col, comp_error->err_string);
    }
}