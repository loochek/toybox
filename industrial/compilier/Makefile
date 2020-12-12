CC=clang
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.c lexer.c ast.c parser.c node_pool.c lerror.c file_utils.c -o lexer
