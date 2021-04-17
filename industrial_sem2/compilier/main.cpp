#include "utils/lstatus.hpp"
#include "utils/file_utils.hpp"
#include "lexer.hpp"

int main()
{
    lstatus_t status = LSTATUS_OK;

    char *src = nullptr;
    status = create_buffer_from_file("examples/fib.tc", &src);
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

    compilier_status_t comp_status = {};
    lexer_parse_source(src, &lexems, &comp_status);

    return 0;
}