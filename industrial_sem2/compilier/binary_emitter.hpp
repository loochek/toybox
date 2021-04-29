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
    REG_RSP,

    // REG_AL,
    // REG_BL,
    // REG_CL,
    // REG_DL,
    // REG_SIL,
    // REG_DIL,
    // REG_R8B,
    // REG_R9B,
    // REG_R10B,
    // REG_R11B,
    // REG_R12B,
    // REG_R13B,
    // REG_R14B,
    // REG_R15B,
    // REG_BPL,
    // REG_SPL,
};

/**
 * Binary emitter object
 * Generates debug assembly and produces ELF file
 */
struct emitter_t
{
    FILE *file;

    bool idle;
};

/**
 * Initializes emitter object
 * 
 * \param \c emt Emitter object
 */
lstatus_t emitter_construct(emitter_t *emt);

/**
 * Deinitializes emitter object
 * 
 * \param \c emt Emitter object
 */
lstatus_t emitter_destruct(emitter_t *emt);

/**
 * mov %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * mov %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, int imm_src);

/**
 * mov %dst, [%src_base + src_offset]
 *
 * \param \c dst Destination register
 * \param \c src_base Memory base register
 * \param \c src_offset Memory offset immediate value
 * \param \c emt Emitter object
 */
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src_base, int src_offset);

/**
 * mov [%dst_base + dst_offset], %src
 *
 * \param \c dst_base Memory base register
 * \param \c dst_offset Memory offset immediate value
 * \param \c src Source register
 * \param \c emt Emitter object
 */
lstatus_t emit_mov  (emitter_t *emt, amd64_reg_t dst_base, int dst_offset, amd64_reg_t src);

/**
 * add %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_add  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * add %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_add  (emitter_t *emt, amd64_reg_t dst, int imm_src);

/**
 * sub %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_sub  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * sub %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_sub  (emitter_t *emt, amd64_reg_t dst, int imm_src);

/**
 * imul %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_imul (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * idiv %divider
 * 
 * \param \c divider Divider register
 * \param \c emt Emitter object
 */
lstatus_t emit_idiv (emitter_t *emt, amd64_reg_t divider);

/**
 * xor %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_xor  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmp %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmp  (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * test %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_test (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmove %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmove (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmovne %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmovne(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmovl %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmovl (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmovg %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmovg (emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmovle %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmovle(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cmovge %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmovge(emitter_t *emt, amd64_reg_t dst, amd64_reg_t src);

/**
 * cqo
 *
 * \param \c emt Emitter object
 */
lstatus_t emit_cqo  (emitter_t *emt);

/**
 * push %src
 *
 * \param \c dst Source register
 * \param \c emt Emitter object
 */
lstatus_t emit_push (emitter_t *emt, amd64_reg_t src);

/**
 * pop %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_pop  (emitter_t *emt, amd64_reg_t dst);

/**
 * call label
 *
 * \param \c label Label
 * \param \c emt Emitter object
 */
lstatus_t emit_call (emitter_t *emt, string_view_t label);

/**
 * call label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_call (emitter_t *emt, const char *label_fmt, ...);

/**
 * jmp label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_jmp  (emitter_t *emt, const char *label_fmt, ...);

/**
 * jz label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_jz   (emitter_t *emt, const char *label_fmt, ...);

/**
 * ret
 *
 * \param \c emt Emitter object
 */
lstatus_t emit_ret  (emitter_t *emt);

/**
 * label:
 *
 * \param \c emt Emitter object
 * \param \c label Label
 */
lstatus_t emit_label(emitter_t *emt, string_view_t label);

/**
 * label_fmt:
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_label(emitter_t *emt, const char *label_fmt, ...);

/**
 * ; comment_fmt
 * (formatted comment)
 *
 * \param \c emt Emitter object
 * \param \c comment_fmt Comment
 * \param \c ... Format args
 */
lstatus_t emit_comment(emitter_t *emt, const char *comment_fmt, ...);

#endif