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

#ifdef FRAMEBUFFER_ENABLE
#include "fblib/fblib.h"
#endif

// The CPU

#define STACK_SEC_HASHING
#define STACK_SEC_CANARY
#define STACK_SEC_POISON

#define TYPE cpuval
#define elem_t double
#include "stack/stack.h"
#undef elem_t
#undef TYPE

#define INITIAL_STACK_CAPACITY 10

typedef struct
{
    double registers[REGISTERS_COUNT]; // 26 Number registers
    size_t pc;                         // current instruction pointer
    double mem[RAM_SIZE];              // RAM, higher 16384 is 128x128 VRAM
    bool halted;
    my_stack_cpuval stack;
} cpu_state_t;

static inline uint8_t cpu_read_byte(cpu_state_t *cpu_state, program_t *prg);
static inline double  cpu_read_double(cpu_state_t *cpu_state, program_t *prg);
static inline double  get_rvalue(uint8_t arg_mask, cpu_state_t *cpu_state, program_t *prg);
static inline double* get_lvalue(uint8_t arg_mask, cpu_state_t *cpu_state, program_t *prg);

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
    if (stack_construct_cpuval(&cpu_state.stack, INITIAL_STACK_CAPACITY) != STACK_OK)
    {
        fprintf(stderr, "Stack error\n");
        stack_destruct_cpuval(&cpu_state.stack);
        return -1;
    }

#ifdef FRAMEBUFFER_ENABLE
    bool graphics_enabled = true;
    size_t fb_width = 0, fb_height = 0;
    if (fb_init() != 0)
    {
        printf("Framebuffer initialization failed, graphical output disabled\n");
        graphics_enabled = false;
    }
    else
        get_resolution(&fb_width, &fb_height);
#endif

    program_t *prg = load_program_from_file(prg_name);
    if (prg == NULL)
    {
        LERRPRINT();
        stack_destruct_cpuval(&cpu_state.stack);
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
#ifdef FRAMEBUFFER_ENABLE
    if (graphics_enabled)
        fb_close();
#endif
    program_unload(prg);
    stack_destruct_cpuval(&cpu_state.stack);
    return 0;
}

static inline uint8_t cpu_read_byte(cpu_state_t *cpu_state, program_t *prg)
{
    return prg->code[cpu_state->pc++];
}

static inline double cpu_read_double(cpu_state_t *cpu_state, program_t *prg)
{
    double to_ret = 0;
    memcpy(&to_ret, &prg->code[cpu_state->pc], IMM_SIZE);
    cpu_state->pc += IMM_SIZE;
    return to_ret;
}

static inline double get_rvalue(uint8_t arg_mask, cpu_state_t *cpu_state, program_t *prg)
{
    double rvalue = 0;
    if ((arg_mask & ARG_MASK_REGISTER) != 0)
    {
        uint8_t reg_num = cpu_read_byte(cpu_state, prg);
        if (reg_num >= REGISTERS_COUNT)
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
            return &cpu_state->registers[reg_num];
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