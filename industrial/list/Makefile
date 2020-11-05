CC=clang
CFLAGS=-fsanitize=address

build:
	$(CC) $(CFLAGS) list.c main.c -o list_main

build_test:
	$(CC) $(CFLAGS) list.c tests.c -o list_test -g

test: build_test
	./list_test

clean:
	rm -f list_test list_main list.dot out.png