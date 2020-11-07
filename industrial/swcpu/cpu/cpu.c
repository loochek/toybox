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

#ifdef FRAMEBUFFER
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
    double          registers[REGISTERS_COUNT]; // 26 Number registers
    size_t          pc;                         // current instruction pointer
    double          mem[RAM_SIZE];              // RAM, higher 16384 is 128x128 VRAM
    bool            halted;
    my_stack_cpuval stack;

    program_t      *prg;

#ifdef FRAMEBUFFER
    framebuffer_t   fb;
    bool            graphics_enabled;
#endif
} cpu_t;

static inline void    cpu_construct  (cpu_t *cpu, const char prg_name[]);
static inline void    cpu_execute    (cpu_t *cpu);
static inline uint8_t cpu_read_byte  (cpu_t *cpu);
static inline double  cpu_read_double(cpu_t *cpu);
static inline double *cpu_access_ram (cpu_t *cpu, size_t addr);
static inline double  cpu_get_rvalue (cpu_t *cpu, uint8_t arg_mask);
static inline double* cpu_get_lvalue (cpu_t *cpu, uint8_t arg_mask);
static inline void    cpu_destruct   (cpu_t *cpu);
static inline void    cpu_validate   (cpu_t *cpu);
static inline void    cpu_dump       (cpu_t *cpu);

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

    cpu_t cpu = {0};
    cpu_construct(&cpu, prg_name);
    if (__lerrno != LERR_NO_ERROR)
    {
        LERRPRINT();
        return -1;
    }

    printf("Program %s\n", cpu.prg->prg_header->program_name);
    cpu_execute(&cpu);

    if (__lerrno == LERR_CPU_INTERNAL)
        LERRPRINT();
    else if (__lerrno == LERR_CPU_EXECUTION)
    {
        printf("CPU execution error: %s\n", __lerr_str);
        cpu_dump(&cpu);
    }

    cpu_destruct(&cpu);
    return 0;
}

#define CPU_CHECK(ret_val)                           \
{                                                    \
    cpu_validate(cpu);                               \
    if (__lerrno != LERR_NO_ERROR)                   \
    {                                                \
        printf("CPU validation failed!\n");          \
        cpu_dump(cpu);                               \
        return ret_val;                              \
    }                                                \
}

static inline void cpu_construct(cpu_t *cpu, const char prg_name[])
{
    __lerrno = LERR_NO_ERROR;
    if (stack_construct_cpuval(&cpu->stack, INITIAL_STACK_CAPACITY) != STACK_OK)
    {
        LERR(LERR_CPU_INTERNAL, "unable to create stack\n");
        return;
    }

#ifdef FRAMEBUFFER
    cpu->graphics_enabled = true;
    if (fb_init(&cpu->fb) != 0)
    {
        printf("INFO: Framebuffer initialization failed, graphical output disabled\n");
        printf("INFO: Error string: %s\n", __lerr_str);
        __lerrno = LERR_NO_ERROR; // it is quite normal to not initialize fb
        cpu->graphics_enabled = false;
    }
#endif

    cpu->prg = load_program_from_file(prg_name);
    if (cpu->prg == NULL)
    {
        stack_destruct_cpuval(&cpu->stack);
        // forward LERR from load_program_from_file
        return;
    }

    // TODO: is it necessary?
    for (size_t i = 0; i < REGISTERS_COUNT; i++)
        cpu->registers[i] = 0;

    cpu->pc     = 0;
    cpu->halted = 0;
}

// INSTRUCTION definition for cpu_execcute
#define INSTRUCTION(MNEMONIC, BASE_OPCODE, ARG_TYPE, IMPL) \
case BASE_OPCODE:                                          \
    IMPL;                                                  \
    break;

static inline void cpu_execute(cpu_t *cpu)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK();

    while (!cpu->halted)
    {
        uint8_t opcode = cpu_read_byte(cpu);
        if (__lerrno != LERR_NO_ERROR)
            return;
        
        uint8_t arg_mask = opcode & OPCODE_ARG_MASK;
        switch (opcode & OPCODE_INSTRUCTION_MASK) 
        {
            
        #include "../cpu_def.h"

        default:
            LERR(LERR_CPU_EXECUTION, "unknown base opcode");
            return;
            break;
        }
    }

    CPU_CHECK();
}

