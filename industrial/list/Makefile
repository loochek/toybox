CC=clang
CFLAGS=-fsanitize=address

build:
	$(CC) $(CFLAGS) list.c main.c -o list_test

clean:
	rm -f list_test list.dot out.png