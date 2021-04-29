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

lstatus_t emitter_construct(emitter_t *emt)
{
    emt->file = fopen("output.asm", "w");
    emt->idle = false;

    return LSTATUS_OK;
}

lstatus_t emitter_destruct(emitter_t *emt)
{
    if (emt == nullptr)
        return LSTATUS_OK;

    fclose(emt->file);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    mov %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_movsx(emitter_t *emt, reg64_t dst, reg8_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    movsx %s, %s\n", regs64_str_repr[dst], regs8_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, int imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    mov %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src_base, int src_offset)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    mov %s, [%s%+d]\n", regs64_str_repr[dst], regs64_str_repr[src_base], src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst_base, int dst_offset, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    mov [%s%+d], %s\n", regs64_str_repr[dst_base], dst_offset, regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    add %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, int imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    add %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    sub %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, int imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    sub %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_imul(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    imul %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_idiv(emitter_t *emt, reg64_t divider)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    idiv %s\n", regs64_str_repr[divider]);
    return LSTATUS_OK;
}

lstatus_t emit_xor(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    xor %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmp(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    cmp %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_test(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    test %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sete(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    sete %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setne(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    setne %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setl(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    setl %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setg(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    setg %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setle(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    setle %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setge(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    setge %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_cqo(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    cqo\n");
    return LSTATUS_OK;
}

lstatus_t emit_push(emitter_t *emt, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    push %s\n", regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_pop(emitter_t *emt, reg64_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    pop %s\n", regs64_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->file, "    call %.*s\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    call ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    return LSTATUS_OK;
}

lstatus_t emit_jmp(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    jmp ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    fprintf(emt->file, "\n");

    return LSTATUS_OK;
}

lstatus_t emit_jz(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    jz ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    fprintf(emt->file, "\n");

    return LSTATUS_OK;
}

lstatus_t emit_ret(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    fprintf(emt->file, "    ret\n\n");
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->file, "%.*s:\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    fprintf(emt->file, ":\n");
    return LSTATUS_OK;
}

lstatus_t emit_comment(emitter_t *emt, const char *comment_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;
        
    fprintf(emt->file, "; ");

    va_list args = {};
    va_start(args, comment_fmt);
    vfprintf(emt->file, comment_fmt, args);
    va_end(args);
    
    fprintf(emt->file, "\n");
    return LSTATUS_OK;
}

reg8_t reg64_to_8(reg64_t reg64)
{
    return (reg8_t)reg64;
}