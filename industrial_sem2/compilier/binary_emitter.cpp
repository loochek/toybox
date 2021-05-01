#include "binary_emitter.hpp"

static const char *regs64_str_repr[] =
{
    "dummy",
    "rax",
    "rbx",
    "rcx",
    "rdx",
    "rsi",
    "rdi",
    "r8",
    "r9",
    "r10",
    "r11",
    "r12",
    "r13",
    "r14",
    "r15",
    "rbp",
    "rsp"
};

static const char *regs8_str_repr[] =
{
    "dummy",
    "al",
    "bl",
    "cl",
    "dl",
    "sil",
    "dil",
    "r8b",
    "r9b",
    "r10b",
    "r11b",
    "r12b",
    "r13b",
    "r14b",
    "r15b",
    "bpl",
    "spl"
};

const uint8_t EMPTY_REX = 0x40;

/**
 * Emits a byte to the program buffer
 * 
 * \param \c emt Emitter object
 * \param \c byte Byte to emit
 */
static lstatus_t emit_byte(emitter_t *emt, uint8_t byte);

/**
 * Emits a dword to the program buffer
 * 
 * \param \c emt Emitter object
 * \param \c dword Dword to emit
 */
static lstatus_t emit_dword(emitter_t *emt, int32_t dword);

/**
 * Emits a qword to the program buffer
 * 
 * \param \c emt Emitter object
 * \param \c qword Qword to emit
 */
static lstatus_t emit_qword(emitter_t *emt, int64_t qword);

/**
 * Emits a ModRM byte of a register-direct addressing mode:
 * ModRM.mod = 11
 * ModRM.reg = 3 least significant bits of reg
 * ModRM.rm  = 3 least significant bits of rm
 * 
 * Which register is the source and which is the destination depends on the opcode
 * 
 * \param \c emt Emitter object
 * \param \c reg Reg
 * \param \c rm R/M
 */
static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm);

/**
 * Emits a ModRM byte of a register-indirect addressing mode with 32bit displacement:
 * ModRM.mod = 10
 * ModRM.reg = 3 least significant bits of reg
 * ModRM.rm  = 3 least significant bits of rm
 * 
 * Operand 1: reg
 * Operand 2: [rm + disp32]
 * 
 * Which operand is the source and which is the destination depends on the opcode
 * 
 * \param \c emt Emitter object
 * \param \c reg Reg
 * \param \c rm R/M
 * \param \c rm_disp R/M displacement
 */
static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm, int32_t rm_disp);

/**
 * Emits a REX byte - 64-bit operand size:
 * REX.X = 0 (SIB.index is not used)
 * REX.W = rex_w
 * REX.R = most significant bit of reg
 * REX.B = most significant bit of rm
 * 
 * \param \c emt Emitter object
 * \param \c rex_w REX.W value
 * \param \c reg Reg
 * \param \c rm R\M
 */
static lstatus_t emit_rex(emitter_t *emt, bool rex_w, reg64_t reg, reg64_t rm);

lstatus_t emitter_construct(emitter_t *emt)
{
    emt->listing_file = fopen("output.asm", "w");
    emt->idle = false;

    return LSTATUS_OK;
}

lstatus_t emitter_destruct(emitter_t *emt)
{
    if (emt == nullptr)
        return LSTATUS_OK;

    fclose(emt->listing_file);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x89)); // MOV r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    mov %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_movsx(emitter_t *emt, reg64_t dst, reg8_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    movsx %s, %s\n", regs64_str_repr[dst], regs8_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, int64_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, dst));
    LSCHK(emit_byte(emt, 0xB8 + (int)dst % 8)); // MOV r64, imm64

    fprintf(emt->listing_file, "    mov %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src_base, int32_t src_offset)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, dst, src_base));
    LSCHK(emit_byte(emt, 0x8B)); // MOV r64,r/m64
    LSCHK(emit_operands(emt, dst, src_base, src_offset));

    fprintf(emt->listing_file, "    mov %s, [%s%+d]\n", regs64_str_repr[dst], regs64_str_repr[src_base], src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst_base, int32_t dst_offset, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst_base));
    LSCHK(emit_byte(emt, 0x89)); // MOV r/m64,r64
    LSCHK(emit_operands(emt, src, dst_base, dst_offset));

    fprintf(emt->listing_file, "    mov [%s%+d], %s\n", regs64_str_repr[dst_base], dst_offset, regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    add %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, int64_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    add %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    sub %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, int64_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    sub %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_imul(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    imul %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_idiv(emitter_t *emt, reg64_t divider)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    idiv %s\n", regs64_str_repr[divider]);
    return LSTATUS_OK;
}

lstatus_t emit_xor(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    xor %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmp(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    cmp %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_test(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    test %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sete(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    sete %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setne(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    setne %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setl(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    setl %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setg(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    setg %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setle(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    setle %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setge(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    setge %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_cqo(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    cqo\n");
    return LSTATUS_OK;
}

lstatus_t emit_push(emitter_t *emt, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    push %s\n", regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_pop(emitter_t *emt, reg64_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    pop %s\n", regs64_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->listing_file, "    call %.*s\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    call ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->listing_file, label_fmt, args);
    va_end(args);

    return LSTATUS_OK;
}

lstatus_t emit_jmp(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    jmp ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->listing_file, label_fmt, args);
    va_end(args);

    fprintf(emt->listing_file, "\n");

    return LSTATUS_OK;
}

lstatus_t emit_jz(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    jz ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->listing_file, label_fmt, args);
    va_end(args);

    fprintf(emt->listing_file, "\n");

    return LSTATUS_OK;
}

lstatus_t emit_ret(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->listing_file, "    ret\n\n");
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->listing_file, "%.*s:\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->listing_file, label_fmt, args);
    va_end(args);

    fprintf(emt->listing_file, ":\n");
    return LSTATUS_OK;
}

lstatus_t emit_comment(emitter_t *emt, const char *comment_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->listing_file, "; ");

    va_list args = {};
    va_start(args, comment_fmt);
    vfprintf(emt->listing_file, comment_fmt, args);
    va_end(args);
    
    fprintf(emt->listing_file, "\n");
    return LSTATUS_OK;
}

reg8_t reg64_to_8(reg64_t reg64)
{
    return (reg8_t)reg64;
}

static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm)
{
    uint8_t modrm = 0xC0; // first two bits are 11
    modrm |= (reg & 0x7) << 3;
    modrm |= rm & 0x7;

    LSCHK(emit_byte(emt, modrm));
    return LSTATUS_OK;
}

static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm, int32_t rm_disp)
{
    uint8_t modrm = 0x80; // first two bits are 10
    modrm |= (reg & 0x7) << 3;
    modrm |= rm & 0x7;
    LSCHK(emit_byte(emt, modrm));
    
    if (reg == REG_RSP || reg == REG_R12)
    {
        // https://wiki.osdev.org/X86-64_Instruction_Encoding#SIB
        uint8_t sib = 0x80; // first two bits are 10
        sib |= 0x4 << 3;
        sib |= (rm & 0x7);
        LSCHK(emit_byte(emt, sib));
    }

    LSCHK(emit_dword(emt, rm_disp));
    return LSTATUS_OK;
}