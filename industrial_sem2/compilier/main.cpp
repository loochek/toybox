#include "utils/lstatus.hpp"
#include "utils/file_utils.hpp"
#include "lexer.hpp"
#include "parser.hpp"

int main()
{
    lstatus_t status = LSTATUS_OK;

    char *src = nullptr;
    status = create_buffer_from_file("examples/test.tc", &src);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        return -1;
    }

    list_t<lexem_t> lexems = {};
    status = list_construct(&lexems, 10);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        return -1;
    }

    compilation_error_t comp_err = {};
    status = lexer_tokenize(src, &lexems, &comp_err);

    ast_node_t *tree_root = nullptr;
    status = ast_build(&lexems, nullptr, &comp_err, &tree_root);
    ast_visualize(tree_root);

    return 0;
}