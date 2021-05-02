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
 * \param \c elf_file_name ELF file name
 * \param \c lst_file_name Assembly listing file name
 */
lstatus_t code_gen(ast_node_t *ast_root, compilation_error_t *comp_err,
                   const char *elf_file_name, const char *lst_file_name);

#endif