#include "utils/lstatus.hpp"
#include "utils/file_utils.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "code_gen.hpp"

#define ERROR_HANDLER() goto error_handler

int main()
{
    lstatus_t status = LSTATUS_OK;

    char *src = nullptr;
    list_t<lexem_t> lexems = {};
    ast_node_t *tree_root = nullptr;
    compilation_error_t comp_err = {};

    status = create_buffer_from_file("examples/test.tc", &src);
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

    status = code_gen(tree_root, &comp_err);
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