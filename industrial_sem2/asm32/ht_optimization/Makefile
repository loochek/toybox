CC=clang
CFLAGS=-O0 -g
export CC
export CFLAGS

list_test: build_list_test
	./list_test

build_list_test:
	$(MAKE) -C lstatus
	$(MAKE) -C list build tests.o
	$(CC) list/list.o list/tests.o lstatus/lstatus.o -o list_test

clean:
	$(MAKE) -C lstatus clean
	$(MAKE) -C list clean
	rm -f list_test