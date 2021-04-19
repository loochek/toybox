#include <cstdio>

#include "compilation_error.hpp"

void print_error(compilation_error_t *comp_error)
{
    printf("\u001b[31mError \u001b[0mat line %d collumn %d: %s\n",
           comp_error->row + 1, comp_error->col + 1, comp_error->err_string);
}