#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"
#include "../common/headers/arithm.h"
#include "stack/stack_common.h"

// The CPU

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

// I'm sorry about this overheaded if mess instead of switch, but I couldn't implement DSL with swith

// INSTRUCTION definition for cpu
#define INSTRUCTION(mnemonic, base_opcode, argument_type, IMPL) \
if ((argument_type) == ARG_TYPE_NO_ARGS ||                      \
    (argument_type) == ARG_TYPE_LABEL)                          \
{                                                               \
    if (opcode == base_opcode)                                  \
    {                                                           \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
}                                                               \
if (((argument_type) & ARG_TYPE_REGISTER) != 0)                 \
{                                                               \
    if (opcode == base_opcode)                                  \
    {                                                           \
        ARG = 0;                                                \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 1)                         \
    {                                                           \
        ARG = 1;                                                \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 2)                         \
    {                                                           \
        ARG = 2;                                                \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 3)                         \
    {                                                           \
        ARG = 3;                                                \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
}                                                               \
if (((argument_type) & ARG_TYPE_IMMEDIATE) != 0)                \
{                                                               \
    if (opcode == base_opcode + 4)                              \
    {                                                           \
        ARG = 4;                                                \
        IMPL                                                    \
        continue;                                               \
    }                                                           \
}                                                               \

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
        uint8_t opcode = cpu_read_byte(&cpu_state, prg);
        int ARG = -1;
        #include "../cpu_def.h"

        printf("CPU fatal error: unknown opcode %02x\n", opcode);
        program_unload(prg);
        stack_destruct_cpuval(&cpu_state.stack);
        return 0;
    }
    
    program_unload(prg);
    stack_destruct_cpuval(&cpu_state.stack);
    return 0;
}
