#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "lexer.hpp"

/**
 * Builds an AST from the sequence of lexems
 * 
 * \param \c lexems The sequence of lexems
 * \param \c pool Memory pool for AST nodes
 * \param \c comp_err Compilation error structure
 * \param \c tree_root_out Where to write a tree root pointer
 */
lstatus_t ast_build(list_t<lexem_t> *lexems, memory_pool_t<ast_node_t> *pool,
                    compilation_error_t *comp_err, ast_node_t **tree_root_out);

#endif