CC=clang
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.c lexer.c ast.c parser.c assm_gen.c string_view.c node_pool.c stack/stack_impl.c lerror.c file_utils.c -o tc

clean:
	rm -f tc tree.dot tree.svg