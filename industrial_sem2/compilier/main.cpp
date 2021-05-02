#include "utils/lstatus.hpp"
#include "utils/file_utils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "code_gen.hpp"

#define CLEANUP()           goto cleanup
#define COMPILATION_ERROR() goto compilation_error
#define GENERAL_ERROR()     goto general_error

#define LSCHK_LOCAL(expr) { status = expr; if (status != LSTATUS_OK) GENERAL_ERROR(); }

lstatus_t compile(const char *src_file_name,
                  const char *elf_file_name,
                  const char *lst_file_name,
                  bool show_ast)
{
    lstatus_t status = LSTATUS_OK;

    char               *src       = nullptr;
    ast_node_t         *tree_root = nullptr;
    list_t<lexem_t>     lexems    = {};
    compilation_error_t comp_err  = {};

    LSCHK_LOCAL(create_buffer_from_file(src_file_name, &src));

    LSCHK_LOCAL(list_construct(&lexems));

    status = lexer_tokenize(src, &lexems, &comp_err);
    if (status == LSTATUS_LEXER_FAIL)
        COMPILATION_ERROR();
    else if (status != LSTATUS_OK)
        GENERAL_ERROR();

    status = ast_build(&lexems, nullptr, &comp_err, &tree_root);
    if (status == LSTATUS_PARSER_FAIL)
        COMPILATION_ERROR();
    else if (status != LSTATUS_OK)
        GENERAL_ERROR();

    LSCHK_LOCAL(ast_optimize(tree_root, &tree_root, nullptr));

    if (show_ast)
        ast_visualize(tree_root);

    status = code_gen(tree_root, &comp_err, elf_file_name, lst_file_name);
    if (status == LSTATUS_CODE_GEN_FAIL)
        COMPILATION_ERROR();
    else if (status != LSTATUS_OK)
        GENERAL_ERROR();

    CLEANUP();

compilation_error:
    print_error(&comp_err);
    CLEANUP();

general_error:
    LS_ERR_PRINT();
    CLEANUP();

cleanup:
    free(src);
    list_destruct(&lexems);
    ast_destroy(tree_root, nullptr);

    return status;
}

int main()
{
    lstatus_t status = LSTATUS_OK;

    char *src = nullptr;
    list_t<lexem_t> lexems = {};
    ast_node_t *tree_root = nullptr;
    compilation_error_t comp_err = {};

    status = create_buffer_from_file("examples/equ.tc", &src);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        ERROR_HANDLER();
    }

    status = list_construct(&lexems, 10);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        ERROR_HANDLER();
    }

    status = lexer_tokenize(src, &lexems, &comp_err);
    if (status == LSTATUS_LEXER_FAIL)
    {
        print_error(&comp_err);
        ERROR_HANDLER();
    }

    status = ast_build(&lexems, nullptr, &comp_err, &tree_root);
    if (status == LSTATUS_PARSER_FAIL)
    {
        print_error(&comp_err);
        ERROR_HANDLER();
    }
    else if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        ERROR_HANDLER();
    }

    status = ast_optimize(tree_root, &tree_root, nullptr);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        ERROR_HANDLER();
    }

    status = code_gen(tree_root, &comp_err, "a.out", "a.lst");
    if (status == LSTATUS_CODE_GEN_FAIL)
    {
        print_error(&comp_err);
        ERROR_HANDLER();
    }
    else if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        ERROR_HANDLER();
    }

    //ast_visualize(tree_root);

    free(src);
    list_destruct(&lexems);
    ast_destroy(tree_root, nullptr);

    return 0;

error_handler:
    free(src);
    list_destruct(&lexems);
    ast_destroy(tree_root, nullptr);

    return -1;
}
