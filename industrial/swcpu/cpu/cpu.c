#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"
#include "../common/headers/opcodes.h"
#include "../common/headers/arithm.h"
#include "stack/stack_common.h"

#define STACK_SEC_HASHING
#define STACK_SEC_CANARY
#define STACK_SEC_POISON

#define TYPE cpuval
#define elem_t int32_t
#include "stack/stack.h"
#undef elem_t
#undef TYPE

typedef struct
{
    int32_t registers[4]; // 4 Number registers
    int32_t pc;           // current instruction pointer
    bool halted;
    my_stack_cpuval stack;
} cpu_state_t;

static inline uint8_t cpu_read_byte(cpu_state_t *cpu_state, program_t *prg)
{
    return prg->code[cpu_state->pc++];
}

static inline uint32_t cpu_read_dword(cpu_state_t *cpu_state, program_t *prg)
{
    // SWCPU is little-endian
    uint32_t dword = prg->code[cpu_state->pc++];
    dword |= prg->code[cpu_state->pc++] << 8;
    dword |= prg->code[cpu_state->pc++] << 16;
    dword |= prg->code[cpu_state->pc++] << 24;
    return dword;
}

int main(int argc, char* argv[])
{
    char* prg_name = NULL;
    if (argc < 2)
    {
        printf("Usage: cpu <program file>\n");
        return 0;
    }
    else if (argc == 2)
        prg_name = argv[1];
        
    cpu_state_t cpu_state = {0};
    if (stack_construct_cpuval(&cpu_state.stack, 10) != STACK_OK)
    {
        fprintf(stderr, "Stack error\n");
        return -1;
    }

    program_t *prg = load_program_from_file(prg_name);
    if (prg == NULL)
    {
        LERRPRINT();
        return -1;
    }

    printf("Program %s\n", prg->prg_header->program_name);

    while (!cpu_state.halted)
    {
        int32_t imm_val1 = 0, imm_val2 = 0;
        stack_status_t status1 = STACK_ERROR, status2 = STACK_ERROR;
        char num_buf[21] = {0};

        uint8_t opcode = cpu_read_byte(&cpu_state, prg);

        switch (opcode)
        {
        case OPCODE_NOP:
            break;

        case OPCODE_PUSH_IMM:
            stack_push_cpuval(&cpu_state.stack, cpu_read_dword(&cpu_state, prg));
            break;

        case OPCODE_PUSH_RAX:
        case OPCODE_PUSH_RBX:
        case OPCODE_PUSH_RCX:
        case OPCODE_PUSH_RDX:
            stack_push_cpuval(&cpu_state.stack, cpu_state.registers[opcode - OPCODE_PUSH_RAX]);
            break;

        case OPCODE_POP_RAX:
        case OPCODE_POP_RBX:
        case OPCODE_POP_RCX:
        case OPCODE_POP_RDX:
            status1 = stack_top_cpuval(&cpu_state.stack,
                                       &cpu_state.registers[opcode - OPCODE_POP_RAX]);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }
            break;

        case OPCODE_IN:
            printf("CPU asked you for a number:");
            scanf("%s", num_buf);
            stack_push_cpuval(&cpu_state.stack, str_to_num(num_buf));
            break;

        case OPCODE_OUT:
            status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }

            num_to_str(imm_val1, num_buf);
            printf("CPU told you the number: %s\n", num_buf);
            break;

        case OPCODE_ADD:
            status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1);
            stack_pop_cpuval(&cpu_state.stack);
            status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }
            stack_push_cpuval(&cpu_state.stack, imm_val1 + imm_val2);
            break;

        case OPCODE_SUB:
            status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1);
            stack_pop_cpuval(&cpu_state.stack);
            status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }
            stack_push_cpuval(&cpu_state.stack, imm_val2 - imm_val1);
            break;

        case OPCODE_MUL:
            status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1);
            stack_pop_cpuval(&cpu_state.stack);
            status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }
            stack_push_cpuval(&cpu_state.stack, (int32_t)((int64_t)imm_val1 * imm_val2 / 1000));
            break;

        case OPCODE_DIV:
            status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1);
            stack_pop_cpuval(&cpu_state.stack);
            status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2);
            stack_pop_cpuval(&cpu_state.stack);
            if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)
            {
                printf("CPU fatal error: tryng to pop from empty stack\n");
                return 0;
            }
            stack_push_cpuval(&cpu_state.stack, imm_val2 * 1000 / imm_val1);
            break;

        case OPCODE_HLT:
            cpu_state.halted = true;
            break;

        default:
            printf("CPU fatal error: unknown opcode %02x\n", opcode);
            return 0;
            break;
        }
    }

    program_unload(prg);
    stack_destruct_cpuval(&cpu_state.stack);
    return 0;
}
