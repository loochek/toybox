CC=clang
CFLAGS=-g -fsanitize=address -DFRAMEBUFFER_ENABLE
export CC
export CFLAGS

build:
	make -C assm/
	make -C disassm/
	make -C cpu/

clean:
	make -C assm/    clean
	make -C disassm/ clean
	make -C cpu/     clean
