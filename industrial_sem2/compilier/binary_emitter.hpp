#ifndef BINARY_EMITTER_HPP
#define BINARY_EMITTER_HPP

#include <cstdio>
#include <cstdint>
#include "utils/lstatus.hpp"
#include "utils/string_view.hpp"
#include "utils/list.hpp"
#include "compilation_error.hpp"

/**
 * Provides an interface for generating binary files from a given assembly sequence.
 * Supports internal symbols resolving and generating debug listings 
 */

const int REG64_COUNT = 17;

const int MAX_LABEL_NAME_LEN = 30;

enum reg64_t
{
    REG_RAX,
    REG_RCX,
    REG_RDX,
    REG_RBX,
    REG_RSP,
    REG_RBP,
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
    REG_DUMMY64,
};

enum reg8_t
{
    REG_AL,
    REG_CL,
    REG_DL,
    REG_BL,
    REG_SPL,
    REG_BPL,
    REG_SIL,
    REG_DIL,
    REG_R8B,
    REG_R9B,
    REG_R10B,
    REG_R11B,
    REG_R12B,
    REG_R13B,
    REG_R14B,
    REG_R15B,
    REG_DUMMY8
};

/**
 * "Symbol resolution request"
 * created by instructions like call, jmp, ...
 */
struct fixup_t
{
    char label[MAX_LABEL_NAME_LEN + 1];

    int fixup_addr;
    int next_instr_addr;
};

/**
 * Thing that can resolve fixups
 * created by labels
 */
struct symbol_t
{
    char label[MAX_LABEL_NAME_LEN + 1];
    int addr;
};

/**
 * Binary emitter object
 */
struct emitter_t
{
    FILE *listing_file;
    
    // emit functions are dummies if idle mode is enabled
    // used for pseudo-compiling
    bool idle;

    // program buffer stuff
    uint8_t *prg_buffer;
    int prg_buffer_size;

    // current position of a "program counter"
    int curr_pc;

    list_t<symbol_t> symbol_table;
    list_t<fixup_t>  fixups;
};

/**
 * Initializes emitter object
 * 
 * \param \c emt Emitter object
 * \param \c lst_file_name Listing file name
 */
lstatus_t emitter_construct(emitter_t *emt, const char *lst_file_name);

/**
 * Deinitializes emitter object
 * 
 * \param \c emt Emitter object
 */
lstatus_t emitter_destruct(emitter_t *emt);

/**
 * Resolves all symbols resolution requests and creates an ELF file with emitted code and standart library
 * Start code calls function called main and does exit syscall with a return value of main
 * 
 * \param \c emt Emitter object
 * \param \c comp_err Compilation error object
 * \param \c file_name Output file name
 */
lstatus_t create_elf(emitter_t *emt, compilation_error_t *comp_err, const char *file_name);

/**
 * mov %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * movsx %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_movsx(emitter_t *emt, reg64_t dst, reg8_t src);

/**
 * mov %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_mov(emitter_t *emt, reg64_t dst, int64_t imm_src);

/**
 * mov %dst, [%src_base + src_offset]
 *
 * \param \c dst Destination register
 * \param \c src_base Memory base register
 * \param \c src_offset Memory offset immediate value
 * \param \c emt Emitter object
 */
lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src_base, int32_t src_offset);

/**
 * mov [%dst_base + dst_offset], %src
 *
 * \param \c dst_base Memory base register
 * \param \c dst_offset Memory offset immediate value
 * \param \c src Source register
 * \param \c emt Emitter object
 */
lstatus_t emit_mov(emitter_t *emt, reg64_t dst_base, int32_t dst_offset, reg64_t src);

/**
 * add %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_add(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * add %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_add(emitter_t *emt, reg64_t dst, int32_t imm_src);

/**
 * sub %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_sub(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * sub %dst, imm_src
 *
 * \param \c dst Destination register
 * \param \c src Immediate source value
 * \param \c emt Emitter object
 */
lstatus_t emit_sub(emitter_t *emt, reg64_t dst, int32_t imm_src);

/**
 * imul %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_imul(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * idiv %divider
 * 
 * \param \c divider Divider register
 * \param \c emt Emitter obj	Contains index of the section header table entry that contains the section names.
0x34	0x40	ect
 */
lstatus_t emit_idiv(emitter_t *emt, reg64_t divider);

/**
 * xor %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_xor(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * cmp %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_cmp(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * test %dst, %src
 * 
 * \param \c src Source register
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_test(emitter_t *emt, reg64_t dst, reg64_t src);

/**
 * sete %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_sete(emitter_t *emt, reg8_t dst);

/**
 * setne %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_setne(emitter_t *emt, reg8_t dst);

/**
 * setl %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_setl(emitter_t *emt, reg8_t dst);

/**
 * setg %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_setg(emitter_t *emt, reg8_t dst);

/**
 * setle %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_setle(emitter_t *emt, reg8_t dst);

/**
 * setge %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_setge(emitter_t *emt, reg8_t dst);

/**
 * cqo
 *
 * \param \c emt Emitter object
 */
lstatus_t emit_cqo(emitter_t *emt);

/**
 * syscall
 *
 * \param \c emt Emitter object
 */
lstatus_t emit_syscall(emitter_t *emt);

/**
 * push %src
 *
 * \param \c dst Source register
 * \param \c emt Emitter object
 */
lstatus_t emit_push(emitter_t *emt, reg64_t src);

/**
 * pop %dst
 *
 * \param \c dst Destination register
 * \param \c emt Emitter object
 */
lstatus_t emit_pop(emitter_t *emt, reg64_t dst);

/**
 * call label
 *
 * \param \c label Label
 * \param \c emt Emitter object
 */
lstatus_t emit_call(emitter_t *emt, string_view_t label);

/**
 * call label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_call(emitter_t *emt, const char *label_fmt, ...);

/**
 * jmp label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_jmp(emitter_t *emt, const char *label_fmt, ...);

/**
 * jz label_fmt
 * (formatted label)
 *
 * \param \c emt Emitter object
 * \param \c label_fmt Label
 * \param \c ... Format args
 */
lstatus_t emit_jz(emitter_t *emt, const char *label_fmt, ...);

/**
 * ret
 *
 * \param \c emt Emitter object
 */
lstatus_t emit_ret(emitter_t *emt);

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

/**
 * Returns a LSB of a given 64bit register
 * 
 * \param \c reg64 Register
 */
reg8_t reg64_to_8(reg64_t reg64);

#endif