CC=clang
CFLAGS=-O2
export CC, CFLAGS

build:
	make -C assm/
	make -C disassm/
	make -C cpu/

clean:
	make -C assm/    clean
	make -C disassm/ clean
	make -C cpu/     clean
