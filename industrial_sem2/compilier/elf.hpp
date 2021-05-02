#include <cstdint>

struct __attribute__((__packed__)) elf64_header_t
{
    uint8_t  signature[4];
    uint8_t  elf_class; // must be 2 - ELF64
    uint8_t  endianess;
    uint8_t  version1;
    uint8_t  abi;
    uint8_t  abi_version;
    uint8_t  unused[7];
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint64_t entry_addr;
    uint64_t ph_table_offset;
    uint64_t sh_table_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_entries_num;
    uint16_t sh_entry_size;
    uint16_t sh_entries_num;
    uint16_t shstrndx;
};

struct __attribute__((__packed__)) elf64_prg_header_t
{
    uint32_t type;
    uint32_t flags;
    uint64_t seg_file_offset;
    uint64_t seg_virt_addr;
    uint64_t seg_phys_addr;
    uint64_t seg_file_size;
    uint64_t seg_mem_size;
    uint64_t alignment;
};