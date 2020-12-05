CC=clang++
CFLAGS=-g -fsanitize=address

build:
	$(CC) $(CFLAGS) main.cpp diff.cpp simpler.cpp expr_tree.cpp parser.cpp node_pool.cpp file_utils.cpp lerror.cpp -o differ

clean:
	rm -f differ expr.dot expr.svg expr.log expr.aux expr.pdf expr.tex dump.txt

