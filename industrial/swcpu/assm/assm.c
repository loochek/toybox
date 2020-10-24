#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "../common/headers/string_lib.h"
#include "../common/headers/arithm.h"
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

static int syntax_check(string_index_t *prg_text, string_index_t *prg_text_immut,
                        label_entry_t *labels, size_t *label_cnt);
static int compile(string_index_t *prg_text, char *output_buf,
                    label_entry_t *labels, size_t label_cnt);

int main(int argc, char* argv[])
{
    char* src_file_name = NULL;
    char* out_file_name = "a.prg";
    char* prg_name = "ASSM_DEFAULT_NAME";
    if (argc < 2)
    {
        printf("Usage: assm <source file> [<output file name>] [<program name>]\n");
        return 0;
    }
    if (argc >= 2)
        src_file_name = argv[1];
    if (argc >= 3)
        out_file_name = argv[2];
    if (argc >= 4)
        prg_name      = argv[3];

    string_index_t *prg_text  = create_index_from_file(src_file_name);
    if (prg_text == NULL)
    {
        LERRPRINT();
        return -1;
    }

    string_index_t *prg_text2 = create_index_from_file(src_file_name);
    label_entry_t  *labels    = calloc(100, sizeof(label_entry_t));
    size_t          label_cnt = 0;
    
    size_t byte_cnt = syntax_check(prg_text, prg_text2, labels, &label_cnt);
    destroy_index(prg_text);
    
    if (byte_cnt == -1)
    {
        destroy_index(prg_text2);
        free(labels);
        return -1;
    }

    char *code = calloc(byte_cnt, sizeof(char));
    int compile_status = compile(prg_text2, code, labels, label_cnt);
    free(labels);
    destroy_index(prg_text2);
    if (compile_status == -1)
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
    prg_header.code_size = byte_cnt;
    strncpy(prg_header.program_name, prg_name, 19);

    FILE *output_file = fopen(out_file_name, "wb");
    if (output_file == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");
        free(code);
        return -1;
    }

    int write_status = (fwrite(&prg_header, sizeof(prg_header_t), 1, output_file) != 1) ||
                 (fwrite(code, sizeof(char), byte_cnt, output_file) != byte_cnt);
    if (write_status)
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

typedef enum
{
    ARG_NONE,
    ARG_RVALUE,
    ARG_LVALUE
} arg_t;

#define ARG_MASK_REGISTER  1
#define ARG_MASK_IMMEDIATE 2
#define ARG_MASK_RAM       4

typedef struct
{
    uint8_t  arg_mask;
    uint8_t  reg_num;
    bool     is_label_used;
    double   imm_val;
    char     imm_label[21];
} parsing_result_t;

static parsing_result_t parse_argument(char *arg_string_orig, arg_t arg_type, size_t line_num)
{
    char *strtok_context = NULL;
    parsing_result_t res = {0};
    if (arg_string_orig == NULL)
    {
        printf("Error on line %zu: expected argument:\n%s\n", line_num, arg_string_orig);
        return res;
    }
    // create copy
    char arg_string[51] = {0};
    strncpy(arg_string, arg_string_orig, 50);
    int arg_len = strlen(arg_string);
    
    char *curr_arg_tok = arg_string;
    // if in brackets
    if (arg_string[0] == '[' && arg_string[arg_len - 1] == ']')
    {
        res.arg_mask |= ARG_MASK_RAM;
        arg_string[0] = '\0';
        arg_string[arg_len - 1] = '\0';
        curr_arg_tok++;
    }
    // start parsing
    curr_arg_tok = __strtok_r(curr_arg_tok, " +", &strtok_context);
    // check for register
    if (strlen(curr_arg_tok) == 3 && curr_arg_tok[0] == 'r' && curr_arg_tok[2] == 'x')
    {
        res.arg_mask |= ARG_MASK_REGISTER;
        res.reg_num = curr_arg_tok[1] - 'a';
        curr_arg_tok = __strtok_r(NULL, " +", &strtok_context);
    }
    // check for immediate number
    double imm_val = str_to_num(curr_arg_tok);
    if (__lerrno != LERR_NAN)
    {
        res.arg_mask |= ARG_MASK_IMMEDIATE;
        res.imm_val = imm_val;
        curr_arg_tok = __strtok_r(NULL, " +", &strtok_context);
    }
    // if label
    if (curr_arg_tok != NULL)
    {
        strcpy(res.imm_label, curr_arg_tok);
        curr_arg_tok = __strtok_r(NULL, " ", &strtok_context);
        if (curr_arg_tok != NULL)
        {
            res.arg_mask = 0;
            printf("Error on line %zu: can't parse argument string:\n%s\n", line_num, arg_string_orig);
            return res;
        }
        else
        {
            res.arg_mask |= ARG_MASK_IMMEDIATE;
            res.is_label_used = true;
        }
    }
    if (arg_type == ARG_RVALUE)
    {
        if (res.arg_mask == 0 || res.arg_mask == 4)
        {
            res.arg_mask = 0;
            printf("Error on line %zu: argument is not rvalue:\n%s\n", line_num, arg_string_orig);
            return res;
        }
    }
    else if (arg_type == ARG_LVALUE)
    {
        if (res.arg_mask == 0 || res.arg_mask == 4 || res.arg_mask == 2 || res.arg_mask == 3)
        {
            res.arg_mask = 0;
            printf("Error on line %zu: argument is not lvalue:\n%s\n", line_num, arg_string_orig);
            return res;
        }
    }
    return res;
}

// INSTRUCTION definition for syntax_check
#define INSTRUCTION(mnemonic, base_opcode, argument_type, impl)                            \
if (strcmp(curr_tok, #mnemonic) == 0)                                                      \
{                                                                                          \
    byte_cnt += 1;                                                                         \
    if ((argument_type) != ARG_NONE)                                                       \
    {                                                                                      \
        curr_tok = __strtok_r(NULL, " ", &strtok_context);                                 \
        parsing_result_t res = parse_argument(curr_tok, argument_type, line + 1);          \
        if (res.arg_mask == 0)                                                             \
           return -1;                                                                      \
        if ((res.arg_mask & ARG_MASK_REGISTER) != 0)                                       \
            byte_cnt += 1;                                                                 \
        if ((res.arg_mask & ARG_MASK_IMMEDIATE) != 0)                                      \
            byte_cnt += 8;                                                                 \
    }                                                                                      \
    goto line_parsed;                                                                      \
}

// checks syntax errors, calculates binary code size and parses labels
// There are two different prg_text because __strtok_r corrupts data, 
// but the function needs the original program text to output the line with the error
static int syntax_check(string_index_t *prg_text, string_index_t *prg_text_immut,
                        label_entry_t *labels, size_t *label_cnt)
{
    char *strtok_context = NULL;
    size_t byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = __strtok_r(curr_tok, " ", &strtok_context);

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
        curr_tok = __strtok_r(NULL, " ", &strtok_context);
        if (curr_tok != NULL)
        {
            // if the token is the start of the comment
            if (strncmp(curr_tok, ";", 1) == 0)
                continue;
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
    if ((argument_type) == ARG_NONE)                                                       \
        output_buf[pc++] = base_opcode;                                                    \
    else                                                                                   \
    {                                                                                      \
        curr_tok = __strtok_r(NULL, " ", &strtok_context);                                 \
        parsing_result_t res = parse_argument(curr_tok, argument_type, line + 1);          \
        output_buf[pc++] = base_opcode | res.arg_mask;                                     \
        if ((res.arg_mask & ARG_MASK_REGISTER) != 0)                                       \
            output_buf[pc++] = res.reg_num;                                                \
        if ((res.arg_mask & ARG_MASK_IMMEDIATE) != 0)                                      \
        {                                                                                  \
            if (res.is_label_used)                                                         \
            {                                                                              \
                bool wrote = false;                                                        \
                for (size_t i = 0; i < label_cnt; i++)                                     \
                {                                                                          \
                    if (strcmp(labels[i].label, res.imm_label) == 0)                       \
                    {                                                                      \
                        int64_t imm_val = 0;                                               \
                        double off_double = labels[i].offset;                              \
                        memcpy(&imm_val, &off_double, sizeof(double));                     \
                        output_buf[pc++] =  imm_val        & 0xFF;                         \
                        output_buf[pc++] = (imm_val >>  8) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 16) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 24) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 32) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 40) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 48) & 0xFF;                         \
                        output_buf[pc++] = (imm_val >> 56) & 0xFF;                         \
                        wrote = true;                                                      \
                    }                                                                      \
                }                                                                          \
                if (!wrote)                                                                \
                {                                                                          \
                    printf("Error on line %zu: unknown label %s\n", line + 1, res.imm_label); \
                    return -1;                                                             \
                }                                                                          \
            }                                                                              \
            else                                                                           \
            {                                                                              \
                int64_t imm_val = 0;                                                   \
                memcpy(&imm_val, &res.imm_val, sizeof(double));                        \
                output_buf[pc++] =  imm_val        & 0xFF;                             \
                output_buf[pc++] = (imm_val >>  8) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 16) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 24) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 32) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 40) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 48) & 0xFF;                             \
                output_buf[pc++] = (imm_val >> 56) & 0xFF;                             \
            }                                                                              \
        }                                                                                  \
    }                                                                                      \
    continue;                                                                              \
}

// generates binary output
// if program was compiled succesful returns 0
static int compile(string_index_t *prg_text, char *output_buf,
                    label_entry_t *labels, size_t label_cnt)
{
    char *strtok_context = NULL;
    size_t pc = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = __strtok_r(curr_tok, " ", &strtok_context);

        // if current string is empty
        if (curr_tok == NULL)
            continue;

        #include "../cpu_def.h"
    }
}

#undef INSTRUCTION