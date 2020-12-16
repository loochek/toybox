#ifndef ASSM_GEN_H
#define ASSM_GEN_H

#include "ast.h"

/**
 * LERR-affecting
 */
void assm_gen(ast_node_t *ast_root, const char *file_name);

#endif