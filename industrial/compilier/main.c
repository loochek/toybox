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

void translate(const char *src_file_name, const char *assm_file_name, bool show_ast)
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

    if (show_ast)
        ast_visualize(ast);

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
        printf("Usage: ./tc <source file> [-O<output file>] [--translate-only] [--show-ast]\n");
        return 0;
    }

    bool translate_only = false, show_ast = false;

    const char *src_file_name = argv[1];
    const char *out_file_name = "a.prg";

    for (int i = 2; i < argc; i++)
    {
        if (strncmp(argv[i], "-O", 2) == 0)
            out_file_name = argv[i] + 2;
        else if (strcmp(argv[i], "--translate-only") == 0)
            translate_only = true;
        else if (strcmp(argv[i], "--show-ast") == 0)
            show_ast = true;
        else
        {
            printf("Unknown parameter: %s\n", argv[i]);
            return -1;
        }
    }

    system("mkdir -p .tmp");

    translate(src_file_name, ".tmp/translated.assm", show_ast);
    if (LERR_PRESENT())
    {
        printf("Error: %s\n", __lerr_str);
        return -1;
    }

    system("cat start.assm .tmp/translated.assm > .tmp/translated_with_runtime.assm");

    char cmdline[MAX_CMDLINE_LENGTH + 1];

    if (translate_only)
    {
        snprintf(cmdline, MAX_CMDLINE_LENGTH,
                 "cp .tmp/translated_with_runtime.assm %s.assm", out_file_name);
    }
    else
    {
        snprintf(cmdline, MAX_CMDLINE_LENGTH,
                 "./bin/assm .tmp/translated_with_runtime.assm %s", out_file_name);
    }

    system(cmdline);

    system("rm -rf .tmp");
    return 0;
}