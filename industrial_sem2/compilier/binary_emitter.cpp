#include <cstdlib>
#include <cstring>
#include <sys/stat.h>

#include "binary_emitter.hpp"
#include "stdlib/stdlib_blob.hpp"
#include "elf.hpp"

static const char *regs64_str_repr[] =
{
    "rax",
    "rcx",
    "rdx",
    "rbx",
    "rsp",
    "rbp",
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
    "dummy"
};

static const char *regs8_str_repr[] =
{
    "al",
    "cl",
    "dl",
    "bl",
    "spl",
    "bpl",
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
    "dummy"
};

static const char *ENTRY_POINT_NAME = "_start";
static const char *START_FUNC_NAME  = "main";

static const int LOAD_ADDRESS = 0x400000;

static const int PRG_BUFFER_INIT_SIZE = 128;

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
 * Emits an argument of a register-indirect addressing mode with 32bit displacement:
 * 
 * ModRM:
 * ModRM.mod = 10
 * ModRM.reg = 3 least significant bits of reg
 * ModRM.rm  = 3 least significant bits of rm
 * 
 * if (rm == RSP || rm == R12), SIB is emitted:
 * SIB.scale = 0
 * SIB.index = RSP
 * SIB.base  = rm
 * 
 * Reg operand: reg
 * R/M operand: [rm_base + disp32]
 * 
 * Which operand is the source and which is the destination depends on the opcode
 * 
 * \param \c emt Emitter object
 * \param \c reg Reg
 * \param \c rm_base R/M base
 * \param \c rm_disp R/M displacement
 */
static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm_base, int32_t rm_disp);

/**
 * Emits an argument of a register-indirect addressing mode with
 * index register and 32bit displacement:
 * 
 * ModRM:
 * ModRM.mod = 10
 * ModRM.reg = 3 least significant bits of reg
 * ModRM.rm  = REG_RSP
 * 
 * SIB:
 * SIB.scale = rm_scale
 * SIB.index = rm_index
 * SIB.base  = rm_base
 * 
 * Imm: rm_disp
 * 
 * Reg operand: reg
 * R/M operand: [rm_base + rm_index * rm_scale + rm_disp32]
 * 
 * Which operand is the source and which is the destination depends on the opcode
 * 
 * \param \c emt Emitter object
 * \param \c reg Reg
 * \param \c rm_base R/M base
 * \param \c rm_index R/M index
 * \param \c rm_scale R/M scale
 * \param \c rm_disp R/M displacement
 */
static lstatus_t emit_operands(emitter_t *emt, reg64_t reg,
                               reg64_t rm_base, reg64_t rm_index, int rm_scale, int32_t rm_disp);

/**
 * Emits a REX byte:
 * REX.W = rex_w
 * REX.R = most significant bit of reg
 * REX.X = 0
 * REX.B = most significant bit of rm
 * 
 * \param \c emt Emitter object
 * \param \c rex_w REX.W value
 * \param \c reg Reg
 * \param \c index Index
 * \param \c rm R\M
 */
static lstatus_t emit_rex(emitter_t *emt, bool rex_w, reg64_t reg, reg64_t rm);

/**
 * Emits a REX byte:
 * REX.W = rex_w
 * REX.R = most significant bit of reg
 * REX.X = most significant bit of index
 * REX.B = most significant bit of rm
 * 
 * \param \c emt Emitter object
 * \param \c rex_w REX.W value
 * \param \c reg Reg
 * \param \c index Index
 * \param \c rm R\M
 */
static lstatus_t emit_rex(emitter_t *emt, bool rex_w, reg64_t reg, reg64_t index, reg64_t rm);

/**
 * Emits a SIB byte:
 * SIB.scale = log(scale)
 * SIB.index = 3 least significant bits of index
 * SIB.base  = 3 least significant bits of base
 * 
 * \param \c emt Emitter object
 * \param \c scale Scale (1, 2, 4 or 8)
 * \param \c index Index register
 * \param \c base Base register
 */
