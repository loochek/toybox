#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"

/// TODO: review parser

/**
 * Lexems-to-AST translator
 */

/**
 * LERR-affecting
 * returns NULL if failed
 */
ast_node_t *ast_build(lexem_t *lexems, node_pool_t *pool);

#endif