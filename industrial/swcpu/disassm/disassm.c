#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "../common/headers/lerror.h"
#include "../common/headers/prg.h"
#include "../common/headers/arithm.h"

// The simplest part of the toolchain

// INSTRUCTION definition for disassm
#define INSTRUCTION(mnemonic, base_opcode, argument_type, IMPL) \
if ((argument_type) == ARG_TYPE_NO_ARGS)                        \
{                                                               \
    if (opcode == base_opcode)                                  \
    {                                                           \
        fprintf(disassm_file, #mnemonic "\n");                  \
        continue;                                               \
    }                                                           \
}                                                               \
if (((argument_type) & ARG_TYPE_REGISTER) != 0)                 \
{                                                               \
    if (opcode == base_opcode)                                  \
    {                                                           \
        fprintf(disassm_file, #mnemonic " rax\n");              \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 1)                         \
    {                                                           \
        fprintf(disassm_file, #mnemonic " rbx\n");              \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 2)                         \
    {                                                           \
        fprintf(disassm_file, #mnemonic " rcx\n");              \
        continue;                                               \
    }                                                           \
    else if (opcode == base_opcode + 3)                         \
    {                                                           \
        fprintf(disassm_file, #mnemonic " rdx\n");              \
        continue;                                               \
    }                                                           \
}                                                               \
if (((argument_type) & ARG_TYPE_IMMEDIATE) != 0)                \
{                                                               \
    if (opcode == base_opcode + 4)                              \
    {                                                           \
        char num_buf[21] = {0};                                 \
        num_to_str(prg_read_dword(prg, pc), num_buf);           \
        pc += 4;                                                \
        fprintf(disassm_file, #mnemonic " %s\n", num_buf);      \
        continue;                                               \
    }                                                           \
}                                                               \
if (((argument_type) & ARG_TYPE_LABEL) != 0)                    \
{                                                               \
    if (opcode == base_opcode)                                  \
    {                                                           \
        int32_t addr = prg_read_dword(prg, pc);                 \
        pc += 4;                                                \
        fprintf(disassm_file, #mnemonic " 0x%x\n", addr);       \
        continue;                                               \
    }                                                           \
}

static inline uint8_t prg_read_byte(program_t *prg, size_t addr)
{
    return prg->code[addr];
}

static inline uint32_t prg_read_dword(program_t *prg, size_t addr)
{
    // SWCPU is little-endian
    uint32_t dword = prg->code[addr];
    dword |= prg->code[addr + 1] << 8;
    dword |= prg->code[addr + 2] << 16;
    dword |= prg->code[addr + 3] << 24;
    return dword;
}

int main(int argc, char* argv[])
{
    char* src_file_name = NULL;
    char* prg_name      = NULL;
    if (argc < 2)
    {
        printf("Usage: disassm <input program file> [<output source file>]\n");
        return 0;
    }
    else if (argc == 2)
    {
        prg_name      = argv[1];
        src_file_name = "src.assm";
    } 
    else if (argc == 3)
    {
        prg_name      = argv[1];
        src_file_name = argv[2];
    }
    program_t *prg = load_program_from_file(prg_name);
    if (prg == NULL)
    {
        LERRPRINT();
        return -1;
    }
    
    FILE *disassm_file = fopen(src_file_name, "w");
    if (disassm_file == NULL)
    {
        fprintf(stderr, "Can't open output file\n");
        return -1;
    }

    fprintf(disassm_file, "; Auto-generated by disassm\n"
                          "; Program %s\n", prg->prg_header->program_name);

    size_t pc = 0;
    while (pc < prg->prg_header->code_size)
    {
        uint8_t opcode = prg_read_byte(prg, pc);
        pc++;

        #include "../cpu_def.h"
        
        printf("Disassm fatal error: unknown opcode %02x\n", opcode);
        fclose(disassm_file);
        program_unload(prg);
        return 0;
    }
    fclose(disassm_file);
    program_unload(prg);
    return 0;
}