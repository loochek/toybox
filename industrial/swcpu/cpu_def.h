// It can be useful for writing implementation

#define STACK_PUSH(val)                                       \
{                                                             \
    if (stack_push_cpuval(&cpu_state.stack, val) != STACK_OK) \
    {                                                         \
        fprintf(stderr, "Stack error\n");                     \
        program_unload(prg);                                  \
        stack_destruct_cpuval(&cpu_state.stack);              \
        return -1;                                            \
    }                                                         \
}

#define STACK_POP(ptr)                                                  \
{                                                                       \
    stack_status_t status = stack_top_cpuval(&cpu_state.stack, ptr);    \
    if (status == STACK_EMPTY)                                          \
    {                                                                   \
        printf("CPU execution error: tryng to pop from empty stack\n"); \
        program_unload(prg);                                            \
        stack_destruct_cpuval(&cpu_state.stack);                        \
        return 0;                                                       \
    }                                                                   \
    else if (status != STACK_OK)                                        \
    {                                                                   \
        fprintf(stderr, "Stack error\n");                               \
        program_unload(prg);                                            \
        stack_destruct_cpuval(&cpu_state.stack);                        \
        return -1;                                                      \
    }                                                                   \
    if (stack_pop_cpuval(&cpu_state.stack) != STACK_OK)                 \
    {                                                                   \
        fprintf(stderr, "Stack error\n");                               \
        program_unload(prg);                                            \
        stack_destruct_cpuval(&cpu_state.stack);                        \
        return -1;                                                      \
    }                                                                   \
}

#define GET_RVALUE()                                        \
({                                                          \
    double rvalue = get_rvalue(arg_mask, &cpu_state, prg);  \
    if (__lerrno == LERR_BAD_ARG)                           \
    {                                                       \
        program_unload(prg);                                \
        stack_destruct_cpuval(&cpu_state.stack);            \
        return 0;                                           \
    }                                                       \
    rvalue;                                                 \
})

#define GET_LVALUE()                                        \
({                                                          \
    double *lvalue = get_lvalue(arg_mask, &cpu_state, prg); \
    if (__lerrno == LERR_BAD_ARG)                           \
    {                                                       \
        program_unload(prg);                                \
        stack_destruct_cpuval(&cpu_state.stack);            \
        return 0;                                           \
    }                                                       \
    lvalue;                                                 \
})

typedef enum
{
    ARG_NONE,
    ARG_RVALUE,
    ARG_LVALUE
} arg_t;

/*

INSTRUCTION macro is kinda DSL
It implements the CPU's functionality

Each each instruction can take no arguments, "lvalue" or "rvalue"
Lvalue is writeable memory cell (i.e. pointer), rvalue is calculated constant
Every opcode must be a multiple of 8,
because 3 least significant bits is the argument mask
MIR   Argument  Applicability
000 - none      Neither
001 - rax       LR
010 - 10        Only as rvalue
011 - rax+10    Only as rvalue
100 - none      Neither
101 - [rax]     LR
110 - [10]      LR
111 - [rax+10]  LR
(MIR - memory, immediate, register)
However, you don't need to worry about this when implementing opcodes
(see GET_RVALUE() and GET_LVALUE())

*/

// INSTRUCTION(mnemonic, base_opcode, argument_type, impl)

INSTRUCTION(nop, 0x00, ARG_NONE, {})

INSTRUCTION(hlt, 0x08, ARG_NONE, { cpu_state.halted = true; })

INSTRUCTION(push, 0x10, ARG_RVALUE,
{
    STACK_PUSH(GET_RVALUE());
})

INSTRUCTION(pop, 0x18, ARG_LVALUE,
{
    STACK_POP(GET_LVALUE());
})

INSTRUCTION(in, 0x20, ARG_NONE,
{
    for (;;)
    {
        char num_buf[21] = {0};
        printf("CPU asked you for a number:");
        scanf("%s", num_buf);
        double num = str_to_num(num_buf);
        if (__lerrno != LERR_NAN)
        {
            STACK_PUSH(num);
            break;
        }
        printf("Not a number, try again\n");
    }
})

INSTRUCTION(out, 0x28, ARG_NONE,
{
    char num_buf[21] = {0};
    double out_val = 0;
    STACK_POP(&out_val);
    num_to_str(out_val, num_buf);
    printf("CPU told you the number: %s\n", num_buf);
})

INSTRUCTION(jmp, 0x30, ARG_RVALUE, { cpu_state.pc = GET_RVALUE(); })

#define CONDITIONAL_JUMP(condition)                                         \
{                                                                           \
    double imm_val1 = 0, imm_val2 = 0;                                      \
    STACK_POP(&imm_val1);                                                   \
    STACK_POP(&imm_val2);                                                   \
    double jump_addr = GET_RVALUE();                                        \
    if (condition)                                                          \
        cpu_state.pc = (size_t)jump_addr;                                   \
}

INSTRUCTION(je , 0x38, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 == imm_val1))
INSTRUCTION(jne, 0x40, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 != imm_val1))
INSTRUCTION(jg , 0x48, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 >  imm_val1))
INSTRUCTION(jge, 0x50, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 >= imm_val1))
INSTRUCTION(jl , 0x58, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 <  imm_val1))
INSTRUCTION(jle, 0x60, ARG_RVALUE, CONDITIONAL_JUMP(imm_val2 <= imm_val1))

INSTRUCTION(call, 0x68, ARG_RVALUE,
{
    double jump_addr = GET_RVALUE();
    STACK_PUSH(cpu_state.pc);
    cpu_state.pc = (size_t)jump_addr;
})

INSTRUCTION(ret, 0x70, ARG_NONE,
{
    double jump_addr = 0;
    STACK_POP(&jump_addr);
    cpu_state.pc = (size_t)jump_addr;
})

#define BINARY_OPERATOR(what_to_push)                                       \
{                                                                           \
    double imm_val1 = 0, imm_val2 = 0;                                     \
    STACK_POP(&imm_val1);                                                   \
    STACK_POP(&imm_val2);                                                   \
    STACK_PUSH(what_to_push);                                               \
}

INSTRUCTION(add, 0x78, ARG_NONE, BINARY_OPERATOR(imm_val1 + imm_val2))
INSTRUCTION(sub, 0x80, ARG_NONE, BINARY_OPERATOR(imm_val2 - imm_val1))
INSTRUCTION(mul, 0x88, ARG_NONE, BINARY_OPERATOR(imm_val1 * imm_val2))
INSTRUCTION(div, 0x90, ARG_NONE, BINARY_OPERATOR(imm_val2 / imm_val1))

INSTRUCTION(sqrt, 0x98, ARG_NONE,
{
    double imm_val = 0;
    STACK_POP(&imm_val);
    imm_val = sqrt(imm_val);
    STACK_PUSH(imm_val);
})

INSTRUCTION(fbupd, 0xA0, ARG_NONE,
{
    for (size_t y = 0; y < 128; y++)
        for (size_t x = 0; x < 128; x++)
            if (cpu_state.mem[49152 + y * 128 + x] > 0)
            {
                int color = cpu_state.mem[49152 + y * 128 + x];
                int r = color % 256;
                color /= 256;
                int g = color % 256;
                color /= 256;
                int b =color % 256;
                put_pixel(x + 900, y + 10, b, g, r, 255);
            }
})