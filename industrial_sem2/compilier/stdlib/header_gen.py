#!/usr/bin/python3

from elftools.elf.elffile  import ELFFile
from elftools.elf.sections import SymbolTableSection

HEADER_BEGIN = """#ifndef STDLIB_BLOB_HPP
#define STDLIB_BLOB_HPP

#include <cstdint>
#include "../binary_emitter.hpp"

/**
 * WARNING: This is an auto-generated file!
 * 
 * This blob contains binary code of a standart library
 * Addresses in the symbol table are relative to the start of the blob
 */

const uint8_t stdlib_blob[]  = { """

HEADER_MIDDLE = """ };

symbol_t stdlib_symbols[] = {"""

SYMBOL_TEMPLATE = """{
"%s",
0x%x
},"""

HEADER_END = """};

#endif"""

header_file = open("stdlib_blob.hpp", "w")
elffile = ELFFile(open("stdlib.o", "rb"))

print(HEADER_BEGIN, file=header_file)

for section in elffile.iter_sections():
    if section.name == ".text":
        stdlib_blob = section.data()
        for i in range(len(stdlib_blob)):
            print("0x%02x, " % stdlib_blob[i], end="", file=header_file)
            if (i + 1) % 8 == 0:
                print(file=header_file)
            
print(HEADER_MIDDLE, file=header_file)
        

symbol_tables = [(idx, s) for idx, s in enumerate(elffile.iter_sections()) if isinstance(s, SymbolTableSection)]

for section_index, section in symbol_tables:
    if section['sh_entsize'] == 0:
        continue
    
    sym_num = section.num_symbols()
    for nsym, symbol in enumerate(section.iter_symbols()):
        if symbol['st_info']['type'] == "STT_FUNC":
            name = symbol.name
            offset = symbol['st_value']
            print(SYMBOL_TEMPLATE % (name, offset), file=header_file)
            
print(HEADER_END, file=header_file)

header_file.close()
