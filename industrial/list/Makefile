CC=clang
CFLAGS=-fsanitize=address -g

build:
	$(CC) $(CFLAGS) list.c main.c lerror.c -o list_main

build_test:
	$(CC) $(CFLAGS) list.c tests.c lerror.c -o list_test

test: build_test
	./list_test

clean:
	rm -f list_test list_main list.dot images/* log.html
