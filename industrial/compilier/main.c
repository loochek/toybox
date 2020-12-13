#include "file_utils.h"
#include "lexer.h"
#include "parser.h"
#include "lerror.h"
#include "node_pool.h"
#include "assm_gen.h"

int main()
{
    node_pool_t pool = {0};
    node_pool_construct(&pool);

    char *buf = NULL;
    int data_size = create_buffer_from_file("prg.c", &buf);
    lexem_t *lexems = create_lexical_array(buf);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }
    
    ast_node_t *ast = ast_build(lexems, &pool);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }

    free(lexems);

    ast_visualize(ast);

    //assm_gen(ast, "test.assm");

    ast_destroy(ast, &pool);
    node_pool_destroy(&pool);
    free(buf);

    return 0;
}