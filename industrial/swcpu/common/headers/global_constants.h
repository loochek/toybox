#define REGISTERS_COUNT 26

#define ARG_MASK_REGISTER  1
#define ARG_MASK_IMMEDIATE 2
#define ARG_MASK_RAM       4

typedef enum
{
    ARG_NONE,
    ARG_RVALUE,
    ARG_LVALUE
} arg_t;