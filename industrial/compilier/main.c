#include "file_utils.h"
#include "lexer.h"
#include "parser.h"
#include "lerror.h"

int main()
{
    char *buf = NULL;
    int data_size = create_buffer_from_file("prg.c", &buf);
    lexem_t *lexems = NULL;
    int lexems_count = create_lexical_array(&lexems, buf);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }
    
    ast_node_t *ast = ast_build(lexems);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }

    return 0;
}