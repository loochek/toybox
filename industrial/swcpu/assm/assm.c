#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "../common/headers/arithm.h"
#include "../common/headers/global_constants.h"
#include "../common/headers/lerror.h"
#include "../common/headers/string_lib.h"
#include "../common/headers/prg.h"

#define MAX_LABEL_SIZE        20
#define MAX_LABEL_COUNT       100
#define MAX_PROGRAM_NAME_SIZE 19

typedef struct
{
    char label[MAX_LABEL_SIZE + 1];
    size_t offset;
} label_entry_t;

typedef struct
{
    label_entry_t *list;
    size_t         count;
} label_list_t;

typedef struct
{
    char *next_token;
    char  orig_sym;
} tokenizer_state_t;

static void tokenizer_init(char *str, tokenizer_state_t *state)
{
    state->next_token = str;
    state->orig_sym   = str[0];
}

static char* next_token(tokenizer_state_t *state)
{
    char *curr_token = state->next_token;
    *state->next_token = state->orig_sym;

    while (isspace(*state->next_token))
    {
        state->next_token++;
        curr_token++;
    }

    while (!(isspace(*state->next_token) ||
                *state->next_token == '\0'))
        state->next_token++;
    
    state->orig_sym = *state->next_token;
    *state->next_token = '\0';

    return curr_token;
}

static inline int strchr_rng(const char *str, char c, size_t l, size_t r)
{
    for (int i = l; i < r; i++)
        if (str[i] == c)
            return i;
    return -1;
}

typedef struct
{
    uint8_t  arg_mask;
    uint8_t  reg_num;
    bool     is_label_used;
    double   imm_val;
    char     imm_label[MAX_LABEL_SIZE + 1];
} arg_parsing_result_t;

static inline arg_parsing_result_t parse_subarg(const char subarg[])
{
    arg_parsing_result_t res = {0};
    if (strlen(subarg) == 0)
        return res;

    // check for register
    if (strlen(subarg) == 3 &&
        subarg[0] == 'r' && subarg[2] == 'x' &&
        subarg[1] >= 'a' && subarg[1] < 'a' + REGISTERS_COUNT)
    {
        res.arg_mask |= ARG_MASK_REGISTER;
        res.reg_num   = subarg[1] - 'a';
        return res;
    }
    // immediate
    res.arg_mask |= ARG_MASK_IMMEDIATE;

    double num = str_to_num(subarg);
    if (__lerrno != LERR_NAN)
    {
        res.imm_val = num;
        return res;
    }
    else
    {
        res.is_label_used = true;
        strncpy(res.imm_label, subarg, MAX_LABEL_SIZE); 
        return res;
    }
}

static inline arg_parsing_result_t merge_results(const arg_parsing_result_t a,
                                                 const arg_parsing_result_t b)
{
    arg_parsing_result_t res = {0};
    res.arg_mask |= a.arg_mask | b.arg_mask;
    if ((a.arg_mask & ARG_MASK_REGISTER) != 0)
        res.reg_num = a.reg_num;
    if ((b.arg_mask & ARG_MASK_REGISTER) != 0)
        res.reg_num = b.reg_num;
    if ((a.arg_mask & ARG_MASK_IMMEDIATE) != 0)
    {
        if (a.is_label_used)
        {
            res.is_label_used = true;
            strcpy(res.imm_label, a.imm_label);
        }
        else
            res.imm_val = a.imm_val;
    }
    if ((b.arg_mask & ARG_MASK_IMMEDIATE) != 0)
    {
        if (b.is_label_used)
        {
            res.is_label_used = true;
            strcpy(res.imm_label, b.imm_label);
        }
        else
            res.imm_val = b.imm_val;
    }
    return res;
}

static arg_parsing_result_t parse_argument(const char *arg_string)
{
    arg_parsing_result_t res = {0};
    
    size_t left_ptr = 0, right_ptr = strlen(arg_string) - 1;
    // check for brackets
    uint8_t ram = 0;
    if (arg_string[left_ptr] == '[' && arg_string[right_ptr] == ']')
    {
        ram |= ARG_MASK_RAM;
        left_ptr++;
        right_ptr--;
    }
    int delim_pos = strchr_rng(arg_string, '+', left_ptr, right_ptr);
    if (delim_pos != -1)
    {
        char subarg1[MAX_ARG_SIZE + 1] = {0},
             subarg2[MAX_ARG_SIZE + 1] = {0};
        strncpy(subarg1, arg_string + left_ptr     , delim_pos - left_ptr);
        strncpy(subarg2, arg_string + delim_pos + 1, right_ptr - delim_pos);
        res = merge_results(parse_subarg(subarg1), parse_subarg(subarg2));
    }
    else
    {
        char subarg[MAX_ARG_SIZE + 1] = {0};
        strncpy(subarg, arg_string + left_ptr, right_ptr - left_ptr + 1);
        res = parse_subarg(subarg);
    }
    res.arg_mask |= ram;
    return res;
}

