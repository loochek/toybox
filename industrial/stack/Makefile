CC=clang

build: build_stack build_main
	$(CC) stack_impl.o main.o -o stack_test

build_stack:
	$(CC) -c stack_impl.c -o stack_impl.o

build_main:
	$(CC) -c main.c -o main.o

clean:
	rm -f stack_impl.o main.o stack_test