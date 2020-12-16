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

#define ERROR_CHECK() if (LERR_PRESENT()) goto cleanup;

void translate(const char *src_file_name, const char *assm_file_name)
{
    LERR_RESET();

    node_pool_t    pool     = {0};
    my_stack_lexem lexems   = {0};
    ast_node_t    *ast      = NULL;
    char          *prg_text = NULL;

    node_pool_construct(&pool);
    ERROR_CHECK();

    int data_size = create_buffer_from_file(src_file_name, &prg_text);
    ERROR_CHECK();

    
    STACK_LERR(stack_construct_lexem(&lexems, 5));
    ERROR_CHECK();

    create_lexical_array(prg_text, &lexems);
    ERROR_CHECK();
    
    ast = ast_build(lexems.data, &pool);
    ERROR_CHECK();

    assm_gen(ast, assm_file_name);

cleanup:
    STACK_LERR(stack_destruct_lexem(&lexems));
    ast_destroy(ast, &pool);
    node_pool_destroy(&pool);
    free(prg_text);
}

#undef ERROR_CHECK

#define MAX_CMDLINE_LENGTH 100

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./tc <source file> [output file]\n");
        return 0;
    }

    const char *out_file_name = "a.prg";
    if (argc >= 3)
        out_file_name = argv[2];

    system("mkdir -p .tmp");

    translate(argv[1], ".tmp/tmp.assm");
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }
    
    system("cat start.assm .tmp/tmp.assm > .tmp/tmp1.assm");

    char assm_string[MAX_CMDLINE_LENGTH + 1];
    snprintf(assm_string, MAX_CMDLINE_LENGTH, "./bin/assm .tmp/tmp1.assm %s", out_file_name);
    system(assm_string);
    system("rm -rf .tmp");
    return 0;
}