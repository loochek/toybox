CC=clang
CFLAGS=-O3 -DFRAMEBUFFER -DBUILD_NUMBER=$(shell cat build_number.txt)
export CC
export CFLAGS

build: update_build_number
	make -C assm/
	make -C disassm/
	make -C cpu/

clean:
	make -C assm/    clean
	make -C disassm/ clean
	make -C cpu/     clean

update_build_number:
	@echo $$(($$(cat build_number.txt) + 1)) > build_number.txt
	
