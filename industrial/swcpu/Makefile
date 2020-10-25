CC=clang
CFLAGS=-g -fsanitize=address
export CC, CFLAGS

build:
	make -C assm/
	make -C disassm/
	make -C cpu/

clean:
	make -C assm/    clean
	make -C disassm/ clean
	make -C cpu/     clean
