CC=clang

build: build_stack build_main
	$(CC) stack_impl.o main.o -o stack_test

double: build_stack build_main_double
	$(CC) stack_impl.o main.o -o stack_test

build_stack:
	$(CC) -c stack_impl.c -o stack_impl.o

build_main:
	$(CC) -c main.c -o main.o

build_main_double:
	$(CC) -c main.c -o main.o -DSTACK_TEST_DOUBLE

clean:
	rm -f stack_impl.o main.o stack_test