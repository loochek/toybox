#define REGISTERS_COUNT         26
#define RAM_SIZE                65536
#define IMM_SIZE                8

#define ARG_MASK_REGISTER       1
#define ARG_MASK_IMMEDIATE      2
#define ARG_MASK_RAM            4

#define MAX_ARG_SIZE            50

#define OPCODE_INSTRUCTION_MASK 248 // 11111000
#define OPCODE_ARG_MASK         7   // 00000111

typedef enum
{
    ARG_NONE,
    ARG_RVALUE,
    ARG_LVALUE
} arg_t;