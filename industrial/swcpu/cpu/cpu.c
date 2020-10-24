#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"
#include "../common/headers/arithm.h"
#include "../common/headers/global_constants.h"
#include "stack/stack_common.h"

// The CPU

#define STACK_SEC_HASHING
#define STACK_SEC_CANARY
#define STACK_SEC_POISON

#define TYPE cpuval
#define elem_t double
#include "stack/stack.h"
#undef elem_t
#undef TYPE

typedef struct
{
    double registers[REGISTERS_COUNT]; // 26 Number registers
    size_t pc;           // current instruction pointer
    double mem[16384];   // RAM
    bool halted;
    my_stack_cpuval stack;
} cpu_state_t;

static inline uint8_t cpu_read_byte(cpu_state_t *cpu_state, program_t *prg)
{
    return prg->code[cpu_state->pc++];
}

static inline double cpu_read_double(cpu_state_t *cpu_state, program_t *prg)
{
    // SWCPU is little-endian
    uint64_t qword = (int64_t)prg->code[cpu_state->pc++];
    qword |= (int64_t)prg->code[cpu_state->pc++] << 8;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 16;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 24;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 32;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 40;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 48;
    qword |= (int64_t)prg->code[cpu_state->pc++] << 56;
    double to_ret = 0;
    memcpy(&to_ret, &qword, sizeof(double));
    return to_ret;
}

static inline double get_rvalue(uint8_t arg_mask, cpu_state_t *cpu_state, program_t *prg)
{
    double rvalue = 0;
    if ((arg_mask & ARG_MASK_REGISTER) != 0)
    {
        uint8_t reg_num = cpu_read_byte(cpu_state, prg);
        if (reg_num >= 4)
        {
            printf("CPU execution error: bad register number: %d\n", reg_num);
            LERR(LERR_BAD_ARG, "");
            return 0;
        }
        rvalue += cpu_state->registers[reg_num];
    }

    if ((arg_mask & ARG_MASK_IMMEDIATE) != 0)
        rvalue += cpu_read_double(cpu_state, prg);

    if ((arg_mask & ARG_MASK_RAM) != 0)
        rvalue = cpu_state->mem[(size_t)rvalue];

    return rvalue;
}

static inline double* get_lvalue(uint8_t arg_mask, cpu_state_t *cpu_state, program_t *prg)
{
    double *lvalue = NULL;
    if ((arg_mask & ARG_MASK_RAM) == 0)
    {
        if (((arg_mask & ARG_MASK_REGISTER) != 0) && ((arg_mask & ARG_MASK_IMMEDIATE) == 0))
        {
            uint8_t reg_num = cpu_read_byte(cpu_state, prg);
            if (reg_num >= REGISTERS_COUNT)
            {
                printf("CPU execution error: bad register number: %d\n", reg_num);
                LERR(LERR_BAD_ARG, "");
                return 0;
            }
            lvalue = &cpu_state->registers[reg_num];
        }
        else
        {
            printf("CPU execution error: can't use rvalue as lvalue\n");
            LERR(LERR_BAD_ARG, "");
            return 0;
        }   
    }
    else
    {
        size_t addr = 0;
        if ((arg_mask & ARG_MASK_REGISTER) != 0)
        {
            uint8_t reg_num = cpu_read_byte(cpu_state, prg);
            if (reg_num >= REGISTERS_COUNT)
            {
                printf("CPU execution error: bad register number: %d\n", reg_num);
                LERR(LERR_BAD_ARG, "");
                return 0;
            }
            addr += cpu_state->registers[reg_num];
        }

        if ((arg_mask & ARG_MASK_IMMEDIATE) != 0)
            addr += cpu_read_double(cpu_state, prg);

        return &cpu_state->mem[addr];
    }
}

// INSTRUCTION definition for cpu
#define INSTRUCTION(MNEMONIC, BASE_OPCODE, ARG_TYPE, IMPL) \
case BASE_OPCODE:                                          \
    IMPL;                                                  \
    break;

int main(int argc, char* argv[])
{
    char* prg_name = NULL;
    if (argc < 2)
    {
        printf("Usage: cpu <program file>\n");
        return 0;
    }
    else if (argc >= 2)
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
        uint8_t opcode   = cpu_read_byte(&cpu_state, prg);
        uint8_t arg_mask = opcode & 7;
        switch (opcode & 248) // 11111000
        {
        #include "../cpu_def.h"

        default:
            printf("CPU execution error: unknown base opcode %02x\n", opcode & 248);
            program_unload(prg);
            stack_destruct_cpuval(&cpu_state.stack);
            return 0;
            break;
        }
    }
    
    program_unload(prg);
    stack_destruct_cpuval(&cpu_state.stack);
    return 0;
}