static lstatus_t emit_sib(emitter_t *emt, int scale, reg64_t index, reg64_t base);

/**
 * Emits program start code
 *
 * \param \c emt Emitter object
 */
static lstatus_t emit_start_code(emitter_t *emt);

/**
 * Emits standart library code
 *
 * \param \c emt Emitter object
 */
static lstatus_t emit_stdlib(emitter_t *emt);

/**
 * Finds a symbol in a symbol table
 * 
 * \param \c emt Emitter object
 * \param \c label Symbol of a label to find
 * \param \c addr_out Where to write address of a symbol
 */
static lstatus_t symbol_find(emitter_t *emt, const char *label, int *addr_out);

/**
 * Tries to resolve fixups using collected symbols
 * Provides pretty error if failed
 * 
 * \param \c emt Emitter object
 * \param \c comp_err Compilation error object
 */
static lstatus_t symbol_resolve(emitter_t *emt, compilation_error_t *comp_err);

/**
 * Expands program buffer
 * 
 * \param \c emt Emitter object
 */
static lstatus_t prg_buf_expand(emitter_t *emt);


#define LSCHK_LOCAL(expr, num) { status = expr; if (status != LSTATUS_OK) ERROR_HANDLER(num); }
#define ERROR_HANDLER(num) goto error_handler##num


lstatus_t emitter_construct(emitter_t *emt, const char *lst_file_name)
{
    lstatus_t status = LSTATUS_OK;

    emt->idle = false;
    emt->prg_buffer = nullptr;
    emt->curr_pc = 0;
    emt->prg_buffer_size = PRG_BUFFER_INIT_SIZE;

    emt->listing_file = fopen(lst_file_name, "w");
    if (emt->listing_file == nullptr)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to open listing file");
        ERROR_HANDLER(0);
    }

    emt->prg_buffer = (unsigned char*)calloc(PRG_BUFFER_INIT_SIZE, sizeof(unsigned char));
    if (emt->prg_buffer == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "");
        ERROR_HANDLER(1);
    }

    LSCHK_LOCAL(list_construct(&emt->fixups), 2);
    LSCHK_LOCAL(list_construct(&emt->symbol_table), 3);
    return LSTATUS_OK;


error_handler3:
    LSCHK(list_destruct(&emt->fixups));

error_handler2:
    free(emt->prg_buffer);
    
error_handler1:
    fclose(emt->listing_file);

error_handler0:
    return status;
}

lstatus_t emitter_destruct(emitter_t *emt)
{
    lstatus_t status = LSTATUS_OK;

    if (emt == nullptr)
        return LSTATUS_OK;

    LSCHK(list_destruct(&emt->fixups));
    LSCHK(list_destruct(&emt->symbol_table));

    free(emt->prg_buffer);

    if (fclose(emt->listing_file) != 0)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to close listing file");
        return status;
    }

    return LSTATUS_OK;
}

