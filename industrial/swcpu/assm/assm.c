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

typedef struct
{
    char label[21];
    size_t offset;
} label_entry_t;

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
                goto line_parsed;                                                          \
        }                                                                                  \
        byte_cnt += 4;                                                                     \
        if (((argument_type) & ARG_TYPE_IMMEDIATE) != 0)                                   \
        {                                                                                  \
            str_to_num(curr_tok);                                                          \
            if (__lerrno != LERR_NAN)                                                      \
                goto line_parsed;                                                          \
        }                                                                                  \
        if (((argument_type) & ARG_TYPE_LABEL) != 0)                                       \
            goto line_parsed;                                                              \
        printf("Error on line %zu: bad argument for " #mnemonic ":\n%s\n", line + 1,       \
                        prg_text_immut->string_entries[line].begin);                       \
        return -1;                                                                         \
    }                                                                                      \
    goto line_parsed;                                                                      \
}

// checks syntax errors, calculates binary code size and parses labels
// There are two different prg_text because strtok corrupts data, 
// but the function needs the original program text to output the line with the error
static int syntax_check(string_index_t *prg_text, string_index_t *prg_text_immut,
                        label_entry_t *labels, size_t *label_cnt)
{
    size_t byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");

        // if current string is empty
        if (curr_tok == NULL)
            continue;

        // if the token is the start of the comment
        if (strncmp(curr_tok, ";", 1) == 0)
            continue;

        // actual CPU commands
        #include "../cpu_def.h"

        // if anything didn't match, test for label:
        size_t tok_len = strlen(curr_tok);
        if (curr_tok[tok_len - 1] == ':')
        {
            curr_tok[tok_len - 1] = '\0';
            strcpy(labels[*label_cnt].label, curr_tok);
            labels[*label_cnt].offset = byte_cnt;
            (*label_cnt)++;
            goto line_parsed;
        }

        // else
        printf("Error on line %zu: bad token:\n%s\n", line + 1,
                prg_text_immut->string_entries[line].begin);
        return -1;

// this code checks for extra tokens  (it treated as error)
line_parsed:
        curr_tok = strtok(NULL, " ");
        if (curr_tok != NULL)
        {
            printf("Error on line %zu: extra tokens:\n%s\n", line + 1,
                            prg_text_immut->string_entries[line].begin);
            return -1;
        }
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
        goto success;                                                                      \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
        curr_tok = strtok(NULL, " ");                                                      \
        if (((argument_type) & ARG_TYPE_REGISTER) != 0)                                    \
        {                                                                                  \
            if (strcmp(curr_tok, "rax") == 0)                                              \
            {                                                                              \
                output_buf[pc++] = base_opcode;                                            \
                goto success;                                                              \
            }                                                                              \
            else if (strcmp(curr_tok, "rbx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 1;                                        \
                goto success;                                                              \
            }                                                                              \
            else if (strcmp(curr_tok, "rcx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 2;                                        \
                goto success;                                                              \
            }                                                                              \
            else if (strcmp(curr_tok, "rdx") == 0)                                         \
            {                                                                              \
                output_buf[pc++] = base_opcode + 3;                                        \
                goto success;                                                              \
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
            goto success;                                                                  \
        }                                                                                  \
        if (((argument_type) & ARG_TYPE_LABEL) != 0)                                       \
        {                                                                                  \
            for (size_t i = 0; i < label_cnt; i++)                                         \
            {                                                                              \
                if (strcmp(labels[i].label, curr_tok) == 0)                                \
                {                                                                          \
                    int32_t imm_val = labels[i].offset;                                    \
                    output_buf[pc++] = base_opcode;                                        \
                    output_buf[pc++] =  imm_val        & 0xFF;                             \
                    output_buf[pc++] = (imm_val >>  8) & 0xFF;                             \
                    output_buf[pc++] = (imm_val >> 16) & 0xFF;                             \
                    output_buf[pc++] = (imm_val >> 24) & 0xFF;                             \
                    goto success;                                                          \
                }                                                                          \
            }                                                                              \
            printf("Error on line %zu: unknown label %s\n", line + 1, curr_tok);           \
            return -1;                                                                     \
        }                                                                                  \
    }                                                                                      \
}

// generates binary output
// if program was compiled succesful returns 0
static int compile(string_index_t *prg_text, char *output_buf,
                    label_entry_t *labels, size_t label_cnt)
{
    size_t pc = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");

        // if current string is empty
        if (curr_tok == NULL)
            continue;

        #include "../cpu_def.h"

success:
        continue;
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
    }

    string_index_t *prg_text  = create_index_from_file(src_file_name);
    if (prg_text == NULL)
    {
        LERRPRINT();
        destroy_index(prg_text);
        return -1;
    }

    string_index_t *prg_text2  = create_index_from_file(src_file_name);
    label_entry_t *labels = calloc(100, sizeof(label_entry_t));
    size_t label_cnt = 0;
    size_t byte_cnt = syntax_check(prg_text, prg_text2, labels, &label_cnt);
    destroy_index(prg_text);
    
    if (byte_cnt == -1)
    {
        destroy_index(prg_text2);
        return -1;
    }

    char *code = calloc(byte_cnt, sizeof(char));
    int status = compile(prg_text2, code, labels, label_cnt);
    free(labels);
    destroy_index(prg_text2);
    if (status == -1)
    {
        free(code);
        return 0;
    }

    prg_header_t prg_header = {0};
    prg_header.signature[0] = 'L';
    prg_header.signature[1] = 'P';
    prg_header.signature[2] = 'R';
    prg_header.signature[3] = 'G';
    prg_header.format_ver = 1;
    strcpy(prg_header.program_name, prg_name);
    prg_header.code_size = byte_cnt;

    strcat(prg_name, ".prg");
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