static inline uint8_t cpu_read_byte(cpu_t *cpu)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK(0);

    if (cpu->pc >= cpu->prg->prg_header->code_size)
    {
        LERR(LERR_CPU_EXECUTION, "program counter out of range");
        return 0;
    }

    return cpu->prg->code[cpu->pc++];
}

static inline double cpu_read_double(cpu_t *cpu)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK(0);

    if (cpu->pc >= cpu->prg->prg_header->code_size)
    {
        LERR(LERR_CPU_EXECUTION, "program counter out of range");
        return 0;
    }

    double to_ret = 0;
    memcpy(&to_ret, &cpu->prg->code[cpu->pc], IMM_SIZE);
    cpu->pc += IMM_SIZE;
    return to_ret;
}

static inline double *cpu_access_ram(cpu_t *cpu, size_t addr)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK(NULL);

    if (addr >= RAM_SIZE)
    {
        LERR(LERR_CPU_EXECUTION, "RAM address out of range");
        return NULL;
    }

    return &cpu->mem[addr];
}

static inline double cpu_get_rvalue(cpu_t *cpu, uint8_t arg_mask)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK(0);

    double rvalue = 0;
    if ((arg_mask & ARG_MASK_REGISTER) != 0)
    {
        uint8_t reg_num = cpu_read_byte(cpu);
        if (__lerrno != LERR_NO_ERROR)
            return 0;
        rvalue += cpu->registers[reg_num];
    }

    if ((arg_mask & ARG_MASK_IMMEDIATE) != 0)
    {
        rvalue += cpu_read_double(cpu);
        if (__lerrno != LERR_NO_ERROR)
            return 0;
    }

    if ((arg_mask & ARG_MASK_RAM) != 0)
    {
        double *mem_cell = cpu_access_ram(cpu, rvalue);
        if (__lerrno != LERR_NO_ERROR)
            return 0;
        rvalue = *mem_cell;
    }

    return rvalue;
}

static inline double* cpu_get_lvalue(cpu_t *cpu, uint8_t arg_mask)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK(NULL);

    if ((arg_mask & ARG_MASK_RAM) == 0)
    {
        uint8_t reg_num = cpu_read_byte(cpu);
        if (__lerrno != LERR_NO_ERROR)
            return NULL;
        return &cpu->registers[reg_num];
    }
    else
    {
        size_t addr = 0;

        if ((arg_mask & ARG_MASK_REGISTER) != 0)
        {
            uint8_t reg_num = cpu_read_byte(cpu);
            if (__lerrno != LERR_NO_ERROR)
                return NULL;
            addr += cpu->registers[reg_num];
        }

        if ((arg_mask & ARG_MASK_IMMEDIATE) != 0)
        {
            addr += cpu_read_double(cpu);
            if (__lerrno != LERR_NO_ERROR)
                return NULL;
        }

        double *mem_cell = cpu_access_ram(cpu, addr);
        if (__lerrno != LERR_NO_ERROR)
            return NULL;
        return mem_cell;
    }
}

static inline void cpu_destruct(cpu_t *cpu)
{
    __lerrno = LERR_NO_ERROR;
    CPU_CHECK();

    program_unload(cpu->prg);
    stack_destruct_cpuval(&cpu->stack);

#ifdef FRAMEBUFFER
    if (cpu->graphics_enabled)
        fb_deinit(&cpu->fb);
#endif
}

static inline void cpu_validate(cpu_t *cpu)
{
    __lerrno = LERR_NO_ERROR;
    if (stack_validate_cpuval(&cpu->stack) != STACK_OK)
    {
        LERR(LERR_CPU_INTERNAL, "stack fault");
        return;
    }

    if (cpu->prg == NULL)
    {
        LERR(LERR_CPU_INTERNAL, "program is not loaded");
        return;
    }
}

static inline void cpu_dump(cpu_t *cpu)
{
    cpu_validate(cpu);
    printf("CPU [%s]:\n"
           "PC: %zu\n"
           "Registers:\n", __lerrno == LERR_NO_ERROR ? "OK" : __lerr_str, cpu->pc);
    for (int i = 0; i < REGISTERS_COUNT; i++)
        printf("r%cx: %lg\n", 'a' + i, cpu->registers[i]);
    printf("Stack:\n");
    stack_dump_cpuval(&cpu->stack);
    printf("\n");
}