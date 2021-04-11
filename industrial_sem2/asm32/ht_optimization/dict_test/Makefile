CXX=clang++
CXXFLAGS=-O0 -DBENCHMARK -g
AS=nasm
ASFLAGS=-f elf64 -F dwarf -g

export CXX
export CXXFLAGS

build:
	$(MAKE) -C ../lstatus
	$(MAKE) -C ../dict
	$(MAKE) -C ../list
	$(CXX) $(CXXFLAGS) db.cpp dict_test.cpp ../lstatus/lstatus.o ../dict/dict.o ../list/list_common.o -o dict_test

build_asm_hash: asm
	$(MAKE) -C ../lstatus
	$(MAKE) -C ../dict
	$(MAKE) -C ../list
	$(CXX) $(CXXFLAGS) db.cpp dict_test.cpp ../lstatus/lstatus.o ../dict/dict.o ../list/list_common.o dict_calc_hash.o -o dict_test

build_asm_both: asm
	$(MAKE) -C ../lstatus
	$(MAKE) -C ../dict
	$(MAKE) -C ../list
	$(CXX) $(CXXFLAGS) db.cpp dict_test.cpp ../lstatus/lstatus.o ../dict/dict.o ../list/list_common.o dict_calc_hash.o dict_find_ring.o -o dict_test
 
asm:
	$(AS) $(ASFLAGS) dict_find_ring.asm
	$(AS) $(ASFLAGS) dict_calc_hash.asm

build build_asm_hash build_asm_both: CXXFLAGS+=$(ARG)

build_asm_hash build_asm_both: CXXFLAGS+=-DDICT_CALC_HASH_ASM

build_asm_both: CXXFLAGS+=-DDICT_FIND_RING_ASM

clean:
	$(MAKE) -C ../lstatus clean
	$(MAKE) -C ../list clean
	$(MAKE) -C ../dict clean
	rm -f dict_test *.o