lstatus_t create_elf(emitter_t *emt, compilation_error_t *comp_err, const char *file_name)
{
    lstatus_t status = LSTATUS_OK;

    // complete program
    LSCHK(emit_start_code(emt));
    LSCHK(emit_stdlib(emt));

    // "link"
    LSCHK(symbol_resolve(emt, comp_err));

    // determine program entry address
    int prg_entry_addr = 0;
    LSCHK(symbol_find(emt, ENTRY_POINT_NAME, &prg_entry_addr));

    int elf_file_size = emt->curr_pc + sizeof(elf64_header_t) + sizeof(elf64_prg_header_t);

    // minimal elf definition:
    // - elf header
    // - single program header - tells the system to load ELF contents to the LOAD_ADDRESS
    // - our emitted program buffer

    // note that headers are also loaded into process memory

    elf64_header_t elf_header = {};
    elf_header.signature[0] = 0x7F;
    elf_header.signature[1] = 'E';
    elf_header.signature[2] = 'L';
    elf_header.signature[3] = 'F';

    elf_header.elf_class   = 0x02; // ELF64
    elf_header.endianess   = 0x01; // little-endian
    elf_header.version1    = 0x01; // version
    elf_header.abi         = 0x00; // System V
    elf_header.abi_version = 0x00; // not specified
    elf_header.type        = 0x2;  // ET_EXEC (executable)
    elf_header.machine     = 0x3E; // AMD x86-64
    elf_header.version2    = 0x01; // version again

    elf_header.entry_addr      = prg_entry_addr + LOAD_ADDRESS // entry address
                                 + sizeof(elf64_header_t) + sizeof(elf64_prg_header_t);
    elf_header.ph_table_offset = sizeof(elf64_header_t); // right after this header
    elf_header.sh_table_offset = 0x00; // no sections used
    elf_header.flags           = 0x00; // no flags
    elf_header.header_size     = sizeof(elf64_header_t);
    elf_header.ph_entry_size   = sizeof(elf64_prg_header_t);
    elf_header.ph_entries_num  = 0x01; // only one
    elf_header.sh_entry_size   = 0x00; // no sections used
    elf_header.sh_entries_num  = 0x00; // no sections used
    elf_header.shstrndx        = 0x00; // no sections used

    elf64_prg_header_t prg_header = {};
    prg_header.type            = 0x01;          // PT_LOAD
    prg_header.flags           = 0x01 | 0x04;   // execute and read
    prg_header.seg_file_offset = 0x00;          // load whole file
    prg_header.seg_virt_addr   = LOAD_ADDRESS;  // where to load
    prg_header.seg_phys_addr   = 0x0;           // irrelevant for AMD64
    prg_header.seg_file_size   = elf_file_size; // load whole file
    prg_header.seg_mem_size    = elf_file_size; // load whole file
    prg_header.alignment       = 0x200000;      // default

    bool write_failed = false;

    FILE *elf_file = fopen(file_name, "wb");
    if (elf_file == nullptr)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to open output ELF file");
        ERROR_HANDLER(0);
    }

    if (fwrite(&elf_header, sizeof(elf64_header_t), 1, elf_file) != 1)
        write_failed = true;

    if (fwrite(&prg_header, sizeof(elf64_prg_header_t), 1, elf_file) != 1)
        write_failed = true;

    if (fwrite(emt->prg_buffer, sizeof(uint8_t), emt->curr_pc, elf_file) != emt->curr_pc)
        write_failed = true;

    if (write_failed)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to write ELF file");
        ERROR_HANDLER(1);
    }

    if (fclose(elf_file) != 0)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to close output ELF file");
        ERROR_HANDLER(0);
    }

    if (chmod(file_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) != 0)
    {
        LSTATUS(LSTATUS_FILE_IO_ERR, "unable to set permissions for output ELF file");
        ERROR_HANDLER(0);
    }

    return LSTATUS_OK;

error_handler1:
    fclose(elf_file);

