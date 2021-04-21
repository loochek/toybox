#ifndef CODE_GEN_HPP
#define CODE_GEN_HPP

#include "utils/lstatus.hpp"
#include "ast.hpp"
#include "compilation_error.hpp"

/**
 * Generates assembly code from AST tree
 * 
 * \param \c ast_root AST root
 * \param \c comp_err Compilation error struct
 */
lstatus_t code_gen(ast_node_t *ast_root, compilation_error_t *comp_err);

#endif