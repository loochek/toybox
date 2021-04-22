#ifndef BINARY_EMITTER_HPP
#define BINARY_EMITTER_HPP

#include "utils/lstatus.hpp"

enum register_t
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

};

lstatus_t emit_mov(emitter_t *emt, register_t dst, register_t src);
lstatus_t emit_mov(emitter_t *emt, register_t dst, int imm_src);
lstatus_t emit_mov(emitter_t *emt, register_t dst, register_t src_base, int src_offset);
lstatus_t emit_mov(emitter_t *emt, register_t dst_base, int dst_offset, register_t src);
lstatus_t emit_add(emitter_t *emt, register_t dst, register_t src);
lstatus_t emit_sub(emitter_t *emt, register_t dst, register_t src);
lstatus_t emit_imul(emitter_t *emt, register_t dst, register_t src);
lstatus_t emit_idiv(emitter_t *emt, register_t divider);
lstatus_t emit_cmp(emitter_t *emt, register_t dst, register_t src);
lstatus_t emit_sete(emitter_t *emt, register_t dst);
lstatus_t emit_setne(emitter_t *emt, register_t dst);
lstatus_t emit_setl(emitter_t *emt, register_t dst);
lstatus_t emit_setg(emitter_t *emt, register_t dst);
lstatus_t emit_setle(emitter_t *emt, register_t dst);
lstatus_t emit_setge(emitter_t *emt, register_t dst);
lstatus_t emit_cdq(emitter_t *emt);
lstatus_t emit_push(emitter_t *emt, register_t src);
lstatus_t emit_pop(emitter_t *emt, register_t dst);

#endif