error_handler0:
    return status;
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

    LSCHK(emit_rex(emt, true, dst, (reg64_t)src));
    LSCHK(emit_byte(emt, 0x0F)); // MOVSX r64, r/m8
    LSCHK(emit_byte(emt, 0xBE)); // MOVSX r64, r/m8
    LSCHK(emit_operands(emt, dst, (reg64_t)src));

    fprintf(emt->listing_file, "    movsx %s, %s\n", regs64_str_repr[dst], regs8_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, int64_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, dst));
    LSCHK(emit_byte(emt, 0xB8 + (int)dst % 8)); // MOV r64, imm64
    LSCHK(emit_qword(emt, imm_src));

    fprintf(emt->listing_file, "    mov %s, %ld\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst, reg64_t src_base, int32_t src_offset)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, dst, src_base));
    LSCHK(emit_byte(emt, 0x8B)); // MOV r64,r/m64
    LSCHK(emit_operands(emt, dst, src_base, src_offset));

    fprintf(emt->listing_file, "    mov %s, [%s%+d]\n",
            regs64_str_repr[dst], regs64_str_repr[src_base], src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst,
                   reg64_t src_base, reg64_t src_index, int src_scale, int32_t src_offset)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, dst, src_index, src_base));
    LSCHK(emit_byte(emt, 0x8B)); // MOV r64,r/m64
    LSCHK(emit_operands(emt, dst, src_base, src_index, src_scale, src_offset));

    fprintf(emt->listing_file, "    mov %s, [%s+%s*%d%+d]\n",
            regs64_str_repr[dst], regs64_str_repr[src_base], regs64_str_repr[src_index], src_scale, src_offset);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst_base, int32_t dst_offset, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst_base));
    LSCHK(emit_byte(emt, 0x89)); // MOV r/m64,r64
    LSCHK(emit_operands(emt, src, dst_base, dst_offset));

    fprintf(emt->listing_file, "    mov [%s%+d], %s\n",
            regs64_str_repr[dst_base], dst_offset, regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_mov(emitter_t *emt, reg64_t dst_base, reg64_t dst_index, int dst_scale, int32_t dst_offset,
                   reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;
    
    LSCHK(emit_rex(emt, true, src, dst_index, dst_base));
    LSCHK(emit_byte(emt, 0x89)); // MOV r/m64,r64
    LSCHK(emit_operands(emt, src, dst_base, dst_index, dst_scale, dst_offset));

    fprintf(emt->listing_file, "    mov [%s+%s*%d%+d], %s\n",
            regs64_str_repr[dst_base], regs64_str_repr[dst_index], dst_scale, dst_offset, regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x01)); // ADD r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    add %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_add(emitter_t *emt, reg64_t dst, int32_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, dst));
    LSCHK(emit_byte(emt, 0x81)); // ADD r/m64, imm32
    LSCHK(emit_operands(emt, REG_RAX, dst)); // reg field must be 0 - opcode extension
    LSCHK(emit_dword(emt, imm_src));

    fprintf(emt->listing_file, "    add %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x29)); // SUB r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    sub %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sub(emitter_t *emt, reg64_t dst, int32_t imm_src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, dst));
    LSCHK(emit_byte(emt, 0x81)); // SUB r/m64, imm32
    LSCHK(emit_operands(emt, REG_RBP, dst)); // reg field must be 5 - opcode extension
    LSCHK(emit_dword(emt, imm_src));

    fprintf(emt->listing_file, "    sub %s, %d\n", regs64_str_repr[dst], imm_src);
    return LSTATUS_OK;
}

lstatus_t emit_imul(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, dst, src));
    LSCHK(emit_byte(emt, 0x0F)); // IMUL r64, r/m64
    LSCHK(emit_byte(emt, 0xAF)); // IMUL r64, r/m64
    LSCHK(emit_operands(emt, dst, src));

    fprintf(emt->listing_file, "    imul %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_idiv(emitter_t *emt, reg64_t divider)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, divider));
    LSCHK(emit_byte(emt, 0xF7)); // IDIV r/m64
    LSCHK(emit_operands(emt, REG_RDI, divider)); // reg field must be 7 - opcode extension

    fprintf(emt->listing_file, "    idiv %s\n", regs64_str_repr[divider]);
    return LSTATUS_OK;
}

