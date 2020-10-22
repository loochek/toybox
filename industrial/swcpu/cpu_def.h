#define ARG_TYPE_NO_ARGS   0
#define ARG_TYPE_REGISTER  1
#define ARG_TYPE_IMMEDIATE 2
#define ARG_TYPE_LABEL     4

#define READ_BYTE()  cpu_read_byte (&cpu_state, prg)
#define READ_DWORD() cpu_read_dword(&cpu_state, prg)

/*

INSTRUCTION macro is kinda DSL
It implements the CPU's functionality

If the instruction has an argument,
it can be converted into several opcodes: for example, different registers or immediate value as
argument of push instruction. Argument types are controlled by ARG_TYPE macros.
For example, PUSH instruction is converted to 0x01..0x05 opcodes - for each register and immediate
value as argument.

If you want to use an immediate value as an argument, then you must also allow registers.
(but it makes sense)
So you can't use ARG_TYPE_IMMEDIATE without ARG_TYPE_REGISTER

It's safe to use this combinations:
ARG_TYPE_NO_ARGS
ARG_TYPE_REGISTER                      - as "lvalue" or "rvalue"
ARG_TYPE_REGISTER | ARG_TYPE_IMMEDIATE - as "lvalue"
ARG_TYPE_LABEL                         - special type for jumps and subroutines calls

*/

// INSTRUCTION(mnemonic, base_opcode, argument_type, impl)

INSTRUCTION(nop, 0x00, ARG_TYPE_NO_ARGS,{})

INSTRUCTION(push, 0x01, ARG_TYPE_REGISTER | ARG_TYPE_IMMEDIATE,
{
    if (ARG == 4)
        stack_push_cpuval(&cpu_state.stack, READ_DWORD());
    else
        stack_push_cpuval(&cpu_state.stack, cpu_state.registers[ARG]);  
})

INSTRUCTION(pop, 0x06, ARG_TYPE_REGISTER,
{
    stack_status_t status1 = stack_top_cpuval(&cpu_state.stack, &cpu_state.registers[ARG]);
    stack_pop_cpuval(&cpu_state.stack);
    if (status1 == STACK_EMPTY)
    {
        printf("CPU fatal error: tryng to pop from empty stack\n");
        return 0;
    }
})

INSTRUCTION(in, 0x0A, ARG_TYPE_NO_ARGS,
{
    char num_buf[21] = {0};
    printf("CPU asked you for a number:");
    scanf("%s", num_buf);
    stack_push_cpuval(&cpu_state.stack, str_to_num(num_buf));
})

INSTRUCTION(out, 0x0B, ARG_TYPE_NO_ARGS,
{
    char num_buf[21] = {0};
    int32_t imm_val = 0;
    stack_status_t status1 = stack_top_cpuval(&cpu_state.stack, &imm_val);
    stack_pop_cpuval(&cpu_state.stack);
    if (status1 == STACK_EMPTY)
    {
        printf("CPU fatal error: tryng to pop from empty stack\n");
        return 0;
    }
    num_to_str(imm_val, num_buf);
    printf("CPU told you the number: %s\n", num_buf);
})

#define BINARY_OPERATOR(what_to_push)                                       \
{                                                                           \
    int32_t imm_val1 = 0, imm_val2 = 0;                                     \
    stack_status_t status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1); \
    stack_pop_cpuval(&cpu_state.stack);                                     \
    stack_status_t status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2); \
    stack_pop_cpuval(&cpu_state.stack);                                     \
    if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)                   \
    {                                                                       \
        printf("CPU fatal error: tryng to pop from empty stack\n");         \
        return 0;                                                           \
    }                                                                       \
    stack_push_cpuval(&cpu_state.stack, what_to_push);                      \
}

INSTRUCTION(add, 0x0C, ARG_TYPE_NO_ARGS, BINARY_OPERATOR(imm_val1 + imm_val2))
INSTRUCTION(sub, 0x0D, ARG_TYPE_NO_ARGS, BINARY_OPERATOR(imm_val2 - imm_val1))
INSTRUCTION(mul, 0x0E, ARG_TYPE_NO_ARGS, BINARY_OPERATOR((int32_t)((int64_t)imm_val1 * imm_val2 / 1000)))
INSTRUCTION(div, 0x0F, ARG_TYPE_NO_ARGS, BINARY_OPERATOR(imm_val2 * 1000 / imm_val1))
INSTRUCTION(hlt, 0x10, ARG_TYPE_NO_ARGS, {cpu_state.halted = true;})

INSTRUCTION(jmp, 0x11, ARG_TYPE_LABEL, { cpu_state.pc = READ_DWORD(); })

#define CONDITIONAL_JUMP(condition)                                         \
{                                                                           \
    int32_t imm_val1 = 0, imm_val2 = 0;                                     \
    stack_status_t status1 = stack_top_cpuval(&cpu_state.stack, &imm_val1); \
    stack_pop_cpuval(&cpu_state.stack);                                     \
    stack_status_t status2 = stack_top_cpuval(&cpu_state.stack, &imm_val2); \
    stack_pop_cpuval(&cpu_state.stack);                                     \
    if (status1 == STACK_EMPTY || status2 == STACK_EMPTY)                   \
    {                                                                       \
        printf("CPU fatal error: tryng to pop from empty stack\n");         \
        return 0;                                                           \
    }                                                                       \
    int32_t jump_addr = READ_DWORD();                                       \
    if (condition)                                                          \
        cpu_state.pc = jump_addr;                                           \
}

INSTRUCTION(je , 0x12, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 == imm_val1))
INSTRUCTION(jne, 0x13, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 != imm_val1))
INSTRUCTION(jg , 0x14, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 >  imm_val1))
INSTRUCTION(jge, 0x15, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 >= imm_val1))
INSTRUCTION(jl , 0x16, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 <  imm_val1))
INSTRUCTION(jle, 0x17, ARG_TYPE_LABEL, CONDITIONAL_JUMP(imm_val2 <= imm_val1))
