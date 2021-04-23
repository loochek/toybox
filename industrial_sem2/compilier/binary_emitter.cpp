#include "binary_emitter.hpp"

const char *regs_str_repr[] =
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

lstatus_t emit_mov(emitter_t *emt, register_t dst, register_t src)
{
    fprintf(emt->file, "    mov %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, register_t dst, int imm_src)
{
    fprintf(emt->file, "    mov %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, register_t dst, register_t src_base, int src_offset)
{
    fprintf(emt->file, "    mov %s, [%s%+d]\n", regs_str_repr[dst], regs_str_repr[src_base], src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, register_t dst_base, int dst_offset, register_t src)
{
    fprintf(emt->file, "    mov [%s%+d], %s\n", regs_str_repr[dst_base], dst_offset, regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, register_t dst, register_t src)
{
    fprintf(emt->file, "    add %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, register_t dst, int imm_src)
{
    fprintf(emt->file, "    add %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, register_t dst, register_t src)
{
    fprintf(emt->file, "    sub %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, register_t dst, int imm_src)
{
    fprintf(emt->file, "    sub %s, %d\n", regs_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_imul(emitter_t *emt, register_t dst, register_t src)
{
    fprintf(emt->file, "    imul %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_idiv(emitter_t *emt, register_t divider)
{
    fprintf(emt->file, "    idiv %s\n", regs_str_repr[divider]);
    return LSTATUS_OK;
}

lstatus_t emit_cmp(emitter_t *emt, register_t dst, register_t src)
{
    fprintf(emt->file, "    cmp %s, %s\n", regs_str_repr[dst], regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sete(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    sete %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setne(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    setne %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setl(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    setl %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setg(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    setg %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setle(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    setle %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setge(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    setge %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_cdq(emitter_t *emt)
{
    fprintf(emt->file, "    cdq\n");
    return LSTATUS_OK;
}

lstatus_t emit_push(emitter_t *emt, register_t src)
{
    fprintf(emt->file, "    push %s\n", regs_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_pop(emitter_t *emt, register_t dst)
{
    fprintf(emt->file, "    pop %s\n", regs_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, string_view_t label)
{
    fprintf(emt->file, "    call %.*s\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, const char *label)
{
    fprintf(emt->file, "    call %s\n", label);
    return LSTATUS_OK;
}

lstatus_t emit_jz(emitter_t *emt, const char *label)
{
    fprintf(emt->file, "    jz %s\n", label);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, string_view_t label)
{
    fprintf(emt->file, "%.*s:\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, const char *label)
{
    fprintf(emt->file, "%s:\n", label);
    return LSTATUS_OK;
}