static void create_label(const char name[], size_t offset, label_list_t *labels)
{
    strncpy(labels->list[labels->count].label, name, MAX_LABEL_SIZE);
    labels->list[labels->count].offset = offset;
    labels->count++;
}

static int check_for_extra_tokens(tokenizer_state_t *tokenizer_state)
{
    __lerrno = LERR_NO_ERROR;
    char* curr_tok = next_token(tokenizer_state);
    if (*curr_tok != '\0')
    {
        if (strncmp(curr_tok, ";", 1) != 0)
        {
            LERR(LERR_PARSING_FAILED, "extra tokens");
            return -1;
        }
    }
    return 0;
}

// INSTRUCTION definition for syntax_check
#define INSTRUCTION(mnemonic, base_opcode, argument_type, impl)                            \
if (strcmp(curr_tok, #mnemonic) == 0)                                                      \
{                                                                                          \
    byte_cnt += 1;                                                                         \
    if ((argument_type) != ARG_NONE)                                                       \
    {                                                                                      \
        curr_tok = next_token(&tokenizer_state);                                           \
        arg_parsing_result_t res = parse_argument(curr_tok);                               \
        if (res.arg_mask & (ARG_MASK_IMMEDIATE | ARG_MASK_RAM) == 0)                       \
        {                                                                                  \
            LERR(LERR_PARSING_FAILED, "bad or missing argument");                          \
            return -1;                                                                     \
        }                                                                                  \
        if (argument_type == ARG_LVALUE &&                                                 \
            (res.arg_mask == (ARG_MASK_IMMEDIATE | ARG_MASK_REGISTER) ||                   \
             res.arg_mask == ARG_MASK_IMMEDIATE))                                          \
        {                                                                                  \
            LERR(LERR_PARSING_FAILED, "expected lvalue");                                  \
            return -1;                                                                     \
        }                                                                                  \
        if ((res.arg_mask & ARG_MASK_REGISTER) != 0)                                       \
            byte_cnt += 1;                                                                 \
        if ((res.arg_mask & ARG_MASK_IMMEDIATE) != 0)                                      \
            byte_cnt += IMM_SIZE;                                                          \
    }                                                                                      \
    if (check_for_extra_tokens(&tokenizer_state) != 0)                                     \
    {                                                                                      \
        return -1;                                                                         \
    }                                                                                      \
    return byte_cnt;                                                                       \
}

static int parse_line(char *line, size_t byte_offset, label_list_t* labels)
{
    __lerrno = LERR_NO_ERROR;
    int byte_cnt = 0;

    tokenizer_state_t tokenizer_state = {0};
    tokenizer_init(line, &tokenizer_state);
    char *curr_tok = next_token(&tokenizer_state);

    // if line is empty
    if (*curr_tok == '\0')
        return 0;
    // if whole line is the comment
    if (strncmp(curr_tok, ";", 1) == 0)
        return 0;
    // actual CPU commands
    #include "../cpu_def.h"
    // if anything didn't match, test for label:
    size_t tok_len = strlen(curr_tok);
    if (curr_tok[tok_len - 1] == ':')
    {
        char label[MAX_LABEL_SIZE + 1] = {0};
        strncpy(label, curr_tok, tok_len - 1);
        create_label(label, byte_offset, labels);
        if (check_for_extra_tokens(&tokenizer_state) != 0)
            return -1;
        return byte_cnt;
    }
    LERR(LERR_PARSING_FAILED, "bad token");
    return -1;
}

static int parse_program(string_index_t *prg_text, label_list_t* labels)
{
    int byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        byte_cnt += parse_line(prg_text->string_entries[line].begin, byte_cnt, labels);
        if (__lerrno == LERR_PARSING_FAILED)
        {
            printf("Error on line %zu: %s\n%s\n", line + 1, __lerr_str,
                                                  prg_text->string_entries[line].begin);
            return -1;
        }
    }
    return byte_cnt;
}

#undef INSTRUCTION
// INSTRUCTION definition for compile_line
#define INSTRUCTION(mnemonic, base_opcode, argument_type, impl)                            \
if (strcmp(curr_tok, #mnemonic) == 0)                                                      \
{                                                                                          \
    if ((argument_type) == ARG_NONE)                                                       \
        code[byte_cnt++] = base_opcode;                                                    \
    else                                                                                   \
    {                                                                                      \
        curr_tok = next_token(&tokenizer_state);                                           \
        arg_parsing_result_t res = parse_argument(curr_tok);                               \
        code[byte_cnt++] = base_opcode | res.arg_mask;                                     \
        if ((res.arg_mask & ARG_MASK_REGISTER) != 0)                                       \
            code[byte_cnt++] = res.reg_num;                                                \
        if ((res.arg_mask & ARG_MASK_IMMEDIATE) != 0)                                      \
        {                                                                                  \
            if (res.is_label_used)                                                         \
            {                                                                              \
                bool wrote = false;                                                        \
                for (size_t i = 0; i < labels->count; i++)                                 \
                {                                                                          \
                    if (strcmp(labels->list[i].label, res.imm_label) == 0)                 \
                    {                                                                      \
                        double offset_d = labels->list[i].offset;                          \
                        memcpy(&code[byte_cnt], &offset_d, IMM_SIZE);                      \
                        byte_cnt += IMM_SIZE;                                              \
                        wrote = true;                                                      \
                    }                                                                      \
                }                                                                          \
                if (!wrote)                                                                \
                {                                                                          \
                    LERR(LERR_UNKNOWN_LABEL, res.imm_label);                               \
                    return -1;                                                             \
                }                                                                          \
            }                                                                              \
            else                                                                           \
            {                                                                              \
                memcpy(&code[byte_cnt], &res.imm_val, IMM_SIZE);                           \
                byte_cnt += IMM_SIZE;                                                      \
            }                                                                              \
        }                                                                                  \
    }                                                                                      \
    return byte_cnt;                                                                       \
}

static int compile_line(char *line, size_t byte_offset, label_list_t* labels, char *code)
{
    __lerrno = LERR_NO_ERROR;
    int byte_cnt = byte_offset;

    tokenizer_state_t tokenizer_state = {0};
    tokenizer_init(line, &tokenizer_state);
    char *curr_tok = next_token(&tokenizer_state);

    // if line is empty
    if (*curr_tok == '\0')
        return byte_cnt;
    // if whole line is the comment
    if (strncmp(curr_tok, ";", 1) == 0)
        return byte_cnt;
    // actual CPU commands
    #include "../cpu_def.h"

    return byte_cnt;
}

static int compile(string_index_t *prg_text, label_list_t* labels, char *code)
{
    size_t byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        byte_cnt = compile_line(prg_text->string_entries[line].begin, byte_cnt, labels, code);
        if (__lerrno == LERR_UNKNOWN_LABEL)
        {
            printf("Error on line %zu: unknown label %s\n%s\n", line + 1, __lerr_str,
                                                  prg_text->string_entries[line].begin);
            return -1;
        }
    }
    return byte_cnt;
}

static inline int write_binary(const char *code, size_t code_size,
                               const char *file_name, const char *prg_name)
{
    prg_header_t prg_header = {0};
    prg_header.signature[0] = 'L';
    prg_header.signature[1] = 'P';
    prg_header.signature[2] = 'R';
    prg_header.signature[3] = 'G';
    prg_header.format_ver = 1;
    prg_header.code_size = code_size;
    strncpy(prg_header.program_name, prg_name, MAX_PROGRAM_NAME_SIZE);

    FILE *output_file = fopen(file_name, "wb");
    if (output_file == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");
        return -1;
    }

    int write_status = (fwrite(&prg_header, sizeof(prg_header_t), 1, output_file) != 1) ||
                       (fwrite(code, sizeof(char), code_size, output_file) != code_size);
    if (write_status)
    {
        fprintf(stderr, "Unable to write to output file\n");
        fclose(output_file);
        return -1;
    }
    fclose(output_file);
    return 0;
}

int create_binary(string_index_t *prg_text, char** code)
{
    label_list_t labels = {0};
    labels.list = calloc(MAX_LABEL_COUNT, sizeof(label_entry_t));
    
    int byte_cnt = parse_program(prg_text, &labels);
    if (byte_cnt == -1)
    {
        free(labels.list);
        return -1;
    }

    *code = calloc(byte_cnt, sizeof(char));
    int compile_status = compile(prg_text, &labels, *code);

    free(labels.list);
    if (compile_status == -1)
    {
        free(*code);
        return -1;
    }
    
    return byte_cnt;
}

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

    char *code = NULL;
    int byte_cnt = create_binary(prg_text, &code);
    destroy_index(prg_text);
    if (byte_cnt == -1)
        return -1;

    if (write_binary(code, byte_cnt, out_file_name, prg_name) == -1)
    {
        free(code);
        return -1;
    }

    free(code);
    return 0;
}