lstatus_t emit_xor(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x31)); // XOR r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    xor %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_cmp(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x39)); // CMP r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    cmp %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_test(emitter_t *emt, reg64_t dst, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, src, dst));
    LSCHK(emit_byte(emt, 0x85)); // TEST r/m64,r64
    LSCHK(emit_operands(emt, src, dst));

    fprintf(emt->listing_file, "    test %s, %s\n", regs64_str_repr[dst], regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_sete(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETE r/m8
    LSCHK(emit_byte(emt, 0x94)); // SETE r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    sete %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setne(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETNE r/m8
    LSCHK(emit_byte(emt, 0x95)); // SETNE r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    setne %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setl(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETL r/m8
    LSCHK(emit_byte(emt, 0x9C)); // SETL r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    setl %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setg(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETG r/m8
    LSCHK(emit_byte(emt, 0x9F)); // SETG r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    setg %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setle(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETLE r/m8
    LSCHK(emit_byte(emt, 0x9E)); // SETLE r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    setle %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_setge(emitter_t *emt, reg8_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, false, REG_RAX, (reg64_t)dst));
    LSCHK(emit_byte(emt, 0x0F)); // SETGE r/m8
    LSCHK(emit_byte(emt, 0x9D)); // SETGE r/m8
    LSCHK(emit_operands(emt, REG_RAX, (reg64_t)dst));

    fprintf(emt->listing_file, "    setge %s\n", regs8_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_cqo(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, REG_RAX));
    LSCHK(emit_byte(emt, 0x99)); // CQO

    fprintf(emt->listing_file, "    cqo\n");
    return LSTATUS_OK;
}

lstatus_t emit_syscall(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_byte(emt, 0x0F)); // SYSCALL
    LSCHK(emit_byte(emt, 0x05)); // SYSCALL

    fprintf(emt->listing_file, "    syscall\n");
    return LSTATUS_OK;
}

lstatus_t emit_push(emitter_t *emt, reg64_t src)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, src));
    LSCHK(emit_byte(emt, 0x50 + (int)src % 8)); // PUSH r64

    fprintf(emt->listing_file, "    push %s\n", regs64_str_repr[src]);
    return LSTATUS_OK;
}

lstatus_t emit_pop(emitter_t *emt, reg64_t dst)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_rex(emt, true, REG_RAX, dst));
    LSCHK(emit_byte(emt, 0x58 + (int)dst % 8)); // PUSH r64

    fprintf(emt->listing_file, "    pop %s\n", regs64_str_repr[dst]);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;

    fixup_t fixup = {};
    fixup.fixup_addr = emt->curr_pc + 1;
    strncpy(fixup.label, label.str, label.length);

    LSCHK(emit_byte(emt, 0xE8)); // CALL rel32
    LSCHK(emit_dword(emt, 0x00)); // will be resolved later

    fixup.next_instr_addr = emt->curr_pc;
    LSCHK(list_push_front(&emt->fixups, fixup));

    fprintf(emt->listing_file, "    call %.*s\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_call(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    char label[MAX_LABEL_NAME_LEN + 1] = {};

    va_list args = {};
    va_start(args, label_fmt);
    vsnprintf(label, MAX_LABEL_NAME_LEN, label_fmt, args);
    va_end(args);

    fixup_t fixup = {};
    fixup.fixup_addr = emt->curr_pc + 1;
    strcpy(fixup.label, label);

    LSCHK(emit_byte(emt, 0xE8)); // CALL rel32
    LSCHK(emit_dword(emt, 0x00));

    fixup.next_instr_addr = emt->curr_pc;
    LSCHK(list_push_front(&emt->fixups, fixup));

    fprintf(emt->listing_file, "    call %s\n", label);
    return LSTATUS_OK;
}

lstatus_t emit_jmp(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    char label[MAX_LABEL_NAME_LEN + 1] = {};

    va_list args = {};
    va_start(args, label_fmt);
    vsnprintf(label, MAX_LABEL_NAME_LEN, label_fmt, args);
    va_end(args);

    fixup_t fixup = {};
    fixup.fixup_addr = emt->curr_pc + 1;
    strcpy(fixup.label, label);

    LSCHK(emit_byte(emt, 0xE9)); // JMP rel32
    LSCHK(emit_dword(emt, 0x00));

    fixup.next_instr_addr = emt->curr_pc;
    LSCHK(list_push_front(&emt->fixups, fixup));

    fprintf(emt->listing_file, "    jmp %s\n", label);
    return LSTATUS_OK;
}

