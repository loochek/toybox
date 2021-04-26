#include "binary_emitter.hpp"

static const char *regs_str_repr[] =
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
    "rsp",

    // "al",
    // "bl",
    // "cl",
    // "dl",
    // "sil",
    // "dil",
    // "r8b",
    // "r9b",
    // "r10b",
    // "r11b",
    // "r12b",
    // "r13b",
    // "r14b",
    // "r15b",
    // "bpl",
    // "spl",
};

lstatus_t emitter_construct(emitter_t *emt)
{
    emt->file = fopen("output.asm", "w");

    return LSTATUS_OK;
}

lstatus_t emitter_destruct(emitter_t *emt)
{
    fclose(emt->file);

    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    mov %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, amd64_reg_t dst, int imm_src)
{
    fprintf(emt->file, "    mov %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src_base, int src_offset)
{
    fprintf(emt->file, "    mov %s, [%s%+d]\n", regs_str_repr[dst], regs_str_repr[src_base], src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, amd64_reg_t dst_base, int dst_offset, amd64_reg_t src)
{
    fprintf(emt->file, "    mov [%s%+d], %s\n", regs_str_repr[dst_base], dst_offset, regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    add %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, amd64_reg_t dst, int imm_src)
{
    fprintf(emt->file, "    add %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    sub %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, amd64_reg_t dst, int imm_src)
{
    fprintf(emt->file, "    sub %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_imul(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    imul %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_idiv(emitter_t *emt, amd64_reg_t divider)
{
    fprintf(emt->file, "    idiv %s\n", regs_str_repr[divider]);
    return LSTATUS_OK;
}

lstatus_t emit_xor(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    xor %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmp(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmp %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_test(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    test %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmove(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmove %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmovne(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmovne %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmovl(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmovl %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmovg(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmovg %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmovle(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmovle %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmovge(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src)
{
    fprintf(emt->file, "    cmovge %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cqo(emitter_t *emt)
{
    fprintf(emt->file, "    cqo\n");
    return LSTATUS_OK;
}

lstatus_t emit_push(emitter_t *emt, amd64_reg_t src)
{
    fprintf(emt->file, "    push %s\n", regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_pop(emitter_t *emt, amd64_reg_t dst)
{
    fprintf(emt->file, "    pop %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, string_view_t label)
{
    fprintf(emt->file, "    call %.*s\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, const char *label_fmt, ...)
{
    fprintf(emt->file, "    call ");

    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    return LSTATUS_OK;
}

lstatus_t emit_jmp(emitter_t *emt, const char *label_fmt, ...)
{
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
    fprintf(emt->file, "    ret\n\n");
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, string_view_t label)
{
    fprintf(emt->file, "%.*s:\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, const char *label_fmt, ...)
{
    va_list args = {};
    va_start(args, label_fmt);
    vfprintf(emt->file, label_fmt, args);
    va_end(args);

    fprintf(emt->file, ":\n");
    return LSTATUS_OK;
}

lstatus_t emit_comment(emitter_t *emt, const char *comment_fmt, ...)
{
    fprintf(emt->file, "; ");

    va_list args = {};
    va_start(args, comment_fmt);
    vfprintf(emt->file, comment_fmt, args);
    va_end(args);
    
    fprintf(emt->file, "\n");
    return LSTATUS_OK;
}