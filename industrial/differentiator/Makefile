CC=clang++
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.cpp diff.cpp simpler.cpp expr_tree.cpp parser.cpp node_pool.cpp file_utils.cpp lerror.cpp -o equator

clean:
	rm -f equator expr.dot expr.svg expr.log expr.aux expr.pdf expr.tex result.txt
