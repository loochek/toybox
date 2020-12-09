#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

/**
 * LERR-affecting
 */
ast_node_t *ast_build(lexem_t *lexems);

#endif