lstatus_t emit_jz(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    char label[MAX_LABEL_NAME_LEN + 1] = {};

    va_list args = {};
    va_start(args, label_fmt);
    vsnprintf(label, MAX_LABEL_NAME_LEN, label_fmt, args);
    va_end(args);

    fixup_t fixup = {};
    fixup.fixup_addr = emt->curr_pc + 2;
    strcpy(fixup.label, label);

    LSCHK(emit_byte(emt, 0x0F)); // JZ rel32
    LSCHK(emit_byte(emt, 0x84)); // JZ rel32
    LSCHK(emit_dword(emt, 0x00));

    fixup.next_instr_addr = emt->curr_pc;
    LSCHK(list_push_front(&emt->fixups, fixup));

    fprintf(emt->listing_file, "    jz %s\n", label);
    return LSTATUS_OK;
}

lstatus_t emit_ret(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_byte(emt, 0xC3)); // RET near

    fprintf(emt->listing_file, "    ret\n\n");
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, string_view_t label)
{
    if (emt->idle)
        return LSTATUS_OK;

    symbol_t symbol = {};
    symbol.addr = emt->curr_pc;
    strncpy(symbol.label, label.str, label.length);
    LSCHK(list_push_front(&emt->symbol_table, symbol));
        
    fprintf(emt->listing_file, "%.*s:\n", label.length, label.str);
    return LSTATUS_OK;
}

lstatus_t emit_label(emitter_t *emt, const char *label_fmt, ...)
{
    if (emt->idle)
        return LSTATUS_OK;

    char label[MAX_LABEL_NAME_LEN + 1] = {};

    va_list args = {};
    va_start(args, label_fmt);
    vsnprintf(label, MAX_LABEL_NAME_LEN, label_fmt, args);
    va_end(args);

    symbol_t symbol = {};
    symbol.addr = emt->curr_pc;
    strcpy(symbol.label, label);
    LSCHK(list_push_front(&emt->symbol_table, symbol));

    fprintf(emt->listing_file, "%s:\n", label);
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

static lstatus_t emit_byte(emitter_t *emt, uint8_t byte)
{
    if (emt->idle)
        return LSTATUS_OK;

    lstatus_t status = LSTATUS_OK;

    while (emt->prg_buffer_size - emt->curr_pc < sizeof(uint8_t))
        LSCHK(prg_buf_expand(emt));

    memcpy(&emt->prg_buffer[emt->curr_pc], &byte, sizeof(uint8_t));
    emt->curr_pc += sizeof(uint8_t);

    return LSTATUS_OK;
}

static lstatus_t emit_dword(emitter_t *emt, int32_t dword)
{
    if (emt->idle)
        return LSTATUS_OK;

    lstatus_t status = LSTATUS_OK;

    while (emt->prg_buffer_size - emt->curr_pc < sizeof(int32_t))
        LSCHK(prg_buf_expand(emt));

    memcpy(&emt->prg_buffer[emt->curr_pc], &dword, sizeof(int32_t));
    emt->curr_pc += sizeof(int32_t);
    
    return LSTATUS_OK;
}

static lstatus_t emit_qword(emitter_t *emt, int64_t qword)
{
    if (emt->idle)
        return LSTATUS_OK;

    lstatus_t status = LSTATUS_OK;

    while (emt->prg_buffer_size - emt->curr_pc < sizeof(int64_t))
        LSCHK(prg_buf_expand(emt));

    memcpy(&emt->prg_buffer[emt->curr_pc], &qword, sizeof(int64_t));
    emt->curr_pc += sizeof(int64_t);
    
    return LSTATUS_OK;
}

static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm)
{
    if (emt->idle)
        return LSTATUS_OK;

    uint8_t modrm = 0xC0; // first two bits are 11
    modrm |= (reg & 0x7) << 3;
    modrm |= (rm & 0x7);

    LSCHK(emit_byte(emt, modrm));
    return LSTATUS_OK;
}

