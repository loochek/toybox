#include "utils/lstatus.hpp"
#include "utils/file_utils.hpp"
#include "utils/memory_pool.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "code_gen.hpp"

const int MAX_FILE_NAME = 31;

#define LSCHK_LOCAL(expr, num) { status = expr; if (status != LSTATUS_OK) CLEANUP(num); }
#define CLEANUP(num) goto cleanup##num

lstatus_t compile(const char *src_file_name,
            const char *elf_file_name,
            const char *lst_file_name,
            bool show_ast)
{
    lstatus_t status = LSTATUS_OK;

    char       *src       = nullptr;
    ast_node_t *tree_root = nullptr;

    memory_pool_t<ast_node_t> pool     = {};
    list_t<lexem_t>           lexems   = {};
    compilation_error_t       comp_err = {};

    LSCHK_LOCAL(memory_pool_construct(&pool), 0);

    LSCHK_LOCAL(create_buffer_from_file(src_file_name, &src), 1);

    LSCHK_LOCAL(list_construct(&lexems), 2);

    status = lexer_tokenize(src, &lexems, &comp_err);
    if (status == LSTATUS_LEXER_FAIL)
    {
        print_error(&comp_err);
        status = LSTATUS_OK;
        CLEANUP(3);
    }
    else if (status != LSTATUS_OK)
        CLEANUP(3);

    status = ast_build(&lexems, &pool, &comp_err, &tree_root);
    if (status == LSTATUS_PARSER_FAIL)
    {
        print_error(&comp_err);
        status = LSTATUS_OK;
        CLEANUP(3);
    }
    else if (status != LSTATUS_OK)
        CLEANUP(3);

    LSCHK_LOCAL(ast_optimize(tree_root, &tree_root, &pool), 4);

    if (show_ast)
        ast_visualize(tree_root);

    status = code_gen(tree_root, &comp_err, elf_file_name, lst_file_name);
    if (status == LSTATUS_CODE_GEN_FAIL)
    {
        print_error(&comp_err);
        status = LSTATUS_OK;
    }

cleanup4:
    LSCHK(ast_destroy(tree_root, &pool));

cleanup3:
    LSCHK(list_destruct(&lexems));

cleanup2:
    free(src);

cleanup1:
    LSCHK(memory_pool_construct(&pool));

cleanup0:
    return status;
}

const char *USAGE_STRING = "Usage: tc [-o <ELF file>] [--show-ast] [--about] <source file>\n";

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("%s", USAGE_STRING);
        return 0;
    }

    const char *src_file_name = nullptr;
    const char *elf_file_name = "a.out";

    char lst_file_name[MAX_FILE_NAME + 5] = {}; // +5 for .lst

    bool show_ast = false;

    int arg_cnt = 1;
    for (; arg_cnt < argc; arg_cnt++)
    {
        if (strcmp(argv[arg_cnt], "-o") == 0)
        {
            if (arg_cnt + 1 == argc)
            {
                printf("\u001b[31mError\u001b[0m: no file name specified after -o\n%s", USAGE_STRING);
                return -1;
            }

            arg_cnt++;
            elf_file_name = argv[arg_cnt];
        }
        else if (strcmp(argv[arg_cnt], "--show-ast") == 0)
            show_ast = true;
        else if (strcmp(argv[arg_cnt], "--about") == 0)
        {
            printf("Tiny Compilier 0.98\n"
                   "Copyleft loochek 2021\n");
            return 0;
        }
        else
            src_file_name = argv[arg_cnt];
    }

    if (src_file_name == nullptr)
    {
        printf("\u001b[31mError\u001b[0m: no source file specified\n%s", USAGE_STRING);
        return -1;
    }

    strncpy(lst_file_name, elf_file_name, MAX_FILE_NAME);
    strcat(lst_file_name, ".lst");

    lstatus_t status = compile(src_file_name, elf_file_name, lst_file_name, show_ast);
    if (status != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        return -1;
    }

    return 0;
}
