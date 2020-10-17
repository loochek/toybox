#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "string_lib.h"

typedef enum
{
    OPCODE_NOP,
    OPCODE_PUSH_IMM,
    OPCODE_PUSH_RAX,
    OPCODE_PUSH_RBX,
    OPCODE_PUSH_RCX,
    OPCODE_PUSH_RDX,
    OPCODE_POP_RAX,
    OPCODE_POP_RBX,
    OPCODE_POP_RCX,
    OPCODE_POP_RDX,
    OPCODE_IN,
    OPCODE_OUT,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_MUL,
    OPCODE_DIV,
    OPCODE_HLT
} __opcodes;

#define EMIT_OPCODE(op) fprintf(fout, "%02d", op)

int main()
{
    string_index_t *prg_lines = create_index_from_file("discr.assm");
    if (prg_lines == NULL)
    {
        // TODO: handle errors
        return -1;
    }
    FILE *fout = fopen("out.exetxt", "w");
    for (size_t line_num = 0; line_num < prg_lines->str_cnt; line_num++)
    {
        char *prg_line = prg_lines->string_entries[line_num].begin;

        prg_line = strtok(prg_line, " ");

        char cmd_buf[11];
        int cur_tok_idx = sscanf(prg_line, "%s", cmd_buf);
        if      (strcmp(cmd_buf, "nop") == 0)
            EMIT_OPCODE(OPCODE_NOP);
        else if (strcmp(cmd_buf, "hlt") == 0)
            EMIT_OPCODE(OPCODE_HLT);
        else if (strcmp(cmd_buf, "push") == 0)
        {
            // push
            char arg_buf[21];
            cur_tok_idx += sscanf(prg_line + cur_tok_idx, "%20s", arg_buf);
            if      (strcmp(arg_buf, "rax") == 0)
                EMIT_OPCODE(OPCODE_PUSH_RAX);
            else if (strcmp(arg_buf, "rbx") == 0)
                EMIT_OPCODE(OPCODE_PUSH_RBX);
            else if (strcmp(arg_buf, "rcx") == 0)
                EMIT_OPCODE(OPCODE_PUSH_RCX);
            else if (strcmp(arg_buf, "rdx") == 0)
                EMIT_OPCODE(OPCODE_PUSH_RCX);
            else
            {
                char *end_ptr = NULL;
                int imm_val = strtol(arg_buf, &end_ptr, 10);
                if (*end_ptr == '\0')
                {
                    EMIT_OPCODE(OPCODE_PUSH_IMM);
                    fprintf(fout, " ");
                    fprintf(fout, "%d", imm_val);
                }
                else
                {
                    printf("Error on line %zu: wrong argument for push: %s\n",
                            line_num + 1, arg_buf);
                    return 0;
                }
            }
        }
        else if (strcmp(cmd_buf, "pop") == 0)
        {
            // push
            char arg_buf[21];
            cur_tok_idx += sscanf(prg_line + cur_tok_idx, "%20s", arg_buf);
            if      (strcmp(arg_buf, "rax") == 0)
                EMIT_OPCODE(OPCODE_POP_RAX);
            else if (strcmp(arg_buf, "rbx") == 0)
                EMIT_OPCODE(OPCODE_POP_RBX);
            else if (strcmp(arg_buf, "rcx") == 0)
                EMIT_OPCODE(OPCODE_POP_RCX);
            else if (strcmp(arg_buf, "rdx") == 0)
                EMIT_OPCODE(OPCODE_POP_RCX);
            else
            {
                printf("Error on line %zu: wrong argument for pop: %s\n",
                        line_num + 1, arg_buf);
                return 0;
            }
        }
        else if (strcmp(cmd_buf, "in") == 0)
            EMIT_OPCODE(OPCODE_IN);
        else if (strcmp(cmd_buf, "out") == 0)
            EMIT_OPCODE(OPCODE_OUT);
        else if (strcmp(cmd_buf, "add") == 0)
            EMIT_OPCODE(OPCODE_ADD);
        else if (strcmp(cmd_buf, "sub") == 0)
            EMIT_OPCODE(OPCODE_SUB);
        else if (strcmp(cmd_buf, "mul") == 0)
            EMIT_OPCODE(OPCODE_MUL);
        else if (strcmp(cmd_buf, "div") == 0)
            EMIT_OPCODE(OPCODE_DIV);
        else
        {
            // unknown mnemonic
            printf("Error on line %zu: unknown mnemonic %s\n",
                 line_num + 1, cmd_buf);
            return 0;
        }
        char comm_buf
        if (scanf(prg_line + cur_tok_idx, "%5s", ))
        fprintf(fout, "\n");
    }
    fclose(fout);
    return 0;
}