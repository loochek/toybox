#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "node_pool.h"

/**
 * Lexems-to-AST translator
 */

/**
 * LERR-affecting
 * returns NULL if failed
 */
ast_node_t *ast_build(lexem_t *lexems, node_pool_t *pool);

#endif