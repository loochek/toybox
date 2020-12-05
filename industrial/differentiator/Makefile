CC=clang
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.c diff.c simpler.c expr_tree.c parser.c file_utils.c lerror.c -o differ

clean:
	rm -f differ expr.dot expr.svg expr.log expr.aux expr.pdf expr.tex 
