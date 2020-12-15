#include "file_utils.h"
#include "lexer.h"
#include "parser.h"
#include "lerror.h"
#include "node_pool.h"
#include "assm_gen.h"

#include "stack/stack_common.h"

#define TYPE lexem
#define elem_t lexem_t
#include "stack/stack.h"
#undef elem_t
#undef TYPE

void compile(node_poo)
{
    LERR_RESET();

    node_pool_t pool = {0};
    node_pool_construct(&pool);

    char *buf = NULL;
    int data_size = create_buffer_from_file("examples/equ.tc", &buf);
    if (LERR_PRESENT())
        return;

    my_stack_lexem lexems = {0};
    STACK_ERROR_CHECK_RET(stack_construct_lexem(&lexems, 5),);

    create_lexical_array(buf, &lexems);
    if (LERR_PRESENT())
        return;
    
    ast_node_t *ast = ast_build(lexems.data, &pool);
    if (LERR_PRESENT())
        return;

    STACK_ERROR_CHECK_RET(stack_destruct_lexem(&lexems),);

    ast_visualize(ast);

    assm_gen(ast, "test.assm");
    if (LERR_PRESENT())
        return;

    ast_destroy(ast, &pool);
    node_pool_destroy(&pool);
    free(buf);
}

int main()
{
    compile();
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }

    return 0;
}