static lstatus_t emit_operands(emitter_t *emt, reg64_t reg, reg64_t rm_base, int32_t rm_disp)
{
    if (emt->idle)
        return LSTATUS_OK;

    uint8_t modrm = 0x80; // first two bits are 10
    modrm |= (reg     & 0x7) << 3;
    modrm |= (rm_base & 0x7);
    LSCHK(emit_byte(emt, modrm));
    
    if (rm_base == REG_RSP || rm_base == REG_R12)
        LSCHK(emit_sib(emt, 1, REG_RSP, rm_base));

    LSCHK(emit_dword(emt, rm_disp));
    return LSTATUS_OK;
}

static lstatus_t emit_operands(emitter_t *emt, reg64_t reg,
                               reg64_t rm_base, reg64_t rm_index, int rm_scale, int32_t rm_disp)
{
    lstatus_t status = LSTATUS_OK;

    if (emt->idle)
        return LSTATUS_OK;

    if (rm_index == REG_RSP)
    {
        LSTATUS(LSTATUS_BIN_ENC_FAIL, "RSP can't be used as index register");
        return status;
    }

    uint8_t modrm = 0x80; // first two bits are 10
    modrm |= (reg & 0x7) << 3;
    modrm |= REG_RSP;
    LSCHK(emit_byte(emt, modrm));

    LSCHK(emit_sib(emt, rm_scale, rm_index, rm_base));
    LSCHK(emit_dword(emt, rm_disp));
    return LSTATUS_OK;
}

static lstatus_t emit_rex(emitter_t *emt, bool rex_w, reg64_t reg, reg64_t rm)
{
    return emit_rex(emt, rex_w, reg, REG_RAX, rm);
}

static lstatus_t emit_rex(emitter_t *emt, bool rex_w, reg64_t reg, reg64_t index, reg64_t rm)
{
    if (emt->idle)
        return LSTATUS_OK;

    uint8_t rex = 0x40;
    rex |= rex_w << 3;
    rex |= (reg   & 0x8) >> 1;
    rex |= (index & 0x8) >> 2;
    rex |= (rm    & 0x8) >> 3;

    LSCHK(emit_byte(emt, rex));
    return LSTATUS_OK;
}

static lstatus_t emit_sib(emitter_t *emt, int scale, reg64_t index, reg64_t base)
{
    lstatus_t status = LSTATUS_OK;

    if (emt->idle)
        return LSTATUS_OK;
    
    if (scale != 1 && scale != 2 && scale != 4 && scale != 8)
    {
        LSTATUS(LSTATUS_BIN_ENC_FAIL, "scale must be 1, 2, 4 or 8");
        return status;
    }

    uint8_t sib = 0x00;

    // set scale
    if (scale == 2 || scale == 8)
        sib |= 0x1 << 6;
    if (scale == 4 || scale == 8)
        sib |= 0x1 << 7;

    sib |= (index & 0x7) << 3;
    sib |= (base  & 0x7);

    LSCHK(emit_byte(emt, sib));
    return LSTATUS_OK;
}

static lstatus_t emit_start_code(emitter_t *emt)
{
    if (emt->idle)
        return LSTATUS_OK;

    LSCHK(emit_label(emt, ENTRY_POINT_NAME));
    LSCHK(emit_call(emt, START_FUNC_NAME));
    LSCHK(emit_mov(emt, REG_RDI, REG_RAX));
    LSCHK(emit_mov(emt, REG_RAX, 0x3C)); // exit syscall
    LSCHK(emit_syscall(emt));
    
    return LSTATUS_OK;
}

