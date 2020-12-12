#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "node_pool.h"

/**
 * LERR-affecting
 */
ast_node_t *ast_build(lexem_t *lexems, node_pool_t *pool);

#endif