#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../common/headers/string_lib.h"
#include "../common/headers/arithm.h"
#include "../common/headers/opcodes.h"
#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"

/*

The Assembler

Assembly takes place in two passes. At the first, the syntax is checked
and the size of binary code is calculated. On the second, binary code is generated.

*/

#define ARG_TYPE_NO_ARG    0
#define ARG_TYPE_REGISTER  1
#define ARG_TYPE_IMMEDIATE 2

// INSTRUCTION definition for syntax_check
#define INSTRUCTION(mnemonic, base_opcode, argument_type, impl)                            \
if (strcmp(curr_tok, #mnemonic) == 0)                                                      \
{                                                                                          \
    byte_cnt += 1;                                                                         \
    if ((argument_type) != ARG_TYPE_NO_ARGS)                                               \
    {                                                                                      \
        curr_tok = strtok(NULL, " ");                                                      \
        if (curr_tok == NULL)                                                              \
        {                                                                                  \
            printf("Error on line %zu: " #mnemonic " expected argument:\n%s\n", line + 1,  \
                        prg_text_immut->string_entries[line].begin);                       \
            return -1;                                                                     \
        }                                                                                  \
        if (((argument_type) & ARG_TYPE_REGISTER) != 0)                                    \
        {                                                                                  \
            if (strcmp(curr_tok, "rax") == 0 ||                                            \
                strcmp(curr_tok, "rbx") == 0 ||                                            \
                strcmp(curr_tok, "rcx") == 0 ||                                            \
                strcmp(curr_tok, "rdx") == 0)                                              \
                continue;                                                                  \
        }                                                                                  \
        if (((argument_type) & ARG_TYPE_IMMEDIATE) != 0)                                   \
        {                                                                                  \
            byte_cnt += 4;                                                                 \
            str_to_num(curr_tok);                                                          \
            if (__lerrno != LERR_NAN)                                                      \
                continue;                                                                  \
        }                                                                                  \
        printf("Error on line %zu: bad argument for " #mnemonic ":\n%s\n", line + 1,       \
                        prg_text_immut->string_entries[line].begin);                       \
        return -1;                                                                         \
    }                                                                                      \
    curr_tok = strtok(NULL, " ");                                                          \
    if (curr_tok != NULL)                                                                  \
    {                                                                                      \
        printf("Error on line %zu: extra tokens:\n%s\n", line + 1,                         \
                        prg_text_immut->string_entries[line].begin);                       \
        return -1;                                                                         \
    }                                                                                      \
    continue;                                                                              \
}

// checks syntax errors and calculates binary code size
// There are two different prg_text because strtok corrupts data, 
// but the function needs the original program text to output the line with the error
static int syntax_check(string_index_t *prg_text, string_index_t *prg_text_immut)
{
    size_t byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");
        // if the token is the start of the comment
        if (strncmp(curr_tok, ";", 1) == 0)
            continue;
        // actual CPU commands
        #include "../cpu_def.h"
        // if anything didn't match
        printf("Error on line %zu: bad token:\n%s\n", line + 1,
                prg_text_immut->string_entries[line].begin);
        return -1;
    }
    return byte_cnt;
}

#undef INSTRUCTION

// INSTRUCTION definition for compile
#define INSTRUCTION(mnemonic, base_opcode, argument_type, impl)                            \
if (strcmp(curr_tok, #mnemonic) == 0)                                                      \
{                                                                                          \
    if ((argument_type) == ARG_TYPE_NO_ARGS)                                               \
    {                                                                                      \
        output_buf[pc++] = base_opcode;                                                    \
        continue;                                                                          \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
        curr_tok = strtok(NULL, " ");                                                      \
        if (((argument_type) & ARG_TYPE_REGISTER) != 0)                                    \
        {                                                                                  \
            if (strcmp(curr_tok, "rax") == 0)                                              \
            {                                                                              \
                output_buf[pc++] = base_opcode;                                            \
                continue;                                                                  \
            }                                                                              \
            else if (strcmp(curr_tok, "rbx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 1;                                        \
                continue;                                                                  \
            }                                                                              \
            else if (strcmp(curr_tok, "rcx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 2;                                        \
                continue;                                                                  \
            }                                                                              \
            else if (strcmp(curr_tok, "rdx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 3;                                        \
                continue;                                                                  \
            }                                                                              \
        }                                                                                  \
        if (((argument_type) & ARG_TYPE_IMMEDIATE) != 0)                                   \
        {                                                                                  \
            int32_t imm_val = str_to_num(curr_tok);                                        \
            output_buf[pc++] = base_opcode + 4;                                            \
            output_buf[pc++] =  imm_val        & 0xFF;                                     \
            output_buf[pc++] = (imm_val >>  8) & 0xFF;                                     \
            output_buf[pc++] = (imm_val >> 16) & 0xFF;                                     \
            output_buf[pc++] = (imm_val >> 24) & 0xFF;                                     \
            continue;                                                                      \
        }                                                                                  \
    }                                                                                      \
}

// generates binary output
static void compile(string_index_t *prg_text, char *output_buf)
{
    size_t pc = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");
        #include "../cpu_def.h"
    }
}

#undef INSTRUCTION

int main(int argc, char* argv[])
{
    char* src_file_name = NULL;
    char prg_name[100] = "COMPILIER_DEFAULT";
    if (argc < 2)
    {
        printf("Usage: assm <source file> [<output program name>]\n");
        return 0;
    }
    else if (argc == 2)
        src_file_name = argv[1];
    else if (argc == 3)
    {
        src_file_name = argv[1];
        strcpy(prg_name, argv[2]);
        strcat(prg_name, ".prg");
    }

    string_index_t *prg_text  = create_index_from_file(src_file_name);
    if (prg_text == NULL)
    {
        LERRPRINT();
        destroy_index(prg_text);
        return -1;
    }
    string_index_t *prg_text2  = create_index_from_file(src_file_name);
    size_t byte_cnt = syntax_check(prg_text, prg_text2);
    destroy_index(prg_text);
    
    if (byte_cnt == -1)
    {
        destroy_index(prg_text2);
        return -1;
    }

    char *code = calloc(byte_cnt, sizeof(char));
    compile(prg_text2, code);
    destroy_index(prg_text2);

    prg_header_t prg_header = {0};
    prg_header.signature[0] = 'L';
    prg_header.signature[1] = 'P';
    prg_header.signature[2] = 'R';
    prg_header.signature[3] = 'G';
    prg_header.format_ver = 1;
    strcpy(prg_header.program_name, prg_name);
    prg_header.code_size = byte_cnt;

    FILE *output_file = fopen(argc == 3 ? prg_name : "a.prg", "wb");
    if (output_file == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");
        free(code);
        return -1;
    }
    if (fwrite(&prg_header, sizeof(prg_header_t), 1, output_file) != 1)
    {
        fprintf(stderr, "Unable to write to output file\n");
        free(code);
        fclose(output_file);
        return -1;
    }
    if (fwrite(code, sizeof(char), byte_cnt, output_file) != byte_cnt)
    {
        fprintf(stderr, "Unable to write to output file\n");
        free(code);
        fclose(output_file);
        return -1;
    }
    fclose(output_file);
    free(code);
    return 0;
}
