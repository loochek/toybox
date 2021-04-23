#ifndef BINARY_EMITTER_HPP
#define BINARY_EMITTER_HPP

#include <cstdio>
#include "utils/lstatus.hpp"
#include "utils/string_view.hpp"

enum amd64_reg_t
{
    REG_DUMMY,
    REG_RAX,
    REG_RBX,
    REG_RCX,
    REG_RDX,
    REG_RSI,
    REG_RDI,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    REG_RBP,
    REG_RSP
};

struct emitter_t
{
    FILE *file;
};

lstatus_t emitter_construct(emitter_t *emt);
lstatus_t emitter_destruct(emitter_t *emt);

lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, int imm_src);
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src_base, int src_offset);
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst_base, int dst_offset, amd64_reg_t src);
lstatus_t emit_add  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_add  (emitter_t *emt, amd64_reg_t dst, int imm_src);
lstatus_t emit_sub  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_sub  (emitter_t *emt, amd64_reg_t dst, int imm_src);
lstatus_t emit_imul (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_idiv (emitter_t *emt, amd64_reg_t divider);
lstatus_t emit_xor  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_cmp  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_test (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);
lstatus_t emit_sete (emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_setne(emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_setl (emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_setg (emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_setle(emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_setge(emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_cdq  (emitter_t *emt);
lstatus_t emit_push (emitter_t *emt, amd64_reg_t src);
lstatus_t emit_pop  (emitter_t *emt, amd64_reg_t dst);
lstatus_t emit_call (emitter_t *emt, string_view_t label);
lstatus_t emit_call (emitter_t *emt, const char *label);
lstatus_t emit_ret  (emitter_t *emt);
lstatus_t emit_jmp  (emitter_t *emt, const char *label);
lstatus_t emit_jz   (emitter_t *emt, const char *label);

lstatus_t emit_label(emitter_t *emt, string_view_t label);
lstatus_t emit_label(emitter_t *emt, const char *label);

lstatus_t emit_comment(emitter_t *emt, const char *comment);

#endif