static lstatus_t emit_stdlib(emitter_t *emt)
{
    lstatus_t status = LSTATUS_OK;

    if (emt->idle)
        return LSTATUS_OK;

    int blob_addr = emt->curr_pc;
    int blob_size = sizeof(stdlib_blob) / sizeof(uint8_t);

    while (emt->prg_buffer_size - emt->curr_pc < blob_size)
        LSCHK(prg_buf_expand(emt));

    memcpy(&emt->prg_buffer[emt->curr_pc], &stdlib_blob, blob_size);
    emt->curr_pc += blob_size;

    LSCHK(emit_comment(emt, "there must be a blob with definition of symbols:"));
    for (int i = 0; i < sizeof(stdlib_symbols) / sizeof(symbol_t); i++)
    {
        // fix offset and append to the symbol table
        symbol_t symbol = stdlib_symbols[i];
        symbol.addr += blob_addr;
        LSCHK(list_push_front(&emt->symbol_table, symbol));

        LSCHK(emit_comment(emt, "  - %s", symbol.label));
    }

    return LSTATUS_OK;
}

static lstatus_t symbol_find(emitter_t *emt, const char *label, int *addr_out)
{
    list_iter_t symbol_iter = NULLITER, symbol_end = NULLITER;
    LSCHK(list_begin(&emt->symbol_table, &symbol_iter));
    LSCHK(list_end  (&emt->symbol_table, &symbol_end));

    while (!list_iter_cmp(symbol_iter, symbol_end))
    {
        symbol_t *symbol = nullptr;
        LSCHK(list_data(&emt->symbol_table, symbol_iter, &symbol));

        if (strcmp(symbol->label, label) == 0)
        {
            *addr_out = symbol->addr;
            return LSTATUS_OK;
        }

        LSCHK(list_next(&emt->symbol_table, &symbol_iter));
    }

    return LSTATUS_NOT_FOUND;
}

static lstatus_t symbol_resolve(emitter_t *emt, compilation_error_t *comp_err)
{
    lstatus_t status = LSTATUS_OK;

    list_iter_t fixup_iter = NULLITER, fixup_end = NULLITER;
    LSCHK(list_begin(&emt->fixups, &fixup_iter));
    LSCHK(list_end  (&emt->fixups, &fixup_end));

    while (!list_iter_cmp(fixup_iter, fixup_end))
    {
        fixup_t *fixup = nullptr;
        LSCHK(list_data(&emt->fixups, fixup_iter, &fixup));

        list_iter_t symbol_iter = NULLITER, symbol_end = NULLITER;
        LSCHK(list_begin(&emt->symbol_table, &symbol_iter));
        LSCHK(list_end  (&emt->symbol_table, &symbol_end));

        int sym_addr = 0;
        status = symbol_find(emt, fixup->label, &sym_addr);
        if (status == LSTATUS_NOT_FOUND)
        {
            COMPILATION_ERROR(comp_err, -1, -1, "unable to resolve symbol %s", fixup->label);
            status = LSTATUS_SYM_RESOLVE_ERR;
            return status;
        }
        else if (status != LSTATUS_OK)
            return status;

        // resolving
        int32_t offset = sym_addr - fixup->next_instr_addr;
        memcpy(&emt->prg_buffer[fixup->fixup_addr], &offset, sizeof(int32_t));

        LSCHK(list_next(&emt->fixups, &fixup_iter));
    }

    return LSTATUS_OK;
}

static lstatus_t prg_buf_expand(emitter_t *emt)
{
    lstatus_t status = LSTATUS_OK;

    uint8_t *new_buf = (uint8_t*)realloc(emt->prg_buffer, emt->prg_buffer_size * 2 * sizeof(uint8_t));
    if (new_buf == nullptr)
    {
        LSTATUS(LSTATUS_BAD_ALLOC, "reallocation failed");
        return status;
    }

    emt->prg_buffer = new_buf;
    emt->prg_buffer_size *= 2;

    return LSTATUS_OK;
}