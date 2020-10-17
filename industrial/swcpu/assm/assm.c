#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../common/headers/string_lib.h"
#include "../common/headers/arithm.h"
#include "../common/headers/opcodes.h"
#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"

static int syntax_check(string_index_t *prg_text, string_index_t *prg_text_immut)
{
    size_t byte_cnt = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");
        for (; curr_tok != NULL; curr_tok = strtok(NULL, " "))
        {
            if (strcmp(curr_tok, "push") == 0)
            {
                byte_cnt += 1;
                // parsing argument
                curr_tok = strtok(NULL, " ");
                if (curr_tok == NULL)
                {
                    printf("Error on line %zu: push requires argument:\n%s\n", line + 1,
                            prg_text_immut->string_entries[line].begin);
                    return -1;
                }
                // testing for registers
                if (strcmp(curr_tok, "rax") == 0)
                    continue;
                else if (strcmp(curr_tok, "rbx") == 0)
                    continue;
                else if (strcmp(curr_tok, "rcx") == 0)
                    continue;
                else if (strcmp(curr_tok, "rdx") == 0)
                    continue;

                byte_cnt += 4;
                // testing for number
                str_to_num(curr_tok);
                if (__lerrno == LERR_NAN)
                {
                    printf("Error on line %zu: invalid argument for push:\n%s\n", line + 1,
                            prg_text_immut->string_entries[line].begin);
                    return -1;
                }
                continue;
            }
            else if (strcmp(curr_tok, "pop") == 0)
            {
                byte_cnt++;
                // parsing argument
                curr_tok = strtok(NULL, " ");
                if (curr_tok == NULL)
                {
                    printf("Error on line %zu: push requires argument:\n%s\n", line + 1,
                            prg_text_immut->string_entries[line].begin);
                    return -1;
                }
                // testing for registers
                if (strcmp(curr_tok, "rax") == 0)
                    continue;
                else if (strcmp(curr_tok, "rbx") == 0)
                    continue;
                else if (strcmp(curr_tok, "rcx") == 0)
                    continue;
                else if (strcmp(curr_tok, "rdx") == 0)
                    continue;
                
                printf("Error on line %zu: invalid argument for pop:\n%s\n", line + 1,
                        prg_text_immut->string_entries[line].begin);
                return -1;
            }
            else if (strcmp(curr_tok, "nop") == 0 ||
                     strcmp(curr_tok, "in")  == 0 ||
                     strcmp(curr_tok, "out") == 0 ||
                     strcmp(curr_tok, "add") == 0 ||
                     strcmp(curr_tok, "sub") == 0 ||
                     strcmp(curr_tok, "mul") == 0 ||
                     strcmp(curr_tok, "div") == 0 ||
                     strcmp(curr_tok, "hlt") == 0)
            {
                byte_cnt++;
                continue;
            }
            else if (strncmp(curr_tok, ";", 1) == 0)
                break;
            else
            {
                printf("Error on line %zu: bad token:\n%s\n", line + 1,
                        prg_text_immut->string_entries[line].begin);
                return -1;
            }
        }
    }
    return byte_cnt;
}

static void compile(string_index_t *prg_text, char *output_buf)
{
    size_t pc = 0;
    for (size_t line = 0; line < prg_text->str_cnt; line++)
    {
        char *curr_tok = prg_text->string_entries[line].begin;
        curr_tok = strtok(curr_tok, " ");
        for (; curr_tok != NULL; curr_tok = strtok(NULL, " "))
        {
            if (strcmp(curr_tok, "push") == 0)
            {
                curr_tok = strtok(NULL, " ");
                if (strcmp(curr_tok, "rax") == 0)
                    output_buf[pc++] = OPCODE_PUSH_RAX;
                else if (strcmp(curr_tok, "rbx") == 0)
                    output_buf[pc++] = OPCODE_PUSH_RBX;
                else if (strcmp(curr_tok, "rcx") == 0)
                    output_buf[pc++] = OPCODE_PUSH_RCX;
                else if (strcmp(curr_tok, "rdx") == 0)
                    output_buf[pc++] = OPCODE_PUSH_RDX;
                else
                {
                    int32_t imm_val = str_to_num(curr_tok);
                    output_buf[pc++] = OPCODE_PUSH_IMM;
                    output_buf[pc++] = imm_val         & 0xFF;
                    output_buf[pc++] = (imm_val >>  8) & 0xFF;
                    output_buf[pc++] = (imm_val >> 16) & 0xFF;
                    output_buf[pc++] = (imm_val >> 24) & 0xFF;
                }
            }
            else if (strcmp(curr_tok, "pop") == 0)
            {
                curr_tok = strtok(NULL, " ");
                if (strcmp(curr_tok, "rax") == 0)
                    output_buf[pc++] = OPCODE_POP_RAX;
                else if (strcmp(curr_tok, "rbx") == 0)
                    output_buf[pc++] = OPCODE_POP_RBX;
                else if (strcmp(curr_tok, "rcx") == 0)
                    output_buf[pc++] = OPCODE_POP_RCX;
                else if (strcmp(curr_tok, "rdx") == 0)
                    output_buf[pc++] = OPCODE_POP_RDX;
            }
            else if (strcmp(curr_tok, "nop") == 0)
                output_buf[pc++] = OPCODE_NOP;
            else if (strcmp(curr_tok, "hlt") == 0)
                output_buf[pc++] = OPCODE_HLT;
            else if (strcmp(curr_tok, "in") == 0)
                output_buf[pc++] = OPCODE_IN;
            else if (strcmp(curr_tok, "out") == 0)
                output_buf[pc++] = OPCODE_OUT;
            else if (strcmp(curr_tok, "add") == 0)
                output_buf[pc++] = OPCODE_ADD;
            else if (strcmp(curr_tok, "sub") == 0)
                output_buf[pc++] = OPCODE_SUB;
            else if (strcmp(curr_tok, "mul") == 0)
                output_buf[pc++] = OPCODE_MUL;
            else if (strcmp(curr_tok, "div") == 0)
                output_buf[pc++] = OPCODE_DIV;
        }
    }
}

int main(int argc, char* argv[])
{
    char* src_file_name = NULL;
    char* prg_name      = NULL;
    if (argc < 2)
    {
        printf("Usage: assm <source file> [<output program file>]\n");
        return 0;
    }
    else if (argc == 2)
    {
        src_file_name = argv[1];
        prg_name      = "a.out";
    } 
    else if (argc == 3)
    {
        src_file_name = argv[1];
        prg_name      = argv[2];
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

    prg_header_t prg_header = {0};
    prg_header.signature[0] = 'L';
    prg_header.signature[1] = 'P';
    prg_header.signature[2] = 'R';
    prg_header.signature[3] = 'G';
    prg_header.format_ver = 1;
    strcpy(prg_header.program_name, "COMPILIER_DEFAULT");
    prg_header.code_size = byte_cnt;

    FILE *output_file = fopen(prg_name, "wb");
    if (output_file == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");
        free(code);
        free(prg_text2);
        return -1;
    }
    if (fwrite(&prg_header, sizeof(prg_header_t), 1, output_file) != 1)
    {
        fprintf(stderr, "Unable to write to output file\n");
        free(code);
        free(prg_text2);
        fclose(output_file);
        return -1;
    }
    if (fwrite(code, sizeof(char), byte_cnt, output_file) != byte_cnt)
    {
        fprintf(stderr, "Unable to write to output file\n");
        free(code);
        free(prg_text2);
        fclose(output_file);
        return -1;
    }
    fclose(output_file);
    free(code);
    free(prg_text2);
    return 0;
}